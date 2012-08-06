#include "stdafx.h"
#include "task.h"

namespace
{

    typedef multimap<moment_t, cost_t> sections_array_t;
    typedef sections_array_t::value_type section_t;

}


inline void insert_section(sections_array_t& dst, const moment_t time, const cost_t gain, const moment_t &lower, const moment_t &upper)
{
    if (lower <= time && upper >= time)
        dst.insert(section_t(time, gain));
}

void add_job(const task_t &task, const perm_t &perm, size_t job, sched_t &out_sched)
{
    const size_t n = task.size();
    sections_array_t sections;

    for (size_t i = job + 1; i < n; ++i)
    {
        if (task[i].due < out_sched[i])        
        {
            sections.insert(section_t(out_sched[i], task[i].tweight));
        }
        else
        {
            sections.insert(section_t(out_sched[i], -task[i].eweight));
            sections.insert(section_t(out_sched[i], task[i].tweight + task[i].eweight));
        }
    }

    cost_t total_gain = 0;
    for (sections_array_t::iterator it = sections.begin(); it != sections.end(); ++it)
    {
        it->second += total_gain;
        total_gain = it->second;
    }

    

}

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
cost_t calculate_cost(const task_t &task, const perm_t &perm)
{
    sched_t sched(task.size());
    perm2sched (task, perm, sched);
    return get_cost(task, sched);
}
