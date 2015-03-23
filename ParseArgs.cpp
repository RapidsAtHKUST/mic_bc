/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 *
 */

#include "ParseArgs.h"

ParseArgs::ParseArgs() {
	// TODO Auto-generated constructor stub
	InputFile = nullptr;
	ScoreFile = nullptr;
	verify = false;
}

void ParseArgs::Parser(int argc, char* argv[]) {

	if(argc == 1)
		PrintUsage();

	int oc;
	while ((oc = getopt(argc, argv, "i:o::vh")) != -1) {

		switch (oc) {
		case 'i':
			InputFile = optarg;
			break;
		case 'o':
			ScoreFile = optarg;
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
	throw  std::runtime_error("Error arguments!");
}
