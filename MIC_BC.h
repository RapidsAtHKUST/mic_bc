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
#include <mutex>

#include <malloc.h>

#include "Graph.h"
#include "MIC_COMMON.h"
#include "MIC_Calc_Function.h"

#ifndef KNL

class MIC_BC {
public:
    MIC_BC(Graph g, int num_cores, bool small_diameter, int* source_vertices, int num_source_vertices,uint32_t mode);
    std::vector<float> result;
    void transfer_to_mic();
    int get_range(int *start, int *end, int want);
    std::vector<float> node_parallel();
    std::vector<float> opt_bc(float traversal_thresold);
    std::vector<float> opt_bc_inner_loop(float traversal_thresold);
    std::vector<float> hybird_opt_bc();
    std::mutex lock;
    int* source_vertices;
    int num_source_vertices;
    int current_node;

    Graph g;


    virtual ~MIC_BC();

};

#endif /* KNL */

#endif /* MIC_BC_H_ */
