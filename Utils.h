/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "MIC_COMMON.h"

//https://en.wikipedia.org/wiki/Kahan_summation_algorithm
//initial: sum = 0.0, c = 0.0
__ONMIC__ void KahanSum(float *sum, float *c, float input);


#endif /* UTILS_H_ */
