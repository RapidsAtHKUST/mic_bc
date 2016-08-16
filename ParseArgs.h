/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 *
 */

#ifndef PARSEARGS_H_
#define PARSEARGS_H_

#include <iostream>
#include <getopt.h>
#include <stdexcept>
#include <map>
#include <string>
#include <bitset>
#include <vector>
using std::string;

#define NAIVE_CPU 1
#define PAR_CPU 2
#define PAR_CPU_1_DEG 4
#define MIC_OFF 8
#define MIC_OFF_1_DEG 16
#define VERIFY 128

class ParseArgs {
public:
	ParseArgs();
	char *InputFile;
	char *ScoreFile;
	bool verify;
	bool cpu_parallel;
	bool printResult;
	bool reduce_1_deg;
    /*run_flags is a bitmap for selecting the running mode:
     * 0000 0000 0000 0000 -> run nothing
     * 0000 0000 0000 0001 -> run naive cpu version
     * 0000 0000 0000 0010 -> run parallel cpu version
     * 0000 0000 0000 0100 -> run parallel cpu version with 1-deg vertices reduction
     * 0000 0000 0000 1000 -> run mic offload version
     * 0000 0000 0001 0000 -> run mic offload version with 1-deg vertices reduction
     *
     */
    std::bitset<16> run_flags;
	void Parser(int argc, char *argv[]);
	void PrintUsage();

    std::map<unsigned short, string> mode_name;

	int num_devices;
	int num_cores_mic;
	int num_cores_cpu;

	virtual ~ParseArgs();
};

#endif /* PARSEARGS_H_ */
