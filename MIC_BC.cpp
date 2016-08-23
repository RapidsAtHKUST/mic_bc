/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_BC.h"

#pragma offload_attribute (push,target(mic))
int *R;
int *F;
int *C;
int *weight;
int *which_comp;
float *result_mic;
#pragma offload_attribute (pop)

MIC_BC::MIC_BC(Graph *g, int num_cores, int mode) {

    current_node = 0;
    n = g->n;
    m = g->m;
    //in case g->m==0, _mm_malloc cannot allocate zero or nagetive length
    if (m <= 0)
        m = 1;
    this->num_cores = num_cores;
    this->g = g;
    this->mode = mode;
    result.resize(n, 0);

    R = (int *) _mm_malloc(sizeof(int) * (n + 1), 64);
    F = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
    C = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
    weight = (int *) _mm_malloc(sizeof(int) * n, 64);
    which_comp = (int *) _mm_malloc(sizeof(int) * n, 64);
    if (mode == 1) {
        std::memcpy(weight, g->weight, sizeof(int) * n);
        std::memcpy(which_comp, g->which_components, sizeof(int) * n);
    } else {
        std::fill_n(weight, n, 1);
        std::fill_n(which_comp, n, 0);
    }

    result_mic = (float *) _mm_malloc(sizeof(float) * n * num_cores + 100, 64);

    std::memcpy(R, g->R, sizeof(int) * (n + 1));
    std::memcpy(F, g->F, sizeof(int) * (m * 2));
    std::memcpy(C, g->C, sizeof(int) * (m * 2));


    std::memset(result_mic, 0, sizeof(float) * n * num_cores + 100);
    transfer_to_mic();

}

void MIC_BC::transfer_to_mic() {

#pragma offload target(mic:0)\
			in(R[0:n+1] : ALLOC)\
			in(F[0:m*2] : ALLOC)\
			in(C[0:m*2] : ALLOC)\
            in(weight[0:n]: ALLOC)\
            in(which_comp[0:n]: ALLOC)\
			in(result_mic[0:n*num_cores ] : ALLOC)
    {
    }


}

std::vector<float> MIC_BC::node_parallel() {

#pragma offload target(mic:0)\
	nocopy(R[0:n+1] : FREE)\
	nocopy(F[0:m*2] : FREE)\
	nocopy(C[0:m*2] : FREE)\
	out(result_mic[0:n*num_cores] : FREE)
    {
        MIC_Coarse_Parallel(n, m, R, F, C, result_mic, num_cores);

    }
    for (int i = 1; i < num_cores; i++)
        for (int j = 0; j < n; j++) {
            result_mic[j] += result_mic[i * n + j];
        }

    for (int i = 0; i < n; i++)
        result[i] = result_mic[i] / 2.0f;

    return result;

}

std::vector<float> MIC_BC::opt_bc() {

    if (mode == 2) {

#pragma offload target(mic:0)\
		nocopy(R[0:n+1] : FREE)\
		nocopy(F[0:m*2] : FREE)\
		nocopy(C[0:m*2] : FREE)\
        nocopy(weight[0:n]: FREE)\
        nocopy(which_comp[0:n]: FREE)\
		out(result_mic[0:n*num_cores] : FREE)
        {
            MIC_Opt_BC(n, m, R, F, C, weight, which_comp, result_mic, num_cores, true);
        }
    } else {
#pragma offload target(mic:0)\
		nocopy(R[0:n+1] : FREE)\
		nocopy(F[0:m*2] : FREE)\
		nocopy(C[0:m*2] : FREE)\
        nocopy(weight[0:n]: FREE)\
        nocopy(which_comp[0:n]: FREE)\
		out(result_mic[0:n*num_cores] : FREE)
        {
            MIC_Opt_BC(n, m, R, F, C, weight, which_comp, result_mic, num_cores, false);
        }
    }

    for (int i = 0; i < num_cores; i++) {
        for (int j = 0; j < n; j++) {
            result[j] += result_mic[i * n + j];
        }
    }

    if (mode == 1)
        for (int i = 0; i < n; i++) {
            result[i] += g->bc[i];
        }
    for (int i = 0; i < n; i++)
        result[i] = result[i] / 2.0f;

    return result;
}

std::vector<float> MIC_BC::hybird_opt_bc() {

    int start_cpu, start_mic, end_cpu, end_mic;

    while (current_node < n) {
        int tmp;
        tmp = get_range(&start_cpu, &end_cpu, 40);
        printf("CPU : %d -> %d : %d\n", start_cpu, end_cpu, tmp);
        tmp = get_range(&start_mic, &end_mic, 240);
        printf("MIC : %d -> %d : %d\n", start_mic, end_mic, tmp);
    }

    return result;
}

int MIC_BC::get_range(int *start, int *end, int want) {
    lock.lock();
    if (current_node >= n)
        return 0;
    *start = current_node;
    int tmp = current_node + want;
    *end = tmp < n ? tmp : n;
    current_node = *end;
    lock.unlock();
    return *end - *start;

}

MIC_BC::~MIC_BC() {
    _mm_free(R);
    _mm_free(F);
    _mm_free(C);
    _mm_free(weight);
// TODO Auto-generated destructor stub
}

