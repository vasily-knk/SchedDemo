// SchedTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


void planes_task(float timespan, task_t &out_task);
sched_t slow_perm2sched(const task_t &task, const perm_t &perm);
sched_t perm2sched(const task_t &task, const perm_t &perm);
task_t gen_task1();
task_t planes_task_with_bounds(const size_t num_planes, const moment_t timespan, const moment_t bound_timespan);

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
    
    const size_t n = 20;
    int fail_count = 0;
    
    for (int i = 0; i < 100; ++i)
    {
        const task_t task = planes_task_with_bounds(n, 10, 10);

        perm_t perm(n);

        for (int j = 0; j < 100; ++j)
        {
            std::random_shuffle(perm.begin(), perm.end());

            const sched_t sched = perm2sched(task, perm);
            const sched_t slow_sched = slow_perm2sched(task, perm);
        
            const cost_t cost = get_cost(task, sched);
            const cost_t slow_cost = get_cost(task, slow_sched);
            if (fabs(slow_cost - cost) > 0.001)
            {
                cout << "Cost: " << cost << ": " << slow_cost << endl;
                ++fail_count;
            }
        }

    }

    cout << fail_count << endl;
    
    return 0;         
}


int bmain()
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

    perm_t perm1(2);
    perm1.resize(10);
    std::generate(perm1.begin(), perm1.end(), perm_generator());
    std::next_permutation(perm1.begin(), perm1.end());
    std::copy(perm1.begin(), perm1.end(), std::ostream_iterator<perm_t::value_type>(cout, " "));
    cout << endl;

    return 0;
}