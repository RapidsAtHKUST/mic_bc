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
#include "TimeCounter.h"
#include "CPU_BC.h"
#include "GraphUtility.h"
#include "MIC_BC.h"

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
		std::vector<float> bc_mic;

		if (args.verify) {
			cpu_t.start_wall_time();
			bc_cpu = BC_cpu(g, source_vertices);
			cpu_t.stop_wall_time();
		}

		MIC_BC Mic_BC(g);

		mic_t.start_wall_time();
		Mic_BC.node_parallel();
		mic_t.stop_wall_time();

		if (args.verify) {
			std::cout.precision(9);
			std::cout << "CPU time: " << cpu_t.ms_wall / 1000.0 << " s"
					<< std::endl;
		}
		if (args.printResult) {

		}

	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

