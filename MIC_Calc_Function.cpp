/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_Calc_Function.h"

#pragma offload_attribute(push, target(mic))
#include <malloc.h>
#include <vector>
#include <queue>
#include <stack>
#include <iostream>
#include <cstring>
#include <omp.h>
//#include <cilk/cilk.h>
//#include <cilk/cilk_api.h>
//#include <tbb/atomic.h>
//#include <tbb/mutex.h>
#include <cmath>
#include <algorithm>
#include <mutex>
#include "Utils.h"
#pragma offload_attribute(pop)

__ONMIC__ void MIC_Node_Parallel(int n, int m, int* R, int* F, int* C,
		float* result_mic, int num_cores) {

#ifdef KAHAN
	std::vector<float> c(n * num_cores, 0.0);
#endif

	omp_set_num_threads(num_cores);
#pragma omp parallel for
	for (int k = 0; k < n; k++) {
		int i = k;
		int id_num = omp_get_thread_num();

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
					delta[v] += (sigma[v] / (float) sigma[w]) * (1 + delta[w]);
				}
			}
			if (w != i) {
#ifdef KAHAN
				KahanSum(&result_mic[id_num * n +w], &c[id_num*n+w], delta[w]);
#else
				result_mic[id_num * n + w] += delta[w];
#endif
			}
		}
	}
}

__ONMIC__ void MIC_WorkEfficient_Parallel(int n, int m, int* R, int* F, int* C,
		float* result_mic) {

}

__ONMIC__ void MIC_Opt_BC(const int n, const int m, const int *R, const int *F,
		const int *C, float *result_mic, const int num_cores) {

//将GPU的block看做是1(也就是编号0), 把GPU的thread对应成mic的thread
	omp_set_num_threads(num_cores);
	int *d_a[num_cores];
	unsigned long long *sigma_a[num_cores];
	float *delta_a[num_cores];
	int *Q2_a[num_cores], *Q_a[num_cores], *S_a[num_cores],
			*endpoints_a[num_cores];

	for (int i = 0; i < num_cores; i++) {
		d_a[i] = (int *) _mm_malloc(sizeof(int) * n, 64);
		sigma_a[i] = (unsigned long long *) _mm_malloc(
				sizeof(unsigned long long) * n, 64);
		delta_a[i] = (float *) _mm_malloc(sizeof(float) * n, 64);
		Q2_a[i] = (int *) _mm_malloc(sizeof(int) * n, 64);
		Q_a[i] = (int *) _mm_malloc(sizeof(int) * n, 64);
		endpoints_a[i] = (int *) _mm_malloc(sizeof(int) * (n + 1), 64);
		S_a[i] = (int *) _mm_malloc(sizeof(int) * n, 64);
	}
	float kahan_c[n];
	int thread_id;
#pragma omp parallel for private(thread_id)
	for (int ind = 0; ind < n; ind++) {

		thread_id = omp_get_thread_num();
		int start_point = ind;

		int *d = d_a[thread_id];
		unsigned long long *sigma = sigma_a[thread_id];
		float *delta = delta_a[thread_id];
		int *Q[2] = { Q_a[thread_id], Q2_a[thread_id] };
		int *S = S_a[thread_id];
		int *endpoints = endpoints_a[thread_id];

		for (int i = 0; i < n; i++) {
			sigma[i] = 0;
			delta[i] = 0;
			d[i] = INT_MAX;
		}
		kahan_c[start_point] = 0;

		int Q_len[2] = { 0, 0 };
		Q[0][0] = start_point;
		Q_len[0] = 1;
		int S_len = 0;

		sigma[start_point] = 1;
		d[start_point] = 0;

		endpoints[0] = 0;
		endpoints[1] = 1;
		int endpoints_len = 2;

		for (int select = 0;; select++) {
			unsigned short ch = select & 0x1;
			unsigned short rch = ~ch & 0x1;
			for (int i = 0; i < Q_len[ch]; i++) {
				int v = Q[ch][i];
				S[S_len] = v;
				S_len++;
				for (int j = R[v]; j < R[v + 1]; j++) {
					int w = C[j];
					if (d[w] == INT_MAX) {
						Q[rch][Q_len[rch]++] = w;
						d[w] = d[v] + 1;
					}
					if (d[w] == (d[v] + 1)) {
						sigma[w] += sigma[v];
					}
				}
			}
			if (Q_len[rch] == 0)
				break;
			Q_len[ch] = 0;
			endpoints[endpoints_len] = endpoints[endpoints_len - 1]
					+ Q_len[rch];
			endpoints_len++;
		}

		int current_depth = d[S[S_len -1]] -1;
		while (current_depth) {
			for (int kk = endpoints[current_depth]; kk < endpoints[current_depth + 1]; kk++){
				int w = S[kk];
				float dsw = 0;
				float sw = (float) sigma[w];
				for (int z = R[w]; z < R[w+1]; z++){
					int v = C[z];
					if(d[v] == (d[w] + 1)){
						dsw +=(sw/(float)sigma[v])*(1.0+delta[v]);
					}
				}
				delta[w] = dsw;
			}
			current_depth--;
		}

		for(int kk = 0; kk < n; kk++){
			result_mic[thread_id * n + kk] += delta[kk];
		}

	}
}
