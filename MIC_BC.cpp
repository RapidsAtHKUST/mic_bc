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
float *result_mic;

float *bc_d, *delta_d;
int *d_d, *R_d, *C_d, *F_d, *Q_d, *Q2_d, *S_d, *endpoints_d, *next_source_d;
unsigned long long *sigma_d;
size_t pitch_d, pitch_sigma, pitch_delta, pitch_Q, pitch_Q2, pitch_S,
		pitch_endpoints;
int *diameters_d;
int *jia_d;

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
	result_mic = (float *) _mm_malloc(sizeof(float) * n * num_cores, 64);

	std::memcpy(R, g.R, sizeof(int) * (n + 1));
	std::memcpy(F, g.F, sizeof(int) * (m * 2));
	std::memcpy(C, g.C, sizeof(int) * (m * 2));

	d_d = (int *) _mm_malloc(sizeof(int) * n * num_cores, 64);
	sigma_d = (unsigned long long *) _mm_malloc(
			sizeof(unsigned long long) * n * num_cores, 64);
	delta_d = (float *) _mm_malloc(sizeof(float) * n * num_cores, 64);
	Q_d = (int *) _mm_malloc(sizeof(int) * n * num_cores, 64);
	Q2_d = (int *) _mm_malloc(sizeof(int) * n * num_cores, 64);
	S_d = (int *) _mm_malloc(sizeof(int) * n * num_cores, 64);
	endpoints_d = (int *) _mm_malloc(sizeof(int) * (n + 1) * num_cores, 64);

	diameters_d = (int *) _mm_malloc(sizeof(int) * DIAMETER_SAMPLES * num_cores,
			64);

	jia_d = (int *) _mm_malloc(sizeof(int) * n,64);

	std::memset(result_mic, 0, sizeof(float) * n * num_cores);
	std::memset(diameters_d, 0, sizeof(int) * DIAMETER_SAMPLES * num_cores);
	std::memset(jia_d, 0, sizeof(float) * n);
	transfer_to_mic();

}
void MIC_BC::transfer_to_mic() {
#pragma offload target(mic:0)\
			in(R[0:n+1] : ALLOC)\
			in(F[0:m*2] : ALLOC)\
			in(C[0:m*2] : ALLOC)\
			in(jia_d[0:n] : ALLOC)\
			in(result_mic[0:n*num_cores] : ALLOC)\
			in(d_d[0:n*num_cores] : ALLOC)\
			in(sigma_d[0:n*num_cores] : ALLOC)\
			in(delta_d[0:n*num_cores] : ALLOC)\
			in(Q_d[0:n*num_cores] : ALLOC)\
			in(Q2_d[0:n*num_cores] : ALLOC)\
			in(S_d[0:n*num_cores] : ALLOC)\
			in(endpoints_d[0:n*num_cores] : ALLOC)\
			in(diameters_d[0:DIAMETER_SAMPLES*num_cores] : ALLOC)
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
		MIC_Node_Parallel(n, m, R, F, C, result_mic);

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

//#pragma offload target(mic:0)\
		nocopy(R[0:n+1] : FREE)\
		nocopy(F[0:m*2] : FREE)\
		nocopy(C[0:m*2] : FREE)\
		in(jia_d[0:n] : ALLOC)\
		out(result_mic[0:n*num_cores] : FREE)\
		nocopy(d_d[0:n*num_cores] : FREE)\
		nocopy(sigma_d[0:n*num_cores] : FREE)\
		nocopy(delta_d[0:n*num_cores] : FREE)\
		nocopy(Q_d[0:n*num_cores] : FREE)\
		nocopy(Q2_d[0:n*num_cores] : FREE)\
		nocopy(S_d[0:n*num_cores] : FREE)\
		nocopy(endpoints_d[0:n*num_cores] : FREE)\
		nocopy(diameters_d[0:DIAMETER_SAMPLES*num_cores] : FREE)
	//{
	MIC_Opt_BC(n, m, R, F, C, result_mic, d_d, sigma_d, delta_d, Q_d, Q2_d, S_d,
			endpoints_d, jia_d, diameters_d, num_cores);
	//}
//	for (int i = 1; i < num_cores; i++) {
//		for (int j = 0; j < n; j++) {
//			result_mic[j] += result_mic[i * n + j];
//		}
//	}
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
	//free(result_mic);
	//free(R);
	//free(F);
	//free(C);
}

