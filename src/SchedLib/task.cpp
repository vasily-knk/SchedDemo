#include "stdafx.h"

cost_t get_cost(const task_t &task, const sched_t &sched)
{
    cost_t cost = 0;
    for (sched_t::const_iterator it = sched.begin(); it != sched.end(); ++it)
    {
        const size_t job = it->first;
        const moment_t job_sched = it->second;
        if (job_sched < task[job].min_bound || job_sched > task[job].max_bound)
            return std::numeric_limits<cost_t>::max();

        const moment_t deviation = std::abs(job_sched - task[job].due);
        if (job_sched - task[job].due > 0)
            cost += task[job].tweight() * deviation;
        else
            cost += task[job].eweight() * deviation;
    }
    return cost;
}


/*cost_t get_cost_partial(const task_t &task, const sched_t &sched, const perm_t &perm, const size_t start, const size_t end)
{
    cost_t cost = 0;
    for (size_t pos = start; pos < end; ++pos)
    {
        const size_t job = perm[pos];
        
        if (sched[job] < task[job].min_bound || sched[job] > task[job].max_bound)
            return std::numeric_limits<cost_t>::max();

        const moment_t deviation = std::abs(sched[job] - task[job].due);
        if (sched[job] - task[job].due > 0)
            cost += task[job].tweight() * deviation;
        else
            cost += task[job].eweight() * deviation;
    }
    return cost;
}*/

bool check_feasible(const task_t &task, const perm_t &perm);

cost_t calculate_cost(const task_t &task, const perm_t &perm)
{
    if (!check_feasible(task, perm))
        return std::numeric_limits<cost_t>::max();
    sched_t sched = perm2sched (task, perm);
    return get_cost(task, sched);
}

bool check_feasible(const task_t &task, const perm_t &perm)
{
    moment_t last = -std::numeric_limits<moment_t>::max();
    
    //sched_t sched();
    for (size_t pos = 0; pos < perm.size(); ++pos)
    {
        const size_t job = perm[pos];

        const moment_t job_sched = std::max(last, task[job].min_bound);

        //sched[job] = std::max(last, task[job].min_bound);
        if (job_sched > task[job].max_bound)
            return false;

        last = job_sched + get_processing_time(task, perm, pos);
    }
    return true;

}

/*size_t get_unfeasible_pos(const task_t &task, const perm_t &perm)
{
    assert(task.size() == perm.size());

    moment_t last = -std::numeric_limits<moment_t>::max();

    sched_t sched(task.size());
    for (size_t pos = 0; pos < task.size(); ++pos)
    {
        const size_t job = perm[pos];

        sched[job] = std::max(last, task[job].min_bound);
        if (sched[job] > task[job].max_bound)
            return pos;

        last = sched[job] + get_processing_time(task, perm, pos);
    }
    return task.size();
}*/

task_t apply_permutation(const task_t &task, const perm_t &perm)
{
    task_t temp(perm.size());
    for (size_t i = 0; i < perm.size(); ++i)
    {
        temp[i] = task[perm[i]];
        /*temp[i].spans.resize(perm.size());
        for (size_t j = 0; j < perm.size(); ++j)
            temp[i].spans[j] = task[perm[i]].spans[perm[j]];*/
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

/*perm_t select_sub_task(const task_t &src_task, const sched_t &sched, const moment_t start, const moment_t end)
{
    perm_t mappings;
    
    for (size_t i = 0; i < src_task.size(); ++i)
    {
        if (sched[i] >= start && sched[i] < end)
            mappings.push_back(i);
    }

    return mappings;
}
*/