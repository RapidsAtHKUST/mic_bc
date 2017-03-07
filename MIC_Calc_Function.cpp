/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "MIC_Calc_Function.h"

#ifndef KNL
#pragma offload_attribute(push, target(mic))
#endif

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

#ifndef KNL
#pragma offload_attribute(pop)
#endif

__ONMIC__ void MIC_Coarse_Parallel(int n, int m, int *__NOLP__ R,
                                   int *__NOLP__ F, int *__NOLP__ C, float *__NOLP__ result_mic,
                                   int num_cores) {

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

__ONMIC__ void MIC_Level_Parallel(int n, int m, int *__NOLP__ R,
                                  int *__NOLP__ F, int *__NOLP__ C, float *__NOLP__ result_mic,
                                  int num_cores) {

#define CC_CHUNK 1024
#define vector_size  8

    omp_set_num_threads(num_cores);
    int b = vector_size * 32;
    size_t size_alloc = n;
    size_alloc *= b / 8;
    int n_align = b / 8;
    char *__restrict__ neighbor = (char *) _mm_malloc(size_alloc, n_align);
    char *__restrict__ current = (char *) _mm_malloc(size_alloc, n_align);
    char *__restrict__ visited = (char *) _mm_malloc(size_alloc, n_align);
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
                ((int *) current)[i * vector_size + k] = cu[k];
#pragma unroll
            for (int k = 0; k < vector_size; ++k)
                ((int *) visited)[i * vector_size + k] = cu[k];
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
                                  | ((int *) current)[v * vector_size + k];
                }
#pragma unroll
                for (int k = 0; k < vector_size; ++k)
                    ((int *) neighbor)[i * vector_size + k] = vali[k];
            }
            //Update
            float flevel = 1.0f / (float) level;
#pragma omp parallel for schedule (dynamic,CC_CHUNK)
            for (int i = 0; i < n; ++i) {
                int cu[vector_size];
#pragma unroll
                for (int k = 0; k < vector_size; k++)
                    cu[k] = ((int *) neighbor)[i * vector_size + k]
                            & ~((int *) visited)[i * vector_size + k];
#pragma unroll
                for (int k = 0; k < vector_size; k++)
                    ((int *) visited)[i * vector_size + k] = cu[k]
                                                             | ((int *) visited)[i * vector_size + k];
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
                    ((int *) current)[i * vector_size + k] = cu[k];
            }
        }
    }
    _mm_free(neighbor);
    _mm_free(current);
    _mm_free(visited);

}

