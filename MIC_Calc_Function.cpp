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

#define DIAMETER_SAMPLES 512

#pragma offload_attribute(pop)

__ONMIC__ void MIC_Node_Parallel(int n, int m, int* R, int* F, int* C,
		float* result_mic) {

#pragma omp parallel for
	for (int k = 0; k < n; k++) {
		int i = k;
		int id_num = omp_get_thread_num() % 40;

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
				result_mic[id_num * n + w] += delta[w];
			}
		}
	}

}

__ONMIC__ void MIC_WorkEfficient_Parallel(int n, int m, int* R, int* F, int* C,
		float* result_mic) {

}

__ONMIC__ void MIC_Opt_BC(const int n, const int m, const int *R, const int *F,
		const int *C, double *result_mic, const int num_cores) {

//将GPU的block看做是1(也就是编号0), 把GPU的thread对应成mic的thread
	omp_set_num_threads(num_cores);

//	int *d_row[num_cores];
//	unsigned long long *sigma_row[num_cores];
//	float *delta_row[num_cores];
//	int *Q2_row[num_cores], *Q_row[num_cores], *S_row[num_cores],
//			*endpoints_row[num_cores], jia, diameters[DIAMETER_SAMPLES];

//	for (int i = 0; i < num_cores; i++) {
//		S_row[i] = (int *) malloc(sizeof(int) * n);
//		d_row[i] = (int *) malloc(sizeof(int) * n);
//		sigma_row[i] = (unsigned long long *) malloc(
//				sizeof(unsigned long long) * n);
//		delta_row[i] = (float *) malloc(sizeof(float) * n);
//		Q2_row[i] = (int *) malloc(sizeof(int) * n);
//		Q_row[i] = (int *) malloc(sizeof(int) * n);
//		endpoints_row[i] = (int *) malloc(sizeof(int) * (n + 1));
//	}
	int jia = 0;

//	std::memset(diameters, 0, sizeof(diameters));

//	diameters[0] = INT_MAX;

	std::mutex lock;

#pragma omp parallel for
	for (int ind = 0; ind < n; ind++) {

		int start_point = ind;

		std::vector<int> d_row(n, INT_MAX);
		std::vector<unsigned long long> sigma_row(n, 0);
		std::vector<double> delta_row(n, 0);
		std::vector<int> Q(n, 0);
		std::vector<int> Q2(n, 0);
		std::vector<int> endpoints(n + 1, 0);
		std::vector<int> S_row(n, 0);

		S_row[0] = start_point;
		endpoints[0] = 0;
		endpoints[1] = 1;

		int Q_len = 0;
		int Q2_len = 0;
		int S_len = 1;
		int current_depth = 0;
		int endpoints_len = 2;
		int sp_calc_done = false;

		d_row[start_point] = 0;
		sigma_row[start_point] = 1;

		for (int r = R[start_point]; r < R[start_point + 1]; r++) {
			int w = C[r];
			if (d_row[w] == INT_MAX) {
				d_row[w] = 1;
				Q2[Q2_len] = w;
				Q2_len++;
			}
			if (d_row[w] == (d_row[start_point] + 1)) {
				sigma_row[w]++;
			}
		}

		if (Q2_len == 0) {
			sp_calc_done = true;
		} else {
			for (int kk = 0; kk < Q2_len; kk++) {
				Q[kk] = Q2[kk];
				S_row[kk + S_len] = Q2[kk];
			}

			endpoints[endpoints_len] = endpoints[endpoints_len - 1] + Q2_len;
			endpoints_len++;
			Q_len = Q2_len;
			S_len += Q2_len;
			Q2_len = 0;
			current_depth++;

		}

		while (!sp_calc_done) {
			if (jia && (Q_len > 512)) {
				for (int k = 0; k < 2 * m; k++) {
					int v = F[k];
					if (d_row[v] == current_depth) {
						int w = C[k];
						if (d_row[w] == INT_MAX) {
							d_row[w] = d_row[v] + 1;
							Q2[Q2_len] = w;
							Q2_len++;
						}
						if (d_row[w] == (d_row[v] + 1)) {
							sigma_row[w] += sigma_row[v];
						}
					}
				}

			} else { // work-efficient

				for (int k = 0; k < Q_len; k++) {
					int v = Q[k];
					for (int r = R[v]; r < R[v + 1]; r++) {
						int w = C[r];
						if (d_row[w] == INT_MAX) {
							d_row[w] = d_row[v] + 1;
							Q2[Q2_len] = w;
							Q2_len++;
						}
						if (d_row[w] == (d_row[v] + 1)) {
							sigma_row[w] += sigma_row[v];
						}
					}
				}
			}
			if (Q2_len == 0) {
				break;
			} else {
				for (int kk = 0; kk < Q2_len; kk++) {
					Q[kk] = Q2[kk];
					S_row[kk + S_len] = Q2[kk];
				}
				endpoints[endpoints_len] = endpoints[endpoints_len - 1]
						+ Q2_len;
				endpoints_len++;
				Q_len = Q2_len;
				S_len += Q2_len;
				Q2_len = 0;
				current_depth++;
			}
		}

		current_depth = d_row[S_row[S_len - 1]] - 1;
		while (current_depth > 0) {
			//printf("%d\n",current_depth);
			int stack_iter_len = endpoints[current_depth + 1]
					- endpoints[current_depth];
			if (jia && (stack_iter_len > 512)) {
				for (int kk = 0; kk < 2 * m; kk++) {
					int w = F[kk];
					if (d_row[w] == current_depth) {
						int v = C[kk];
						if (d_row[v] == (d_row[w] + 1)) {
							double change = (sigma_row[w] / (double) sigma_row[v])
									* (1.0f + delta_row[v]);
							delta_row[w] += change;
						}
					}
				}
			} else {
				for (int kk = endpoints[current_depth];
						kk < endpoints[current_depth + 1]; kk++) {

					int w = S_row[kk];
					double dsw = 0;
					double sw = (double) sigma_row[w];
					for (int z = R[w]; z < R[w + 1]; z++) {
						int v = C[z];
						if (d_row[v] == (d_row[w] + 1)) {
							dsw += (sw / (double) sigma_row[v])
									* (1.0f + delta_row[v]);
						}
					}
					delta_row[w] = dsw;
				}
			}
			current_depth--;

		}
		int thread_id = omp_get_thread_num();
		for (int kk = 0; kk < n; kk++) {
			result_mic[thread_id * n + kk] += delta_row[kk];
		}

//		if (ind == 2 * DIAMETER_SAMPLES) {
//			int diameter_keys[DIAMETER_SAMPLES];
//			for (int kk = 0; kk < DIAMETER_SAMPLES; kk++) {
//				diameter_keys[kk] = diameters[kk];
//			}
//
//			std::sort(diameter_keys, diameter_keys + DIAMETER_SAMPLES,
//					std::greater<int>());
//
//			int log2n = 0;
//			int tempn = n;
//			while (tempn >>= 1) {
//				++log2n;
//			}
//			if (diameter_keys[DIAMETER_SAMPLES / 2] < 4 * log2n) {
//				jia = 1;
//			}
//		}

	}
}
