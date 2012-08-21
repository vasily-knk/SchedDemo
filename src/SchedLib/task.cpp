#include "stdafx.h"



cost_t get_cost(const task_t &task, const sched_t &sched)
{
    assert (task.size() == sched.size());

    cost_t cost = 0;
    for (size_t i = 0; i < task.size(); ++i)
    {
        const moment_t deviation = std::abs(sched[i] - task[i].due);
        if (sched[i] - task[i].due > 0)
            cost += task[i].tweight * deviation;
        else
            cost += task[i].eweight * deviation;
    }
    return cost;
}

cost_t calculate_cost(const task_t &task, const perm_t &perm)
{
    sched_t sched = perm2sched (task, perm);
    return get_cost(task, sched);
}

bool check_feasible(task_t &task, const perm_t &perm)
{
    assert(task.size() == perm.size());
    
    moment_t last = -std::numeric_limits<moment_t>::max();
    
    sched_t sched(task.size());
    for (size_t pos = 0; pos < task.size(); ++pos)
    {
        const size_t job = perm[pos];

        sched[job] = std::max(last, task[job].min_bound);
        if (sched[job] > task[job].max_bound)
            return false;

        last = sched[job] + get_processing_time(task, perm, pos);
    }
    return true;

}

