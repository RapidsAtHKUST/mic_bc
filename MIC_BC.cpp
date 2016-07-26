/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_BC.h"
#include "Utils.h"

#define DIAMETER_SAMPLES 512

#pragma offload_attribute (push,target(mic))
namespace MIC {
int *R;
int *F;
int *C;
int *weight;
float *result_mic;

}
#pragma offload_attribute (pop)

using namespace MIC;

MIC_BC::MIC_BC(Graph g, int num_cores) {

	current_node = 0;
	n = g.n;
	m = g.m;
	this->num_cores = num_cores;

	//std::cout << n << " " << m << " " << this->num_cores << std::endl;

	R = (int *) _mm_malloc(sizeof(int) * (n + 1), 64);
	F = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
	C = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
#ifdef REDUCE_ONE_DEG
    weight = (int *) _mm_malloc(sizeof(int) * n, 64);
#endif
	result_mic = (float *) _mm_malloc(sizeof(float) * n * num_cores, 64);

	std::memcpy(R, g.R, sizeof(int) * (n + 1));
	std::memcpy(F, g.F, sizeof(int) * (m * 2));
	std::memcpy(C, g.C, sizeof(int) * (m * 2));

#ifdef REDUCE_ONE_DEG
    std::memcpy(weight, g.weight, sizeof(int) *n);
#endif

	std::memset(result_mic, 0, sizeof(float) * n * num_cores);
	transfer_to_mic();

}
void MIC_BC::transfer_to_mic() {
#ifdef REDUCE_ONE_DEG
#pragma offload target(mic:0)\
			in(R[0:n+1] : ALLOC)\
			in(F[0:m*2] : ALLOC)\
			in(C[0:m*2] : ALLOC)\
            in(weight[0:n]: ALLOC)\
			in(result_mic[0:n*num_cores] : ALLOC)
	{
	}
#else
#pragma offload target(mic:0)\
			in(R[0:n+1] : ALLOC)\
			in(F[0:m*2] : ALLOC)\
			in(C[0:m*2] : ALLOC)\
			in(result_mic[0:n*num_cores] : ALLOC)
    {
    }
#endif

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
		result.push_back(result_mic[i] / 2.0f);

	return result;

}

std::vector<float> MIC_BC::opt_bc() {

#ifdef REDUCE_ONE_DEG
#pragma offload target(mic:0)\
		nocopy(R[0:n+1] : FREE)\
		nocopy(F[0:m*2] : FREE)\
		nocopy(C[0:m*2] : FREE)\
        nocopy(weight[0:n]: FREE)\
		out(result_mic[0:n*num_cores] : FREE)
    {
		MIC_Opt_BC(n, m, R, F, C, weight, result_mic, num_cores, true);
	}
#else
#pragma offload target(mic:0)\
		nocopy(R[0:n+1] : FREE)\
		nocopy(F[0:m*2] : FREE)\
		nocopy(C[0:m*2] : FREE)\
		out(result_mic[0:n*num_cores] : FREE)
    {
        MIC_Opt_BC(n, m, R, F, C, R, result_mic, num_cores, true);
    }
#endif


#ifdef KAHAN
	std::vector<float> c(n, 0);
#endif
	for (int i = 1; i < num_cores; i++) {
		for (int j = 0; j < n; j++) {
#ifdef KAHAN
			KahanSum(&result_mic[j],&c[j],result_mic[i * n + j]);
#else
			result_mic[j] += result_mic[i * n + j];
#endif

		}
	}
	for (int i = 0; i < n; i++)
		result.push_back(result_mic[i] / 2.0f);

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
// TODO Auto-generated destructor stub
}

