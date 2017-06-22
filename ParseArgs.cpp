/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 *
 */

#include "ParseArgs.h"
#include "MIC_COMMON.h"

#ifndef KNL

#include <offload.h>

#endif

#include <omp.h>
#include <iostream>

ParseArgs::ParseArgs() {
    // TODO Auto-generated constructor stub
    InputFile = nullptr;
    sourceVertexFile = nullptr;
    verify = cpu_parallel = reduce_1_deg = false;
    printResult = false;
    traversal_thresold = 0.5;
    approx = false;
#ifndef KNL
    num_devices = _Offload_number_of_devices();
    num_cores_cpu = omp_get_max_threads();
#else
    num_devices = 0;
    num_cores_cpu = 256;
#endif

    num_cores_mic = MAX_MIC_CORE;
    run_flags.reset();

#ifndef KNL
    mode_name[NAIVE_CPU] = "Naive CPU";
    mode_name[PAR_CPU_EV_TRVL] = "Parallel CPU with edge&vertices traversal enabled";
    mode_name[PAR_CPU_WE_ONLY] = "Parallel CPU with work-efficient-only traversal enabled";
    mode_name[PAR_CPU_1_DEG] = "Parallel CPU with 1-deg vertices reduction";
    mode_name[MIC_OFF] = "Xeon Phi Offload";
    mode_name[MIC_OFF_1_DEG] = "Xeon Phi Offload with 1-deg vertices reduction";
    mode_name[MIC_OFF_E_V_TRVL] = "Xeon Phi Offload with edge&vertices traversal enabled";
    mode_name[MIC_OFF_WE_ONLY] = "Xeon Phi Offload with work-efficient-only traversal enabled";
    mode_name[VERIFY] = "Verify the results";
    mode_name[ENABLE_INNER_LOOP] = "Enable inner loop";
#else
    mode_name[NAIVE_CPU] = "Naive CPU";
    mode_name[PAR_CPU_EV_TRVL] = "Parallel KNL with edge&vertices traversal enabled";
    mode_name[PAR_CPU_WE_ONLY] = "Parallel KNL with work-efficient-only traversal enabled";
    mode_name[PAR_CPU_1_DEG] = "Parallel KNL with 1-deg vertices reduction";
    mode_name[VERIFY] = "Verify the results";
    mode_name[ENABLE_INNER_LOOP] = "Enable inner loop";
#endif
}

void ParseArgs::Parser(int argc, char *argv[]) {

    if (argc == 1)
        PrintUsage();

    int oc;
    while ((oc = getopt(argc, argv, "i:f:t:o:cvhr")) != -1) {

        switch (oc) {
            case 'i':
                InputFile = optarg;
                break;
            case 'o':
                approx = true;
                sourceVertexFile = optarg;
                break;
            case 'c':
                cpu_parallel = true;
                break;
            case 'v':
                verify = true;
                break;
            case 'r':
                reduce_1_deg = true;
                break;
            case 'f': {
                std::string in_str = optarg;
                int in_o = 0;
                //char *t;
                in_o = std::strtol(in_str.c_str(), nullptr, 0);
                run_flags = std::bitset<32>(in_o);
                //std::cout << "running mode: " << std::bitset<16>(run_flags) << std::endl;
                for (int i = 0; i <= 32; i++) {
                    if (run_flags[i] && (mode_name.find(0x1 << i) != mode_name.end()))
                        run_flags[i] = 1;
                    else
                        run_flags[i] = 0;
                }
                std::cout << "RUNNING IN " << run_flags.count() << " MODE(S):\n";
                std::ios::fmtflags f;
                f = std::cout.flags();
                std::cout.setf(std::ios::showbase | std::ios::hex);
                for (int i = 0; i <= 32; i++) {
                    if (run_flags[i]) {
                        std::cout << '\t' << "mode: " << std::hex << (0x1 << i) << " task: " << mode_name[(0x01 << i)]
                                  << '\n';
                    }
                }
                std::cout.flags(f);
            }
                break;
            case 't':
                traversal_thresold = atof(optarg);
                break;
            case 'h':
            case '?':
            default:
                PrintUsage();
        }
    }
    printf("threshold value: %f\n", traversal_thresold);
}

ParseArgs::~ParseArgs() {
    // TODO Auto-generated destructor stub
}

void ParseArgs::PrintUsage() {
    throw std::runtime_error("Error arguments! See readme file.");
}
