/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "Utils.h"


__ONMIC__ void KahanSum(float* sum, float* c, float input) {
	float y, t;
	y = input - *c;
	t = *sum + y;
	*c = (t - *sum) - y;
	*sum = t;

}

