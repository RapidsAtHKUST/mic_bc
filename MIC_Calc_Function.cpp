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
#ifdef __ONMIC__
	printf("MIC\n");
#else
	printf("CPU\n");
#endif

}
