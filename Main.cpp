/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 *
 */

#include <iostream>
#include <vector>
#include <set>

#include "ParseArgs.h"
#include "Graph.h"
#include "TimeCounter.h"
#include "CPU_BC.h"

int main(int argc, char *argv[]) {
	try {
		std::ios::sync_with_stdio(false);

		ParseArgs args;

		args.Parser(argc, argv);

		Graph g;

		g.parse(args.InputFile);

		std::cout << "Number of nodes: " << g.n << std::endl;
		std::cout << "Number of edges: " << g.m << std::endl;

		TimeCounter cpu_t;
		TimeCounter mic_t;

		std::set<int> source_vertices;
		std::vector<float> bc_cpu;

		if (args.verify) {
			cpu_t.start_wall_time();
			bc_cpu = BC_cpu(g, source_vertices);
			cpu_t.stop_wall_time();
		}

		if (args.verify) {
			std::cout.precision(6);
			std::cout << "CPU time: " << cpu_t.ms_wall/1000.0 << " s" << std::endl;
			//for (auto i = bc_cpu.begin(); i != bc_cpu.end(); i++) {
			//	std::cout << *i << std::endl;
			//}
		}

	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

