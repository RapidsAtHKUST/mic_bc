/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 *
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <cstdlib>
#include <ctime>

#include "ParseArgs.h"
#include "TimeCounter.h"
#include "CPU_BC.h"

#include "MIC_BC.h"

bool sampling(Graph g) {
    int sampling_number = 512;
    std::vector<int> diameters(sampling_number, INT_MAX);
    for (int it = 0; it < sampling_number; it++) {
        int depth = 0;
        std::vector<bool> vis(g.n, false);
        std::vector<int> que1, que2;
        que1.push_back(it);
        vis[it] = true;
        while (1) {
            for (int i = 0; i < que1.size(); i++) {
                int u = que1[i];
                for (int j = g.R[u]; j < g.R[u + 1]; j++) {
                    int v = g.C[j];
                    if (!vis[v]) {
                        que2.push_back(v);
                        vis[v] = true;
                    }
                }
            }
            if (que2.size() == 0) {
                break;
            } else {
                que1.clear();
                for (int i = 0; i < que2.size(); i++) {
                    que1.push_back(que2[i]);
                }
                que2.clear();
                depth++;
            }
        }
        diameters[it] = depth;
    }

    std::sort(diameters.begin(), diameters.end(), std::less<int>());
    int log2n = 0;
    int tempn = g.n;
    while (tempn >>= 1)
        ++log2n;
    if (diameters[sampling_number / 2] < 4 * log2n) {
        return true;
    }

    return false;
}

