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



	cilk_for (int k = 0; k < n; k++) {
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
					delta[v] += (sigma[v] / (float) sigma[w]) * (1 + delta[w]);
				}
			}
			if (w != i) {

				result_mic[i * n + w] += delta[w];
			}
		}
	}

}
