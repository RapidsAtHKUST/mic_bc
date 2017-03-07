/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_BC.h"

#ifndef KNL

#pragma offload_attribute (push,target(mic))

#include <sys/time.h>

int num_cores;
uint32_t mode;
int n;
int m;
bool is_small_diameter;
int *R;
int *F;
int *C;
int *weight;
int *which_comp;
float *result_mic;
#pragma offload_attribute (pop)

MIC_BC::MIC_BC(Graph g, int num_cores_t, bool small_diameter, uint32_t mode_t) {

    current_node = 0;
    n = g.n;
    m = g.m;
    //in case g->m==0, _mm_malloc cannot allocate zero or nagetive length
    if (m <= 0)
        m = 1;
    num_cores = num_cores_t;
    this->g = g;
    is_small_diameter = small_diameter;
    mode = mode_t;
    result.resize(n);
    std::fill_n(result.begin(), n, 0);

    R = (int *) _mm_malloc(sizeof(int) * (n + 1), 64);
    F = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
    C = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
    weight = (int *) _mm_malloc(sizeof(int) * n, 64);
    which_comp = (int *) _mm_malloc(sizeof(int) * n, 64);
    if (mode & MIC_OFF_1_DEG) {
        std::memcpy(weight, g.weight, sizeof(int) * n);
        std::memcpy(which_comp, g.which_components, sizeof(int) * n);
    } else {
        for (int i = 0; i < n; i++)
            weight[i] = 1;
        std::memset(which_comp, 0, sizeof(int) * n);
    }

    result_mic = (float *) _mm_malloc(sizeof(float) * (n + 100) * 256, 64);

    std::memcpy(R, g.R, sizeof(int) * (n + 1));
    std::memcpy(F, g.F, sizeof(int) * (m * 2));
    std::memcpy(C, g.C, sizeof(int) * (m * 2));


    std::memset(result_mic, 0, sizeof(float) * n * num_cores + 100);
    transfer_to_mic();

}

void MIC_BC::transfer_to_mic() {

#pragma offload target(mic:0)\
            in(n)\
            in(m)\
            in(is_small_diameter)\
			in(R[0:n+1] : ALLOC)\
			in(F[0:m*2] : ALLOC)\
			in(C[0:m*2] : ALLOC)\
            in(weight[0:n]: ALLOC)\
            in(which_comp[0:n]: ALLOC)\
			in(result_mic[0:(n+100)*256] : ALLOC)
    {
    }


}

std::vector<float> MIC_BC::node_parallel() {

#pragma offload target(mic:0)\
	nocopy(R[0:n+1] : FREE)\
	nocopy(F[0:m*2] : FREE)\
	nocopy(C[0:m*2] : FREE)\
	out(result_mic[0:(n+100)*256] : FREE)
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

#pragma offload target(mic:0)\
		nocopy(R[0:n+1] : FREE)\
		nocopy(F[0:m*2] : FREE)\
		nocopy(C[0:m*2] : FREE)\
        nocopy(weight[0:n]: FREE)\
        nocopy(which_comp[0:n]: FREE)\
		out(result_mic[0:(n+100)*256] : FREE)
    {
        timeval start_wall_time_t, end_wall_time_t;
        float ms_wall, init_t, s1_t, s2_t;
#ifdef STAGET
        gettimeofday(&start_wall_time_t, nullptr);
        MIC_Opt_BC(n, m, R, F, C, weight, which_comp, result_mic, num_cores, INIT_T | mode);
        gettimeofday(&end_wall_time_t, nullptr);
        ms_wall = ((end_wall_time_t.tv_sec - start_wall_time_t.tv_sec) * 1000 * 1000
                   + end_wall_time_t.tv_usec - start_wall_time_t.tv_usec) / 1000.0;
        init_t = ms_wall/1000.0;
        std::cout << "\tinitial time: " << init_t << " s" << std::endl;

        gettimeofday(&start_wall_time_t, nullptr);
        MIC_Opt_BC(n, m, R, F, C, weight, which_comp, result_mic, num_cores, TRAVER_T | mode);
        gettimeofday(&end_wall_time_t, nullptr);
        ms_wall = ((end_wall_time_t.tv_sec - start_wall_time_t.tv_sec) * 1000 * 1000
                   + end_wall_time_t.tv_usec - start_wall_time_t.tv_usec) / 1000.0;
        s1_t = ms_wall/1000.0 - init_t;
        std::cout << "\ttraversal time: " << s1_t << " s" << std::endl;

        gettimeofday(&start_wall_time_t, nullptr);
        MIC_Opt_BC(n, m, R, F, C, weight, which_comp, result_mic, num_cores, mode);
        gettimeofday(&end_wall_time_t, nullptr);
        ms_wall = ((end_wall_time_t.tv_sec - start_wall_time_t.tv_sec) * 1000 * 1000
                   + end_wall_time_t.tv_usec - start_wall_time_t.tv_usec) / 1000.0;
        s2_t = ms_wall/1000.0 - init_t - s1_t;
        std::cout << "\taccumulation time: " << s2_t << " s\n" << std::endl;
        std::cout << "\ttotal time: " << ms_wall / 1000.0 << " s\n" << std::endl;
#else
        MIC_Opt_BC(n, m, R, F, C, weight, which_comp, result_mic, num_cores, is_small_diameter, mode);
#endif
    }

    for (int i = 0; i < num_cores; i++) {
        for (int j = 0; j < n; j++) {
            result[j] += result_mic[i * n + j];
        }
    }

    if (mode & MIC_OFF_1_DEG)
        for (int i = 0; i < n; i++) {
            result[i] += g.bc[i];
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

#endif /* KNL */