__ONMIC__ void MIC_Opt_BC(const int n, const int m, const int *R,
                          const int *F, const int *C, const int *weight, const int *which_comp,
                          float *result_mic, const int num_cores, bool is_small_diameter, uint32_t mode) {

#define THOLD 0.3
#define CHUNK_SIZE 1
//将GPU的block看做是1(也就是编号0), 把GPU的thread对应成mic的thread
    omp_set_num_threads(num_cores);
    int *d_a[num_cores];
    unsigned long long *sigma_a[num_cores];
    float *delta_a[num_cores];
    int *Q2_a[num_cores], *Q_a[num_cores], *S_a[num_cores],
            *endpoints_a[num_cores];
    unsigned long long *succeed_count_a[num_cores];

    bool edge_enable = 0;

    if (mode & MIC_OFF_E_V_TRVL)
        edge_enable = is_small_diameter;

    if ((mode & PAR_CPU_1_DEG) || (mode & MIC_OFF_1_DEG) || (mode & RUN_ON_CPU)) {
        edge_enable = 0;
    }


#ifdef EDGEONLY
    printf("Edge Only.\n");

#endif
#ifdef WEONLY
    printf("Work-efficient Only.\n");

#endif

#ifdef __MIC__
    // There are only 16 memory channels
#pragma omp parallel for num_threads(16)
#endif
    for (int i = 0; i < num_cores; i++) {
        d_a[i] = (int *) _mm_malloc(sizeof(int) * n, 64);
        sigma_a[i] = (unsigned long long *) _mm_malloc(
                sizeof(unsigned long long) * n, 64);
        delta_a[i] = (float *) _mm_malloc(sizeof(float) * n, 64);
        Q2_a[i] = (int *) _mm_malloc(sizeof(int) * n, 64);
        Q_a[i] = (int *) _mm_malloc(sizeof(int) * n, 64);
        endpoints_a[i] = (int *) _mm_malloc(sizeof(int) * (n + 1), 64);
        S_a[i] = (int *) _mm_malloc(sizeof(int) * n, 64);
        succeed_count_a[i] = (unsigned long long *) _mm_malloc(
                sizeof(unsigned long long) * n, 64);
    }


    int edge_count_main = 0;
#pragma omp parallel for schedule(dynamic)
    for (int ind = 0; ind < n; ind++) {
        if (R[ind + 1] - R[ind] != 0) {

            int thread_id = omp_get_thread_num();
            int start_point = ind;

            int *d = d_a[thread_id];
            unsigned long long *sigma = sigma_a[thread_id];
            float *delta = delta_a[thread_id];
            int *Q = Q_a[thread_id];
            int *Q2 = Q2_a[thread_id];
            int *endpoints = endpoints_a[thread_id];
            int *S = S_a[thread_id];
            unsigned long long *successors_count =
                    succeed_count_a[thread_id];

            S[0] = start_point;
            endpoints[0] = 0;
            endpoints[1] = 1;

            int Q_len = 0;
            int Q2_len = 0;
            int S_len = 1;
            int depth = 0;
            int endpoints_len = 2;
            bool calc_done = false;

            int edge_count = 0, we_count = 0;

            d[start_point] = 0;
            sigma[start_point] = 1;

            memset(d, 0xef, sizeof(int) * n);
            memset(sigma, 0, sizeof(unsigned long long) * n);
            memset(delta, 0, sizeof(float) * n);
            memset(successors_count, 0, sizeof(unsigned long long) * n);

            d[start_point] = 0;
            sigma[start_point] = 1;
#ifdef STAGET
            if(mode & INIT_T)
                continue;
#endif

            for (int r = R[start_point]; r < R[start_point + 1]; r++) {
                int w = C[r];
                if (d[w] == 0xefefefef) {
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
                    int v = Q2[kk];
                    Q[kk] = v;
                    successors_count[depth + 1] += R[v + 1] - R[v];
                    S[kk + S_len] = v;
                }
                endpoints[endpoints_len] = endpoints[endpoints_len - 1] + Q2_len;
                endpoints_len++;
                Q_len = Q2_len;
                S_len += Q2_len;
                Q2_len = 0;
                depth++;
            }

            while (!calc_done) {
                bool e = edge_enable && (successors_count[depth] > THOLD * n);
#ifdef EDGEONLY
                e = true;
#endif
#ifdef WEONLY
                e = false;
#endif
                if (e) {
                    for (int k = 0; k < 2 * m; k++) {
                        int v = F[k];
                        if (d[v] == depth) {
                            int w = C[k];
                            if (d[w] == 0xefefefef) {
                                d[w] = d[v] + 1;
                                Q2[Q2_len] = w;
                                Q2_len++;
                            }
                            if (d[w] == (d[v] + 1)) {
                                sigma[w] += sigma[v];
                            }
                        }
                    }
                } else {
                    for (int k = 0; k < Q_len; k++) {
                        int v = Q[k];
                        for (int r = R[v]; r < R[v + 1]; r++) {
                            int w = C[r];
                            if (d[w] == 0xefefefef) {
                                d[w] = d[v] + 1;
                                Q2[Q2_len] = w;
                                Q2_len++;
                            }
                            if (d[w] == (d[v] + 1)) {
                                sigma[w] += sigma[v];
                            }
                        }
                    }
                }
                if (Q2_len == 0) {
                    break;
                } else {
                    for (int kk = 0; kk < Q2_len; kk++) {
                        int v = Q2[kk];
                        Q[kk] = v;
                        successors_count[depth + 1] += R[v + 1] - R[v];
                        S[kk + S_len] = v;
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

            depth = d[S[S_len - 1]];

#ifdef STAGET
            if(mode & TRAVER_T)
                continue;
#endif

            if ((mode & MIC_OFF_1_DEG) || (mode & PAR_CPU_1_DEG))
                for (int i = 0; i < n; i++) {
                    delta[i] = weight[i] - 1;

                }
            while (S_len > 1) {
                int w = S[--S_len];
                //if (w == start_point) continue;
                for (int r = R[w]; r < R[w + 1]; r++) {
                    int v = C[r];
                    if (d[w] == (d[v] + 1)) {
                        delta[v] += (sigma[v] * (1 + delta[w])) / sigma[w];
                    }
                }
            }
            for (int kk = 0; kk < start_point; kk++) {
                if (which_comp[start_point] == which_comp[kk])
                    result_mic[thread_id * n + kk] += delta[kk] * weight[start_point];
            }
            for (int kk = start_point + 1; kk < n; kk++) {
                if (which_comp[start_point] == which_comp[kk])
                    result_mic[thread_id * n + kk] += delta[kk] * weight[start_point];
            }
        }
    }
}
