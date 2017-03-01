/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef CPU_BC_H_
#define CPU_BC_H_

#include <vector>
#include <set>
#include <queue>
#include <stack>

#include "GraphUtility.h"

std::vector<float> BC_cpu(Graph g, const std::set<int> &source_vertices);

std::vector<float> BC_cpu_parallel(Graph g, int num_cores, bool is_small_diameter, uint32_t mode);


#endif /* CPU_BC_H_ */
