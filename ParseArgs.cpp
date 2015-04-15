/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 *
 */

#include "ParseArgs.h"

#include <offload.h>
#include <omp.h>

ParseArgs::ParseArgs() {
	// TODO Auto-generated constructor stub
	InputFile = nullptr;
	ScoreFile = nullptr;
	verify = cpu_parallel = false;
	printResult = false;
	num_devices = _Offload_number_of_devices();
	num_cores_mic = omp_get_max_threads_target(TARGET_MIC, 0);
	num_cores_cpu = omp_get_max_threads();
}

void ParseArgs::Parser(int argc, char* argv[]) {

	if (argc == 1)
		PrintUsage();

	int oc;
	while ((oc = getopt(argc, argv, "i:o::cvh")) != -1) {

		switch (oc) {
		case 'i':
			InputFile = optarg;
			break;
		case 'o':
			printResult = true;
			ScoreFile = optarg;
			break;
		case 'c':
			cpu_parallel = true;
			break;
		case 'v':
			verify = true;
			break;
		case 'h':
		case '?':
		default:
			PrintUsage();
		}
	}
}

ParseArgs::~ParseArgs() {
	// TODO Auto-generated destructor stub
}

void ParseArgs::PrintUsage() {
	throw std::runtime_error("Error arguments!");
}
