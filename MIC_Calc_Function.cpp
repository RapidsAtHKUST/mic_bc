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
		int jia, int *diameters, int num_cores) {

//将GPU的block看做是1(也就是编号0), 把GPU的thread对应成mic的thread
	omp_set_num_threads(num_cores);

	int *d_row, *Q_row, *Q2_row, *S_row, *endpoints_row;
	unsigned long long *sigma_row;
	float *delta_row, *bc;

	d_row = d_d;
	Q_row = Q_d;
	Q2_row = Q2_d;
	S_row = S_d;
	endpoints_row = endpoints_d;
	sigma_row = sigma_d;
	delta_row = delta_d;

	int ind = 0;
	int start_point = 0;
	while (ind < n) {
#pragma omp parallel for
		for (int thread_id = 0; thread_id < num_cores; thread_id++) {
			if (thread_id == start_point) {
				d_row[thread_id] = 0;
				sigma_row[thread_id] = 1;
			} else {
				d_row[thread_id] = INT_MAX;
				sigma_row[thread_id] = 0;
			}
			delta_row[thread_id] = 0;
		}

		int Q_len;
		int Q2_len;
		int S_len;
		int current_depth;
		int endpoints_len;
		bool sp_calc_done;

		Q_row[0] = start_point;
		Q_len = 1;
		Q2_len = 0;
		S_row[0] = start_point;
		S_len = 1;
		endpoints_row[0] = 0;
		endpoints_row[1] = 0;
		endpoints_len = 2;
		current_depth = 0;
		sp_calc_done = false;

#pragma omp parallel for
		for (int r = R[start_point]; r < R[start_point + 1]; r++) {
			int w = C[r];
			if (d_row[w] == INT_MAX) {
				d_row[w] = 1;
				int t = Q2_len;
#pragma omp atomic
				Q2_len += 1;
				Q2_row[t] = w;
			}
			if (d_row[w] == (d_row[start_point] + 1)) {
#pragma omp atomic
				sigma_row[w] += 1;
			}
		}

		if (Q2_len == 0) {
			sp_calc_done = true;
		} else {
#pragma omp parallel for
			for (int kk = 0; kk < Q2_len; kk++) {
				Q_row[kk] = Q2_row[kk];
				S_row[kk + S_len] = Q2_row[kk];
			}

			endpoints_row[endpoints_len] = endpoints_row[endpoints_len - 1]
					+ Q2_len;
			endpoints_len++;
			Q_len = Q2_len;
			S_len += Q2_len;
			Q2_len = 0;
			current_depth++;
		}

		while (!sp_calc_done) {
			if (jia && (Q_len > 512)) {
#pragma omp parallel for
				for (int k = 0; k < 2 * m; k++) {
					int v = F[k];
					if (d_row[v] == current_depth) {
						int w = C[k];
						if (d_row[w] == INT_MAX) {
#pragma omp atomic
							d_row[w] = d_row[w] + 1;
							int t = Q2_len;
#pragma omp atomic
							Q2_len += 1;
							Q2_row[t] = w;
						}
						if (d_row[w] == (d_row[v] + 1)) {
#pragma omp atomic
							sigma_row[w] += sigma_row[v];
						}
					}
				}
			} else { // work-efficient

#pragma omp parallel for
				for (int k = 0; k < Q_len; k++) {
					int v = Q_row[k];
					for (int r = R[v]; r < R[v + 1]; r++) {
						int w = C[r];
						if (d_row[w] == INT_MAX) {
#pragma omp atomic
							d_row[w] = d_row[v] + 1;
							int t = Q2_len;
#pragma omp atomic
							Q2_len += 1;
							Q2_row[t] = w;
						}
						if (d_row[w] == (d_row[v] + 1)) {
#pragma omp atomic
							sigma_row[w] += sigma_row[v];
						}
					}
				}
			}

			if (Q2_len == 0) {
				break;
			} else {
#pragma omp parallel for
				for (int kk = 0; kk < Q2_len; kk++) {
					Q_row[kk] = Q2_row[kk];
					S_row[kk + S_len] = Q2_row[kk];
				}
				endpoints_row[endpoints_len] = endpoints_row[endpoints_len - 1]
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
			int stack_iter_len = endpoints_row[current_depth + 1]
					- endpoints_row[current_depth];
			if (jia && (stack_iter_len > 512)) {
#pragma omp parallel for
				for (int kk = 0; kk < 2 * m; kk++) {
					int w = F[kk];
					if (d_row[w] == current_depth) {
						int v = C[kk];
						if (d_row[v] == (d_row[w] + 1)) {
							float change = (sigma_row[w] / (float) sigma_row[v])
									* (1.0f + delta_row[v]);
#pragma omp atomic
							delta_row[w] += change;
						}
					}
				}
			} else {
#pragma omp parallel for
				for (int kk = endpoints_row[current_depth];
						kk < endpoints_row[current_depth + 1]; kk++) {
					int w = S_row[kk];
					float dsw = 0;
					float sw = (float) sigma_row[w];
					for(int z = R[w]; z < R[w +1]; z++){
						int v = C[z];
						if(d_row[v] == (d_row[w] + 1)){
							dsw += (sw / (float)sigma_row[v])
									*(1.0f+delta_row[v]);
						}
					}
					delta_row[w] = dsw;
				}
			}
			current_depth--;

		}
		for(int kk = 0; kk<n;kk++){
			result_mic[kk] += delta_row[kk];
		}

		ind++;
		start_point = ind;
	}

}
