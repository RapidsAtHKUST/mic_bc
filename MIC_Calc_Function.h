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

__ONMIC__ void MIC_Coarse_Parallel(int n, int m, int *R, int *F, int *C,
		float *result_mic, int num_cores);
__ONMIC__ void MIC_WorkEfficient_Parallel(int n, int m, int *R, int *F, int *C,
		float *result_mic);
__ONMIC__ void MIC_Opt_BC(const int n, const int m, const int *R, const int *F,
		const int *C, float *result_mic, const int num_cores);

#endif /* MIC_CALC_FUNCTION_H_ */
