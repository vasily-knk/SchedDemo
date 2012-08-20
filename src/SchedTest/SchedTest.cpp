// SchedTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


void planes_task(float timespan, task_t &out_task);
sched_t perm2sched(const task_t &task, const perm_t &perm);
task_t gen_task1();

task_t apply_perm(const task_t &task, const perm_t &perm)
{
    task_t temp(task.size());
    for (size_t i = 0; i < perm.size(); ++i)
    {
        temp[i] = task[perm[i]];
    }
    return temp;
}

int main()
{
    bool fail = false;
    
    const size_t n = 6;
    int fail_count = 0;
    
    for (int i = 0; i < 100; ++i)
    {
        task_t task(n);
        planes_task(5, task);

        perm_t perm(n);

        for (int j = 0; j < 100; ++j)
        {
            std::random_shuffle(perm.begin(), perm.end());

            sched_t sched1 = perm2sched(task, perm);
            sched_t sched2 = slow_perm2sched(task, perm);
        
            const cost_t cost1 = get_cost(task, sched1);
            const cost_t cost2 = get_cost(task, sched2);
            if (fabs(cost2 - cost1) > 0.0001f)
            {
                cout << "Cost: " << cost1 << ": " << cost2 << endl;
                ++fail_count;
            }
        }

    }

    cout << fail_count << endl;
    
    return 0;         
}

task_t gen_task3();

int amain()
{
    const size_t n = 4;
    task_t task = gen_task3();
    perm_t perm(n);
    sched_t sched1 = perm2sched(task, perm);
    sched_t sched2 = slow_perm2sched(task, perm);;

    const cost_t cost1 = get_cost(task, sched1);
    const cost_t cost2 = get_cost(task, sched2);
    cout << "Cost: " << get_cost(task, sched1) << ": " << get_cost(task, sched2) << endl;
    return 0;
}
