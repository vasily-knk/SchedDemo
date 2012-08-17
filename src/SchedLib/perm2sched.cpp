#include "stdafx.h"

namespace
{
    typedef multimap<moment_t, size_t> jobs_list_t;
    
    void add_job(const task_t &task, const perm_t &perm, 
                 jobs_list_t &jobs_list, const size_t pos, 
                 sched_t *out_sched, moment_t *out_offset)
    {
        const size_t n = task.size();
        
        if (pos == n - 1)
        {
            
            return;
        }

    }
}

sched_t perm2sched(const task_t &task, const perm_t &perm)
{
    assert (!task.empty());
    assert (task.size() == perm.size());
    assert (task.size() == out_sched.size());

    const size_t n = task.size();

    jobs_list_t jobs_list;
    moment_t offset = 0;
    cost_t penalty_offset = 0;
    sched_t sched;

    // Hanging last job
    jobs_list.insert(jobs_list_t::value_type(0, n - 1));
    sched[perm.back()] = task[perm.back()].due;

    for (int pos = n - 2; pos >= 0; --pos)
    {
        const size_t job = perm[pos];
        const size_t next_job = perm[pos + 1];
        
        const moment_t original_time = sched[next_job] - get_processing_time(task, perm, pos);
        //const moment_t earliness = std::max<moment_t>(original_time - task[job].due, 0);
        moment_t final_time;
        moment_t earliness;
        cost_t weight;
        
        if (original_time >= task[job].due)
        {
            // Free-hanging job
            final_time = task[job].due;
            weight = task[job].tweight;
        }
        else
        {
            // Early job
            jobs_list_t::iterator it;
            for (it = jobs_list.begin(); it != jobs_list.end(); ++it)
            {
                // Job at its due date, no need to move further
                if (it->first - offset > task[job].due - original_time)
                {
                    final_time = task[job].due;
                    break;
                }
                
                // Enough penalty gathered
                if (penalty_offset + it->second > task[job].eweight)
                    break;
                
                // Both calculating sum of penalties for comparison and updating offset for penalties to be removed
                penalty_offset += it->second;
            }
            
            final_time = (it->first - offset) + original_time;

            // Removing penalties for late jobs (added to offset)
            jobs_list.erase(jobs_list.begin(), it);
            
            penalty_offset -= task[job].eweight;
            weight = task[job].eweight + task[job].tweight;
        }

        offset += task[job].due - original_time;
        
        const moment_t freedom = offset + task[job].due - final_time;
        jobs_list.insert(jobs_list_t::value_type(freedom, weight));
    }
}
