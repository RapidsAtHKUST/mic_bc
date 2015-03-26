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

#include "Graph.h"
#include "MIC_COMMON.h"

class MIC_BC {
public:
	MIC_BC(Graph g);
	std::vector<float> result;
	void node_parallel();
	virtual ~MIC_BC();

//	__ONMIC_VAR__ int *R;
//	__ONMIC_VAR__ int *F;
//	__ONMIC_VAR__ int *C;
//	__ONMIC_VAR__ int n;
//	__ONMIC_VAR__ int m;
//	__ONMIC_VAR__ float *result_mic;

	int *R;
	int *F;
	int *C;
	int n;
	int m;
	float *result_mic;

};

#endif /* MIC_BC_H_ */