int main(int argc, char *argv[]) {

    //std::ios::sync_with_stdio(false);

    ParseArgs args;

    args.Parser(argc, argv);

    Graph g;
    GraphUtility g_util(&g);
    int left_vertices = 0;

    g_util.parse(args.InputFile);

    std::cout << "\nSYSTEM INFO:\n";
#ifndef KNL
    std::cout << "\tNumber of MIC: " << args.num_devices << std::endl;
    std::cout << "\tMax Number of threads on MIC[0]: " << args.num_cores_mic
              << std::endl;
    std::cout << "\tMax Number of threads on HOST: " << args.num_cores_cpu
              << std::endl;
#else
    std::cout << "\tNumber of threads on KNL: " << args.num_cores_cpu << std::endl;
#endif
    std::cout << "\nINPUT DATA INFO:\n";
    std::cout << "\tNumber of vertices: " << g.n << std::endl;
    std::cout << "\tNumber of edges: " << g.m << "\n" << std::endl;
    Graph g_out;
    TimeCounter _t;
    if ((args.run_flags.to_ulong() & PAR_CPU_1_DEG) | (args.run_flags.to_ulong() & MIC_OFF_1_DEG)) {
        std::cout << "REDUCING 1 DEGREE VERTICES:\n";
        _t.start_wall_time();
        bool finish = g_util.reduce_1_degree_vertices(&g, &g_out);
        while (!finish) {
            finish = g_util.reduce_1_degree_vertices(&g_out, &g_out);
        }
        for (int i = 0; i < g.n; i++) {
            if (g_out.R[i + 1] - g_out.R[i] > 0) {
                left_vertices++;
            }
        }
        _t.stop_wall_time();
        std::cout << "\tDeleted " << g.n - left_vertices << " vertices\n";
        std::cout << "\t1 degree vertices percent: " << (g.n - left_vertices) * 100 / (float) g.n << "%\n";
        std::cout << "\tREDUCING 1 DEG VERTICES TIME: " << _t.ms_wall / 1000.0 << " s\n" << std::endl;

#ifdef DEBUG
        std::cout << "\tPre-processing BC values:\n\t";
        for (int i = 0; i < g.n; i++) {
            std::cout << g_out.bc[i] << " ";
        }
        std::cout << std::endl;
#endif
    }


    //test running platform and sampling the input graph
    bool small_diameter = sampling(g);
    if (small_diameter) {
        std::cout << "This is a small diameter graph." << std::endl;
    } else {
        std::cout << "This is a large diameter graph." << std::endl;
    }


    std::set<int> source_vertices;
    std::vector<float> bc_cpu;
    std::vector<float> result;
    std::vector<float> bc_cpu_parallel;
    int *source_vertices_array;

    if (args.approx) {
        std::string fileName = std::string(args.sourceVertexFile);

        if (fileName.find(".txt") == std::string::npos) {
            int k = std::strtol(args.sourceVertexFile, nullptr, 0);
            if (k > g.n) {
                k = g.n;
            }
            std::srand(time(NULL));
            while (source_vertices.size() < k) {
                int temp_source = std::rand() % g.n;
                source_vertices.insert(temp_source);
            }
        } else {
            std::ifstream sourceFile(args.sourceVertexFile, std::ifstream::in);
            sourceFile.seekg(0);
            while (sourceFile.peek() != EOF) {
                int vertices_id;
                sourceFile >> vertices_id;
                source_vertices.insert(vertices_id);
            }
            sourceFile.close();
        }
        source_vertices_array = (int *) malloc(sizeof(int) * source_vertices.size());
        int i = 0;
        for (auto e = source_vertices.begin(); e != source_vertices.end(); e++) {
            source_vertices_array[i++] = *e;
        }
        std::cout << "Source Vertices is provided with size " << source_vertices.size() << ", ratio: "
                  << ((float) source_vertices.size() / g.n) << std::endl;
    }


    if (args.run_flags.to_ulong() & VERIFY) {
        bc_cpu = BC_cpu(g, source_vertices);
        //g_util.print_BC_scores(bc_cpu, nullptr);
    }
#ifdef DEBUG
    std::cout << args.run_flags.to_ulong() << std::endl;
    for (int i = 0; i <= 32; i++) {
        if (args.run_flags[i])
            printf("%d\n", i);
    }
#endif

    for (int j = 0; j <= 8; j++) {
        int bit = j;
        if (args.run_flags[bit]) {
            sleep(3);
            std::ios::fmtflags f;
            f = std::cout.flags();
            std::cout.setf(std::ios::showbase | std::ios::hex);
            std::cout << "MODE: " << std::hex << (0x1 << bit) << " TASK: " << args.mode_name[0x1 << bit] << std::endl;
            std::cout.flags(f);

            //std::cout << "start running" << std::endl;
            switch (0x1 << bit) {
                case NAIVE_CPU:
                    _t.start_wall_time();
                    result = BC_cpu(g, source_vertices);
                    _t.stop_wall_time();
                    break;
                case PAR_CPU_EV_TRVL:
                    _t.start_wall_time();
                    result = BC_cpu_parallel(g, args.num_cores_cpu, small_diameter, source_vertices_array,
                                             source_vertices.size(), PAR_CPU_EV_TRVL, args.traversal_thresold);
                    _t.stop_wall_time();
                    break;
                case PAR_CPU_WE_ONLY:
                    _t.start_wall_time();
                    result = BC_cpu_parallel(g, args.num_cores_cpu, small_diameter, source_vertices_array,
                                             source_vertices.size(), PAR_CPU_WE_ONLY, args.traversal_thresold);
                    _t.stop_wall_time();
                    break;
                case PAR_CPU_1_DEG:
                    _t.start_wall_time();
                    result = BC_cpu_parallel(g_out, args.num_cores_cpu, small_diameter, source_vertices_array,
                                             source_vertices.size(), PAR_CPU_1_DEG, args.traversal_thresold);
                    _t.stop_wall_time();
                    //g_util.print_BC_scores(result, nullptr);
                    break;
                case MIC_OFF:
#ifndef KNL
                    MIC_BC *mic_bc = new MIC_BC(g, args.num_cores_mic, small_diameter, source_vertices_array,
                                                source_vertices.size(), MIC_OFF);
                    _t.start_wall_time();
                    result = mic_bc->opt_bc(args.traversal_thresold);
                    _t.stop_wall_time();
#endif
                    break;
                case MIC_OFF_1_DEG:
#ifndef KNL
                    MIC_BC *mic_bc_o = new MIC_BC(g_out, args.num_cores_mic, small_diameter, source_vertices_array,
                                                  source_vertices.size(), MIC_OFF_1_DEG);
                    _t.start_wall_time();
                    result = mic_bc_o->opt_bc(args.traversal_thresold);
                    _t.stop_wall_time();
#endif
                    break;
                case MIC_OFF_E_V_TRVL:
#ifndef KNL
                    MIC_BC *mic_bc_ev = new MIC_BC(g, args.num_cores_mic, small_diameter, source_vertices_array,
                                                   source_vertices.size(), MIC_OFF_E_V_TRVL);
                    _t.start_wall_time();
                    result = mic_bc_ev->opt_bc(args.traversal_thresold);
                    _t.stop_wall_time();
#endif
                    break;
                case MIC_OFF_WE_ONLY:
#ifndef KNL
                    MIC_BC *mic_bc_we_only = new MIC_BC(g, args.num_cores_mic, small_diameter, source_vertices_array,
                                                        source_vertices.size(), MIC_OFF_WE_ONLY);
                    _t.start_wall_time();
                    result = mic_bc_we_only->opt_bc(args.traversal_thresold);
                    _t.stop_wall_time();
#endif
                    break;
                case VERIFY:
                    _t.ms_wall = 0;
                    std::cout << "\tThe results will be verified after each task\n";
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
            if (args.run_flags.to_ulong() & VERIFY) {
                g_util.verify(g, result, bc_cpu);
                std::cout << std::endl;
            }
        }
    }

    //inner loop parallel
    if (!(args.run_flags.to_ulong() & ENABLE_INNER_LOOP))
        return 0;
    std::cout << "Enable inner loop" << std::endl;
    for (int j = 0; j < 8; j++) {
        int bit = j;
        if (args.run_flags[bit]) {
            if ((0x1 << bit) != PAR_CPU_WE_ONLY || (0x1 << bit) != MIC_OFF_WE_ONLY)
                continue;
            sleep(3);
            std::ios::fmtflags f;
            f = std::cout.flags();
            std::cout.setf(std::ios::showbase | std::ios::hex);
            std::cout << "MODE: " << std::hex << (0x1 << bit) << " TASK: " << args.mode_name[0x1 << bit] << std::endl;
            std::cout.flags(f);

            //std::cout << "start running" << std::endl;
            switch (0x1 << bit) {

                case PAR_CPU_WE_ONLY:
                    _t.start_wall_time();
                    result = BC_cpu_parallel_inner_loop(g, args.num_cores_cpu, small_diameter, source_vertices_array,
                                                        source_vertices.size(), PAR_CPU_WE_ONLY,
                                                        args.traversal_thresold);
                    _t.stop_wall_time();
                    break;
                case MIC_OFF_WE_ONLY:
#ifndef KNL
                    MIC_BC *mic_bc_we_only = new MIC_BC(g, args.num_cores_mic, small_diameter, source_vertices_array,
                                                        source_vertices.size(), MIC_OFF_WE_ONLY);
                    _t.start_wall_time();
                    result = mic_bc_we_only->opt_bc_inner_loop(args.traversal_thresold);
                    _t.stop_wall_time();
#endif
                    break;
                case VERIFY:
                    _t.ms_wall = 0;
                    std::cout << "\tThe results will be verified after each task\n";
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
            if (args.run_flags.to_ulong() & VERIFY) {
                g_util.verify(g, result, bc_cpu);
                std::cout << std::endl;
            }
        }
    }


    return 0;
}