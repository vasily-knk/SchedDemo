// SchedTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


void planes_task(float timespan, task_t &out_task);
sched_t perm2sched(const task_t &task, const perm_t &perm);

int main()
{
    const size_t n = 10;
    const moment_t timespan = 60;
    task_t task(n);
    perm_t perm(n);

    planes_task(timespan, task);
    std::random_shuffle(perm.begin(), perm.end());

    sched_t sched1(n);
    slow_perm2sched(task, perm, sched1);
    sched_t sched2 = perm2sched(task, perm);

    cout << get_cost(task, sched1) << ": " << get_cost(task, sched2) << endl;
    
    return 0;         
}

