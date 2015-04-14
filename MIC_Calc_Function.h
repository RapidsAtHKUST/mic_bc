/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef MIC_CALC_FUNCTION_H_
#define MIC_CALC_FUNCTION_H_

#include <cstdio>

#include <limits.h>

#include "MIC_COMMON.h"

#pragma offload_attribute(push, target(mic))
#include <malloc.h>
#include <vector>
#include <queue>
#include <stack>
#include <iostream>
#include <cstring>
#include <omp.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <tbb/atomic.h>
#pragma offload_attribute(pop)


__ONMIC__ void MIC_Node_Parallel(int n, int m, int *R, int *F, int *C,
		float *result_mic);
__ONMIC__ void MIC_WorkEfficient_Parallel(int n, int m, int *R, int *F, int *C,
		float *result_mic);
__ONMIC__ void MIC_Opt_BC(int n, int m, int *R, int *F, int *C,
		float *result_mic, int *d_d, unsigned long long *sigma_d,
		float *delta_d, int *Q_d, int *Q2_d, int *S_d, int *endpoints_d,
		int jia_d, int *diameters_d, int num_cores);

#endif /* MIC_CALC_FUNCTION_H_ */
