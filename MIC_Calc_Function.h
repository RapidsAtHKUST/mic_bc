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

#include "MIC_COMMON.h"

__ONMIC__ void MIC_Node_Parallel(int n, int m, int *R, int *F, int *C,
		float *result_mic);

#endif /* MIC_CALC_FUNCTION_H_ */
