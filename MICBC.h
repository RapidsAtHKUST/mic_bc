/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef MICBC_H_
#define MICBC_H_

#include <vector>

class MIC_BC {
public:
	MIC_BC();
	std::vector<float> bc();
	virtual ~MIC_BC();
};

#endif /* MICBC_H_ */
