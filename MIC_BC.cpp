/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_BC.h"

#pragma offload_attribute (push,target(mic))
namespace MIC {
	int *R;
	int *F;
	int *C;
	int n;
	int m;
	float *result_mic;
}
#pragma offload_attribute (pop)

using namespace MIC;

MIC_BC::MIC_BC(Graph g) {

	n = g.n;
	m = g.m;

	std::cout << n << " " << m << std::endl;

	R = (int *) _mm_malloc(sizeof(int) * (n + 1), 64);
	F = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
	C = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
	result_mic = (float *) _mm_malloc(sizeof(float) * n * n, 64);

	std::memset(result_mic, 0, sizeof(result_mic));

	std::memcpy(R, g.R, sizeof(int) * (n + 1));
	std::memcpy(F, g.F, sizeof(int) * (m * 2));
	std::memcpy(C, g.C, sizeof(int) * (m * 2));

	transfer_to_mic();

}
void MIC_BC::transfer_to_mic() {
#pragma offload target(mic:0)\
			in(R[0:n+1] : ALLOC)\
			in(F[0:m*2] : ALLOC)\
			in(C[0:m*2] : ALLOC)\
			in(result_mic[0:n*n] : ALLOC)
	{
	}
}

std::vector<float> MIC_BC::node_parallel() {


#pragma offload target(mic:0)\
	nocopy(R[0:n+1] : FREE)\
	nocopy(F[0:m*2] : FREE)\
	nocopy(C[0:m*2] : FREE)\
	out(result_mic[0:n*n] : FREE)
	{
		MIC_Node_Parallel(n, m, R, F, C, result_mic);

	}

	for(int i = 1 ; i < n; i++)
		for(int j = 0 ; j < n; j++)
			result_mic[j] += result_mic[i * n + j];
	for (int i = 0; i < n; i++)
		result.push_back(result_mic[i]/2.0f);

	return result;

}

MIC_BC::~MIC_BC() {
// TODO Auto-generated destructor stub
}

