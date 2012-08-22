#include "stdafx.h"

cost_t get_cost(const task_t &task, const sched_t &sched)
{
    assert(task.size() == sched.size());

    cost_t cost = 0;
    for (size_t i = 0; i < task.size(); ++i)
    {
        if (sched[i] < task[i].min_bound || sched[i] > task[i].max_bound)
            return std::numeric_limits<cost_t>::max();

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
    if (!check_feasible(task, perm))
        return std::numeric_limits<cost_t>::max();
    sched_t sched = perm2sched (task, perm);
    return get_cost(task, sched);
}

bool check_feasible(const task_t &task, const perm_t &perm)
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

task_t apply_permutation(const task_t &task, const perm_t &perm)
{
    task_t temp(perm.size());
    for (size_t i = 0; i < perm.size(); ++i)
    {
        temp[i] = task[perm[i]];
        temp[i].spans.resize(perm.size());
        for (size_t j = 0; j < perm.size(); ++j)
            temp[i].spans[j] = task[perm[i]].spans[perm[j]];
    }
    return temp;
}

perm_t due_dates_perm(const task_t &t)
{
    perm_t perm(t.size());
    std::sort(perm.begin(), perm.end(), [&](size_t i, size_t j) -> bool
    {
        return (t[i].due < t[j].due);
    });
    return perm;
}

perm_t select_sub_task(const task_t &src_task, const sched_t &sched, const moment_t start, const moment_t end)
{
    perm_t mappings;
    
    for (size_t i = 0; i < src_task.size(); ++i)
    {
        if (sched[i] >= start && sched[i] < end)
            mappings.push_back(i);
    }

    return mappings;
}
