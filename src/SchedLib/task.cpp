#include "stdafx.h"

namespace
{
    typedef multimap<moment_t, cost_t> sections_array_t;
    typedef sections_array_t::value_type section_t;

}



void add_job(const task_t &task, const perm_t &perm, const size_t pos, sched_t &out_sched)
{
    const size_t n = task.size();
    const size_t job = perm[pos];
    if (pos == n - 1)
    {
        out_sched[job] = task[job].due;
        return;
    }
    
    sections_array_t sections;

    moment_t offset = get_processing_time(task, perm, pos);
    for (size_t pos1 = pos + 1; pos1 < n; ++pos1)
    {
        const size_t job1 = perm[pos1];        
        if (task[job1].due < out_sched[job1])        
        {
            sections.insert(section_t(out_sched[job1] - offset, task[job1].tweight));
        }
        else
        {
            sections.insert(section_t(out_sched[job1] - offset, -task[job1].eweight));
            sections.insert(section_t(task[job1].due - offset, task[job1].tweight + task[job1].eweight));
        }

        offset += get_processing_time(task, perm, pos1);
    }

    sections.insert(section_t(task[job].min_bound, 0));
    sections.insert(section_t(task[job].due, task[job].eweight + task[job].tweight));

    cost_t total_gain = 0;
    for (sections_array_t::iterator it = sections.begin(); it != sections.end(); ++it)
    {
        it->second += total_gain;
        total_gain = it->second;
    }
    for (sections_array_t::iterator it = sections.begin(); it != sections.end(); ++it)
        it->second -= task[job].eweight;

    const sections_array_t::const_iterator loc = std::find_if(sections.begin(), sections.end(), [](const section_t &s) -> bool
    {
        return (s.second >= 0);
    });

    out_sched[job] = std::min(loc->first, task[job].due);
    out_sched[job] = std::max(out_sched[job], task[job].min_bound);

    for (size_t pos1 = pos + 1; pos1 < n; ++pos1)
    {
        const size_t job1 = perm[pos1];
        const size_t prev_job = perm[pos1 - 1]; 
        const moment_t last = out_sched[prev_job] + task[prev_job].spans[job1];
        out_sched[job1] = std::max(out_sched[job1], last);
    }
}

void slow_perm2sched(const task_t &task, const perm_t &perm, sched_t &out_sched)
{
    assert (task.size() == perm.size());
    assert (task.size() == out_sched.size());

    /*moment_t last = 0;
    for (size_t i = 0; i < task.size(); ++i)
    {
        out[perm[i]] = std::max(last, task[perm[i]].min_bound);
        if (i < task.size() - 1)
            last = out[perm[i]] + task[perm[i]].spans[perm[i + 1]];
    }*/
    for (int job = task.size() - 1; job >= 0; --job)
    {
        add_job(task, perm, job, out_sched);
    }
}

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
    sched_t sched(task.size());
    slow_perm2sched (task, perm, sched);
    return get_cost(task, sched);
}


/*
void move_job(const task_t &task, const perm_t &perm, const size_t pos, const moment_t delta, sched_t &sched)
{
    const size_t n = task.size();
    if (delta > 0)
    {
        for (size_t pos1 = pos + 1; pos1 < n; ++pos1)
        {
            sched[perm[pos1 - 1]] + get_processing_time(task, perm, )
        }
    }
}
  */