/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_BC.h"

MIC_BC::MIC_BC(Graph g) {
	// TODO Auto-generated constructor stub

	n = g.n;
	m = g.m;

	R = (int *) _mm_malloc(sizeof(int) * (n + 1), 64);
	F = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
	C = (int *) _mm_malloc(sizeof(int) * (m * 2), 64);
	result_mic = (float *) _mm_malloc(sizeof(float) * n, 64);

	std::memset(result_mic, 0, sizeof(result_mic));

	std::memcpy(R, g.R, n + 1);
	std::memcpy(F, g.F, m * 2);
	std::memcpy(C, g.C, m * 2);

	std::cout << "mic_bc" << std::endl;
//
//#pragma offload target(mic:0)\
//			in(R:length(n+1) alloc_if(1) free_if(0))\
//			in(F:length(m*2) alloc_if(1) free_if(0))\
//			in(C:length(n+1) alloc_if(1) free_if(0))\
//			in(result_mic:length(n) alloc_if(1) free_if(0))
//	{
//	}

}

void MIC_BC::node_parallel() {

#pragma offload target(mic)
	{
		MIC_Node_Parallel(0,0,nullptr,nullptr,nullptr,nullptr);
	}

#if 0
#pragma offload target(mic:0)
	{

		int end = n;

		for (int k = 0; k < end; k++) {
			int i = k;
			std::queue<int> Q;
			std::stack<int> S;
			std::vector<int> d(n, INT_MAX);
			d[i] = 0;
			std::vector<unsigned long long> sigma(n, 0);
			sigma[i] = 1;
			std::vector<float> delta(n, 0);
			Q.push(i);

			while (!Q.empty()) {
				int v = Q.front();
				Q.pop();
				S.push(v);
				for (int j = R[v]; j < R[v + 1]; j++) {
					int w = C[j];
					if (d[w] == INT_MAX) {
						Q.push(w);
						d[w] = d[v] + 1;
					}
					if (d[w] == (d[v] + 1)) {
						sigma[w] += sigma[v];
					}
				}
			}

			while (!S.empty()) {
				int w = S.top();
				S.pop();
				for (int j = R[w]; j < R[w + 1]; j++) {
					int v = C[j];
					if (d[v] == (d[w] - 1)) {
						delta[v] += (sigma[v] / (float) sigma[w])
								* (1 + delta[w]);
					}
				}

				if (w != i) {
					result_mic[w] += delta[w];
				}
			}
		}

		for (int i = 0; i < n; i++) {
			result_mic[i] /= 2.0f; //Undirected edges are modeled as two directed edges, but the scores shouldn't be double counted.
		}
	}
#endif
}

MIC_BC::~MIC_BC() {
// TODO Auto-generated destructor stub
}

