/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef MIC_BC_H_
#define MIC_BC_H_

#include <vector>
#include <cstring>

#include <mm_malloc.h>

#include "GraphUtility.h"
#include "MIC_COMMON.h"
#include "MIC_Calc_Function.h"

class MIC_BC {
public:
	MIC_BC(Graph g);
	std::vector<float> result;
	void node_parallel();
	virtual ~MIC_BC();

	int *R;
	int *F;
	int *C;
	int n;
	int m;
	float *result_mic;

};

#endif /* MIC_BC_H_ */
