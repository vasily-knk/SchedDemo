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

int amain()
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
            if (cost2 >  cost1 + 0.001)
            {
                cout << "Cost: " << cost1 << ": " << cost2 << endl;
                ++fail_count;
            }
        }

    }

    cout << fail_count << endl;
    
    return 0;         
}


int main()
{
    task_t task(2);

    size_t index = 0;

    task[index].due = 0;
    task[index].min_bound = 0;
    task[index].max_bound = 0.5;
    task[index].eweight = task[index].tweight = 2;
    std::fill(task[index].spans.begin(), task[index].spans.end(), moment_t(2));
    ++index;

    task[index].due = 2;
    task[index].min_bound = 2;
    task[index].max_bound = 2.5;
    task[index].eweight = task[index].tweight = 2;
    std::fill(task[index].spans.begin(), task[index].spans.end(), moment_t(2));
    ++index;

    perm_t perm(task.size());
    std::next_permutation(perm.begin(), perm.end());

    cout << (check_feasible(task, perm) ? "OK" : "FAIL") << endl;

    return 0;
}