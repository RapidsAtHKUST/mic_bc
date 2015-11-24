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

__ONMIC__ void MIC_Coarse_Parallel(int n, int m, int* __NOLP__ R, int* __NOLP__ F, int* __NOLP__ C,
		float* __NOLP__ result_mic, int num_cores) {

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

__ONMIC__ void MIC_Level_Parallel(int n, int m, int* __NOLP__ R, int* __NOLP__ F, int* __NOLP__ C,
		float* __NOLP__ result_mic, int num_cores) {

#define CC_CHUNK 1024
#define vector_size  8

	omp_set_num_threads(num_cores);
	int b = vector_size * 32;
	size_t size_alloc = n;
	size_alloc *= b / 8;
	int n_align = b / 8;
	char* __restrict__ neighbor = (char*) _mm_malloc(size_alloc, n_align);
	char* __restrict__ current = (char*) _mm_malloc(size_alloc, n_align);
	char* __restrict__ visited = (char*) _mm_malloc(size_alloc, n_align);
	for (int s = 0; s < n; s += b) {
		//Init
#pragma omp parallel for schedule (dynamic, CC_CHUNK)
		for (int i = 0; i < n; ++i) {
			int cu[vector_size];
#pragma unroll
			for (int j = 0; j < vector_size; j++)
				cu[j] = 0;
			if (i >= s && i < s + b)
				cu[(i - s) >> 5] = 1 << ((i - s) & 0x1F);
#pragma unroll
			for (int k = 0; k < vector_size; ++k)
				((int*) current)[i * vector_size + k] = cu[k];
#pragma unroll
			for (int k = 0; k < vector_size; ++k)
				((int*) visited)[i * vector_size + k] = cu[k];
		}
		int cont = 1;
		int level = 0;
		while (cont != 0) {
			cont = 0;
			++level;
			//SpMM
#pragma omp parallel for schedule (dynamic,CC_CHUNK)
			for (int i = 0; i < n; ++i) {
				int vali[vector_size];
#pragma unroll
				for (int k = 0; k < vector_size; ++k)
					vali[k] = 0;
				for (int j = R[i]; j < R[i + 1]; ++j) {
					int v = C[j];
#pragma unroll
					for (int k = 0; k < vector_size; k++)
						vali[k] = vali[k]
								| ((int*) current)[v * vector_size + k];
				}
#pragma unroll
				for (int k = 0; k < vector_size; ++k)
					((int*) neighbor)[i * vector_size + k] = vali[k];
			}
			//Update
			float flevel = 1.0f / (float) level;
#pragma omp parallel for schedule (dynamic,CC_CHUNK)
			for (int i = 0; i < n; ++i) {
				int cu[vector_size];
#pragma unroll
				for (int k = 0; k < vector_size; k++)
					cu[k] = ((int*) neighbor)[i * vector_size + k]
							& ~((int*) visited)[i * vector_size + k];
#pragma unroll
				for (int k = 0; k < vector_size; k++)
					((int*) visited)[i * vector_size + k] = cu[k]
							| ((int*) visited)[i * vector_size + k];
				int bcount = 0;
#pragma unroll
				for (int k = 0; k < vector_size; k++)
					bcount += (cu[k]);
				if (bcount > 0) {
					result_mic[i] += bcount * flevel;
					cont = 1;
				}
#pragma unroll
				for (int k = 0; k < vector_size; ++k)
					((int*) current)[i * vector_size + k] = cu[k];
			}
		}
	}
	_mm_free(neighbor);
	_mm_free(current);
	_mm_free(visited);

}

__ONMIC__ void MIC_Opt_BC(const int n, const int m, const int* __NOLP__ R, const int* __NOLP__ F,
		const int* __NOLP__ C, float* __NOLP__ result_mic, const int num_cores) {

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

	int thread_id;
#pragma omp parallel for private(thread_id)
	for (int ind = 0; ind < n; ind++) {

		thread_id = omp_get_thread_num();
		int start_point = ind;

		int* __NOLP__ d = d_a[thread_id];
		unsigned long long* __NOLP__ sigma = sigma_a[thread_id];
		float* __NOLP__ delta = delta_a[thread_id];
		int* __NOLP__ Q = Q_a[thread_id];
		int* __NOLP__ Q2 = Q2_a[thread_id];
		int* __NOLP__ endpoints = endpoints_a[thread_id];
		int* __NOLP__ S = S_a[thread_id];

		S[0] = start_point;
		endpoints[0] = 0;
		endpoints[1] = 1;

		int Q_len = 0;
		int Q2_len = 0;
		int S_len = 1;
		int depth = 0;
		int endpoints_len = 2;
		bool calc_done = false;

		d[start_point] = 0;
		sigma[start_point] = 1;

		for (int i = 0; i < n; i++) {
			d[i] = INT_MAX;
			sigma[i] = 0;
			delta[i] = 0;
		}

		d[start_point] = 0;
		sigma[start_point] = 1;

		for (int r = R[start_point]; r < R[start_point + 1]; r++) {
			int w = C[r];
			if (d[w] == INT_MAX) {
				d[w] = 1;
				Q2[Q2_len] = w;
				Q2_len++;
			}
			if (d[w] == (d[start_point] + 1)) {
				sigma[w]++;
			}
		}

		if (Q2_len == 0) {
			calc_done = true;
		} else {
			for (int kk = 0; kk < Q2_len; kk++) {
				Q[kk] = Q2[kk];
				S[kk + S_len] = Q2[kk];
			}
			endpoints[endpoints_len] = endpoints[endpoints_len - 1] + Q2_len;
			endpoints_len++;
			Q_len = Q2_len;
			S_len += Q2_len;
			Q2_len = 0;
			depth++;
		}

		while (!calc_done) {
			for (int k = 0; k < Q_len; k++) {
				int v = Q[k];
				for (int r = R[v]; r < R[v + 1]; r++) {
					int w = C[r];
					if (d[w] == INT_MAX) {
						d[w] = d[v] + 1;
						Q2[Q2_len] = w;
						Q2_len++;
					}
					if (d[w] == (d[v] + 1)) {
						sigma[w] += sigma[v];
					}
				}
			}
			if (Q2_len == 0) {
				break;
			} else {
				for (int kk = 0; kk < Q2_len; kk++) {
					Q[kk] = Q2[kk];
					S[kk + S_len] = Q2[kk];
				}
				endpoints[endpoints_len] = endpoints[endpoints_len - 1]
						+ Q2_len;
				endpoints_len++;
				Q_len = Q2_len;
				S_len += Q2_len;
				Q2_len = 0;
				depth++;
			}
		}

		depth = d[S[S_len - 1]] - 1;

		while (depth > 0) {
			for (int kk = endpoints[depth]; kk < endpoints[depth + 1]; kk++) {
				int w = S[kk];
				float dsw = 0;
				float sw = (float) sigma[w];
				for (int z = R[w]; z < R[w + 1]; z++) {
					int v = C[z];
					if (d[v] == (d[w] + 1)) {
						dsw += (sw / (float) sigma[v]) * (1.0f + delta[v]);
					}
				}
				delta[w] = dsw;
			}
			depth--;
		}

		for (int kk = 0; kk < n; kk++) {
			result_mic[thread_id * n + kk] += delta[kk];
		}
	}
}
