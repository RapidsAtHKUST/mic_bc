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
#include <mm_malloc.h>
#include <vector>
#include <queue>
#include <stack>
#include <iostream>
#include <cstring>
#include <omp.h>
#include <cilk/cilk.h>
#pragma offload_attribute(pop)

__ONMIC__ void MIC_Node_Parallel(int n, int m, int *R, int *F, int *C,
		float *result_mic);

#endif /* MIC_CALC_FUNCTION_H_ */
