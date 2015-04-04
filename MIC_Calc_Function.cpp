/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_Calc_Function.h"

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

__ONMIC__ void MIC_Opt_BC(int n, int m, int *R, int *F, int *C,
		float *result_mic, int *d_d, unsigned long long *sigma_d,
		float *delta_d, int *Q_d, int *Q2_d, int *S_d, int *endpoints_d,
		int jia, int *diameters_d, int num_cores) {

//将GPU的block看做是1(也就是编号0), 把GPU的thread对应成mic的thread
	omp_set_num_threads(num_cores);

	int *d_row[num_cores], *Q_row[num_cores], *Q2_row[num_cores],
			*S_row[num_cores], *endpoints_row[num_cores];
	unsigned long long *sigma_row[num_cores];
	float *delta_row[num_cores], *bc[num_cores];

#pragma omp parallel for
	for (int thread_id = 0; thread_id < num_cores; thread_id++) {

		d_row[thread_id] = d_d + thread_id * n;
		Q_row[thread_id] = Q_d + thread_id * n;
		Q2_row[thread_id] = Q2_d + thread_id * n;
		S_row[thread_id] = S_d + thread_id * n;
		endpoints_row[thread_id] = endpoints_d + thread_id * n;
	}




}
