/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_BC.h"

#define DIAMETER_SAMPLES 512

#pragma offload_attribute (push,target(mic))
namespace MIC {
int *R;
int *F;
int *C;
int n;
int m;
double *result_mic;

}
#pragma offload_attribute (pop)

using namespace MIC;

MIC_BC::MIC_BC(Graph g, int num_cores) {

	n = g.n;
	m = g.m;
	this->num_cores = num_cores;

	std::cout << n << " " << m << " " << this->num_cores << std::endl;

	R = (int *) _mm_malloc(sizeof(int) * (n + 1), 64);
	F = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
	C = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
	result_mic = (double *) _mm_malloc(sizeof(double) * n * num_cores, 64);

	std::memcpy(R, g.R, sizeof(int) * (n + 1));
	std::memcpy(F, g.F, sizeof(int) * (m * 2));
	std::memcpy(C, g.C, sizeof(int) * (m * 2));

	std::memset(result_mic, 0, sizeof(float) * n * num_cores);
	transfer_to_mic();

}
void MIC_BC::transfer_to_mic() {
#pragma offload target(mic:0)\
			in(R[0:n+1] : ALLOC)\
			in(F[0:m*2] : ALLOC)\
			in(C[0:m*2] : ALLOC)\
			in(result_mic[0:n*num_cores] : ALLOC)
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
		//MIC_Node_Parallel(n, m, R, F, C, result_mic);

	}
	for (int i = 1; i < MAX_MIC_CORE; i++)
		for (int j = 0; j < n; j++) {
			result_mic[j] += result_mic[i * n + j];
		}

	for (int i = 0; i < n; i++)
		result.push_back(result_mic[i] / 2.0f);

	return result;

}

std::vector<float> MIC_BC::opt_bc() {

#pragma offload target(mic:0)\
		nocopy(R[0:n+1] : FREE)\
		nocopy(F[0:m*2] : FREE)\
		nocopy(C[0:m*2] : FREE)\
		out(result_mic[0:n*num_cores] : FREE)
	{
		MIC_Opt_BC(n, m, R, F, C, result_mic, num_cores);
	}

	for (int i = 1; i < num_cores; i++) {
		for (int j = 0; j < n; j++) {
			result_mic[j] += result_mic[i * n + j];
		}
	}
	for (int i = 0; i < n; i++)
		result.push_back(result_mic[i] / 2.0f);

	return result;
}

MIC_BC::~MIC_BC() {
// TODO Auto-generated destructor stub
}

