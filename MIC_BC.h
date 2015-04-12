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
#include <iostream>


#include <malloc.h>

#include "Graph.h"
#include "MIC_COMMON.h"
#include "MIC_Calc_Function.h"
//
//#pragma offload_attribute (push,target(mic))
//namespace MIC {
//	extern int *R;
//	extern int *F;
//	extern int *C;
//	extern int n;
//	extern int m;
//	extern float *result_mic;
//}
//#pragma offload_attribute (pop)

class MIC_BC {
public:
	MIC_BC(Graph g, int num_cores);
	std::vector<float> result;
	void transfer_to_mic();
	std::vector<float> node_parallel();
	std::vector<float> opt_bc();

	int num_cores;

	virtual ~MIC_BC();

};

#endif /* MIC_BC_H_ */
