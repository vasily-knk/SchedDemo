#include "stdafx.h"
#include "task.h"

void perm2sched(const task_t &task, const perm_t &perm, sched_t &out)
{
    assert (task.size() == perm.size());
    assert (task.size() == out.size());

    moment_t last = 0;
    for (size_t i = 0; i < task.size(); ++i)
    {
        out[perm[i]] = std::max(last, task[perm[i]].min_bound);
        if (i < task.size() - 1)
            last = out[perm[i]] + task[perm[i]].spans[perm[i + 1]];
    }
}

cost_t get_cost(const task_t &task, const sched_t &sched)
{
    assert (task.size() == sched.size());

    cost_t cost = 0;
    for (size_t i = 0; i < task.size(); ++i)
    {
        const moment_t tardiness = std::max<moment_t>(sched[i] - task[i].due, 0);
        cost += task[i].tweight * tardiness;
    }
    return cost;
}
