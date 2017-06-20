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
#include <stdint.h>

#include "MIC_COMMON.h"

__ONMIC__ void MIC_Coarse_Parallel(int n, int m, int *__NOLP__ R, int *__NOLP__ F, int *__NOLP__ C,
                                   float *__NOLP__ result_mic, int num_cores);

__ONMIC__ void MIC_Level_Parallel(int n, int m, int *__NOLP__ R, int *__NOLP__ F, int *__NOLP__ C,
                                  float *__NOLP__ result_mic, int num_cores);

__ONMIC__ void MIC_Opt_BC_inner_loop_parallel(const int n, const int m, const int *__NOLP__ R, const int *__NOLP__ F,
                                              const int *__NOLP__ C, const int *weight, const int *which_comp,
                                              float *__NOLP__ result_mic,
                                              const int num_cores, bool is_small_diameter, uint32_t mode,
                                              float traversal_thresold, const int *source_vertices,
                                              int num_source_vertices);

__ONMIC__ void MIC_Opt_BC(const int n, const int m, const int *__NOLP__ R, const int *__NOLP__ F,
                          const int *__NOLP__ C, const int *weight, const int *which_comp, float *__NOLP__ result_mic,
                          const int num_cores, bool is_small_diameter, uint32_t mode, float traversal_thresold,
                          const int *source_vertices, int num_source_vertices);

#endif /* MIC_CALC_FUNCTION_H_ */
