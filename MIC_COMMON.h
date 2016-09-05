/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef MIC_COMMON_H_
#define MIC_COMMON_H_


#define NAIVE_CPU 1
#define PAR_CPU 2
#define PAR_CPU_1_DEG 4
#define MIC_OFF 8
#define MIC_OFF_1_DEG 16
#define MIC_OFF_E_V_TRVL 32
#define VERIFY 128

#define INIT_T 0x0100
#define TRAVER_T 0x0200
#define RUN_ON_CPU 0x8000

#define MAX_MIC_CORE 240

#define ALLOC alloc_if(1) free_if(0)
#define FREE alloc_if(0) free_if(1)
#define REUSE alloc_if(0) free_if(0)
#ifndef KNL
#define __ONMIC__ __attribute__((target(mic)))
#else
#define __ONMIC__
#endif
//NOLP -> no overlap
#define __NOLP__ __restrict__

#endif /* MIC_COMMON_H_ */
