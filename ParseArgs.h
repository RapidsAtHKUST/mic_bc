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

#include <offload.h>
#include <omp.h>

class ParseArgs {
public:
	ParseArgs();
	char *InputFile;
	char *ScoreFile;
	bool verify;
	bool printResult;
	void Parser(int argc, char *argv[]);
	void PrintUsage();

	int num_devices;
	int num_cores;

	virtual ~ParseArgs();
};

#endif /* PARSEARGS_H_ */
