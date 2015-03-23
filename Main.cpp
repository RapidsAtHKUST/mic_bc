/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 *
 */

#include <iostream>

#include "ParseArgs.h"
#include "Graph.h"

int main(int argc, char *argv[]) {
	try {
		std::ios::sync_with_stdio(false);

		ParseArgs args;

		args.Parser(argc, argv);

		Graph g;



	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

