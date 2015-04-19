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
#include <stdexcept>

#include "ParseArgs.h"
#include "TimeCounter.h"
#include "CPU_BC.h"
#include "Graph.h"
#include "GraphUtility.h"

#include "MIC_BC.h"

int main(int argc, char *argv[]) {
	try {
		std::ios::sync_with_stdio(false);

		ParseArgs args;

		args.Parser(argc, argv);

		Graph g;
		GraphUtility g_util(&g);

		g_util.parse(args.InputFile);

		std::cout << "Number of MIC: " << args.num_devices << std::endl;
		std::cout << "Max Number of threads on MIC[0]: " << args.num_cores_mic
				<< std::endl;
		std::cout << "Max Number of threads on HOST: " << args.num_cores_cpu
				<< std::endl;

		std::cout << "\nNumber of nodes: " << g.n << std::endl;
		std::cout << "Number of edges: " << g.m << std::endl;

		TimeCounter cpu_t;
		TimeCounter mic_t;
		TimeCounter cpu_parallel_t;

		std::set<int> source_vertices;
		std::vector<float> bc_cpu;
		std::vector<float> bc_mic;
		std::vector<float> bc_cpu_parallel;

		if (!source_vertices.empty())
			throw std::runtime_error("source_vectivces NOT empty!");

		if (args.verify) {
			cpu_t.start_wall_time();
			bc_cpu = BC_cpu(g, source_vertices);
			cpu_t.stop_wall_time();
		}

		if (args.cpu_parallel) {
			cpu_parallel_t.start_wall_time();
			bc_cpu_parallel = BC_cpu_parallel(g, args.num_cores_cpu);
			cpu_parallel_t.stop_wall_time();
		}

		MIC_BC Mic_BC(g, args.num_cores_mic);

		mic_t.start_wall_time();
		bc_mic = Mic_BC.opt_bc();
		//bc_mic = BC_cpu_parallel(g, args.num_cores_cpu);
		mic_t.stop_wall_time();

		if (args.printResult) {
			g_util.print_BC_scores(bc_cpu, nullptr);
		}
		if (args.verify) {

			g_util.verify(g, bc_cpu, bc_mic);

			std::cout.precision(9);
			std::cout << "CPU time: " << cpu_t.ms_wall / 1000.0 << " s"
					<< std::endl;
			std::ofstream outcpu("out-cpu.txt");
			std::ofstream outmic("out-mic.txt");

			for (int i = 0; i < g.n; i++) {
				outcpu << bc_cpu[i] << "\n";
				outmic << bc_mic[i] << "\n";
			}
			outcpu.close();
			outmic.close();
		}

		if (args.cpu_parallel) {
			std::cout << "CPU parallel time: "
					<< cpu_parallel_t.ms_wall / 1000.0 << " s" << std::endl;
		}
		std::cout << "MIC time: " << mic_t.ms_wall / 1000.0 << " s"
				<< std::endl;
		std::cout << "MTEPS: " << ((long long)g.m * g.n/1000000) / (mic_t.ms_wall / 1000.0)
				<< std::endl;

	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

