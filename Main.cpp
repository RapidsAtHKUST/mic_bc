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

		g.parse(args.InputFile);

		std::cout << "Number of nodes: " << g.n << std::endl;
		std::cout << "Number of edges: " << g.m << std::endl;



	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

