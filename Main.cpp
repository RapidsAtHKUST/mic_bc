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

#include "MIC_BC.h"

int main(int argc, char *argv[]) {

   // try {
        std::ios::sync_with_stdio(false);

        ParseArgs args;

        args.Parser(argc, argv);

        Graph g;
        GraphUtility g_util(&g);
        int left_vertices = 0;

        g_util.parse(args.InputFile);

        std::cout<<"\nSYSTEM INFO:\n";
        std::cout << "\tNumber of MIC: " << args.num_devices << std::endl;
        std::cout << "\tMax Number of threads on MIC[0]: " << args.num_cores_mic
                  << std::endl;
        std::cout << "\tMax Number of threads on HOST: " << args.num_cores_cpu
                  << std::endl;
        std::cout<<"\nINPUT DATA INFO:\n";
        std::cout << "\tNumber of vertices: " << g.n << std::endl;
        std::cout << "\tNumber of edges: " << g.m << "\n" << std::endl;
        Graph g_out;
        if ((args.run_flags.to_ulong() & PAR_CPU_1_DEG) | (args.run_flags.to_ulong() & MIC_OFF_1_DEG)) {
            std::cout << "REDUCING 1 DEGREE VERTICES:\n";
            bool finish = g_util.reduce_1_degree_vertices(&g, &g_out);
            while (!finish) {
                finish = g_util.reduce_1_degree_vertices(&g_out, &g_out);
            }
            for(int i = 0 ; i < g.n; i ++){
                if(g_out.R[i + 1] - g_out.R[i] > 0){
                    left_vertices++;
                }
            }
            std::cout << "\tDeleted " << g.n - left_vertices << " vertices\n";
            std::cout << "\t1 degree vertices percent: " << (g.n - left_vertices) * 100 / (float) g.n << "%\n"
                      << std::endl;
        }


        std::set<int> source_vertices;
        std::vector<float> bc_cpu;
        std::vector<float> result;
        std::vector<float> bc_cpu_parallel;


        if (args.run_flags.to_ulong() & VERIFY) {
            bc_cpu = BC_cpu(g, source_vertices);
        }

        for (int i = 0; i < 16; i++) {
            if (args.run_flags[i]) {
                std::ios::fmtflags f;
                f = std::cout.flags();
                std::cout.setf(std::ios::showbase | std::ios::hex);
                std::cout << "MODE: " << std::hex << (0x1<<i) << " TASK: " << args.mode_name[0x1 << i]<<std::endl;
                std::cout.flags(f);
                TimeCounter _t;

                //std::cout << "start running" << std::endl;
                switch (0x1 << i) {
                    case NAIVE_CPU:
                        _t.start_wall_time();
                        result = BC_cpu(g, source_vertices);
                        _t.stop_wall_time();
                        break;
                    case PAR_CPU:
                        _t.start_wall_time();
                        result = BC_cpu_parallel(g, args.num_cores_cpu);
                        _t.stop_wall_time();
                        break;
                    case PAR_CPU_1_DEG:
                        _t.start_wall_time();
                        result = BC_cpu_parallel(g_out, args.num_cores_cpu);
                        _t.stop_wall_time();
                        break;
                    case MIC_OFF:
                        MIC_BC *mic_bc = new MIC_BC(&g, args.num_cores_mic);
                        _t.start_wall_time();
                        result = mic_bc->opt_bc();
                        _t.stop_wall_time();
                        break;
                    case MIC_OFF_1_DEG:
                        MIC_BC *mic_bc_o = new MIC_BC(&g_out, args.num_cores_mic);
                        _t.start_wall_time();
                        result = mic_bc_o->opt_bc();
                        _t.stop_wall_time();
                        break;
                    case VERIFY:
                        _t.ms_wall = 0;
                        std::cout << "\tThe results will be verified after every task\n";
                        break;
                    default:
                        std::cout << "DD NOTHING AND EXIT\n";
                }

                std::cout << "\ttime: "
                          << _t.ms_wall / 1000.0 << " s" << std::endl;
                std::cout << "\tMTEPS: "
                          << ((long long) g.m * g.n / 1000000)
                             / (_t.ms_wall / 1000.0) << "\n"
                          << std::endl;
                if(args.run_flags.to_ulong() & VERIFY) {
                    g_util.verify(g, result, bc_cpu);
                    std::cout << std::endl;
                }
            }
        }

//    } catch (std::exception &e) {
//        std::cout << e.what() << std::endl;
//    }
    return 0;
}

