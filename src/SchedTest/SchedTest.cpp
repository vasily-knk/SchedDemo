// SchedTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


void planes_task(float timespan, task_t &out_task);
sched_t perm2sched(const task_t &task, const perm_t &perm);
task_t gen_task1();


int main()
{
    bool fail = false;
    
    const size_t n = 4;
    
    for (int i = 0; i < 100 && !fail; ++i)
    {
        task_t task(n);
        planes_task(10, task);

        perm_t perm(n);

        for (int j = 0; j < 100 && !fail; ++j)
        {
            std::random_shuffle(perm.begin(), perm.end());
            sched_t sched1 = perm2sched(task, perm);
            sched_t sched2(n);
            slow_perm2sched(task, perm, sched2);
        
            const cost_t cost1 = get_cost(task, sched1);
            const cost_t cost2 = get_cost(task, sched2);
            if (cost1 - cost2 > 0.0001f)
            {
                cout << "Cost: " << get_cost(task, sched1) << ": " << get_cost(task, sched2) << endl;
                fail = true;
            }
        }

    }
    
    return 0;         
}

