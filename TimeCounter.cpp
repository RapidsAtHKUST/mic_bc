/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "TimeCounter.h"

TimeCounter::TimeCounter() {
    // TODO Auto-generated constructor stub
    cpu_click = -1;
    ms_wall = -1;
    ms_by_cpu_click = -1;
}

void TimeCounter::start_wall_time() {
    gettimeofday(&start_wall_time_t, nullptr);
}

void TimeCounter::stop_wall_time() {
    gettimeofday(&end_wall_time_t, nullptr);

    ms_wall = ((end_wall_time_t.tv_sec - start_wall_time_t.tv_sec) * 1000 * 1000
               + end_wall_time_t.tv_usec - start_wall_time_t.tv_usec) / 1000.0;
}

void TimeCounter::start_cpu_click() {
    cpu_click = clock();
}

void TimeCounter::stop_cpu_click() {
    cpu_click = clock() - cpu_click;
    ms_by_cpu_click = (cpu_click * 1000.0) / CLOCKS_PER_SEC;
}

TimeCounter::~TimeCounter() {
    // TODO Auto-generated destructor stub
}

