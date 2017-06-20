/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef TIMECOUNTER_H_
#define TIMECOUNTER_H_

#include <sys/time.h>
#include <time.h>

class TimeCounter {
public:
    timeval start_wall_time_t, end_wall_time_t;
    float ms_wall, ms_by_cpu_click;
    clock_t cpu_click;

    void start_wall_time();

    void stop_wall_time();

    void start_cpu_click();

    void stop_cpu_click();

    TimeCounter();

    virtual ~TimeCounter();
};

#endif /* TIMECOUNTER_H_ */
