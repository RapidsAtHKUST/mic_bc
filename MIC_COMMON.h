/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef MIC_COMMON_H_
#define MIC_COMMON_H_


#define MAX_MIC_CORE 244

#define ALLOC alloc_if(1) free_if(0)
#define FREE alloc_if(0) free_if(1)
#define REUSE alloc_if(0) free_if(0)
#define __ONMIC__ __attribute__((target(mic)))




#endif /* MIC_COMMON_H_ */
