#include "stdafx.h"

namespace
{
    typedef multimap<moment_t, cost_t> jobs_list_t;

    void move_jobs_list(jobs_list_t &jobs_list, const moment_t offset)
    {
        jobs_list_t temp;
        
        for (jobs_list_t::const_iterator it = jobs_list.begin(); it != jobs_list.end(); ++it)
            temp.insert(jobs_list_t::value_type(it->first + offset, it->second));

        std::swap(jobs_list, temp);
    }
    
    void add_last_job(const task_t &task, const perm_t &perm, sched_t &sched, jobs_list_t &jobs_list)
    {
        const size_t job = perm.back();
        jobs_list.insert(jobs_list_t::value_type(moment_t(0), task[job].tweight));
        sched[job] = task[job].due;
    }

    void add_free_hanging_job(const task_t &task, const perm_t &perm, const size_t pos, sched_t &sched, jobs_list_t &jobs_list)
    {
        const size_t job = perm[pos];
        
        move_jobs_list(jobs_list, sched[job] - task[job].due);
        jobs_list.insert(jobs_list_t::value_type(moment_t(0), task[job].tweight));
        sched[job] = task[job].due;
    }

    void add_early_job(const task_t &task, const perm_t &perm, const size_t pos, sched_t &sched, jobs_list_t &jobs_list)
    {
        const size_t job = perm[pos];

        moment_t final_time = task[job].due;

        bool reached_due = true;
        cost_t gathered_penalty = -task[job].eweight;

        moment_t last_push = 0;
        jobs_list_t::iterator it;
        for (it = jobs_list.begin(); it != jobs_list.end(); ++it)
        {
            // Job at its due date, no need to move further
            if (it->first > task[job].due - sched[job])
            {
                break;
            }

            gathered_penalty += it->second;

            // Enough penalty gathered
            if (gathered_penalty > 0)
            {
                reached_due = false;
                final_time = sched[job] + it->first;
                ++it;
                break;
            }
        }
        
        // Removing penalties for late jobs
        jobs_list.erase(jobs_list.begin(), it);

        move_jobs_list(jobs_list, sched[job] - final_time);

        if (reached_due)
        {
            jobs_list.insert(jobs_list_t::value_type(moment_t(0), gathered_penalty + task[job].eweight + task[job].tweight));
        }
        else
        {
            const moment_t moment1 = 0;
            const moment_t moment2 = task[job].due - final_time;

            jobs_list.insert(jobs_list_t::value_type(moment1, gathered_penalty));
            jobs_list.insert(jobs_list_t::value_type(moment2, task[job].eweight + task[job].tweight));
        }

        sched[job] = final_time;
    }
    

    void add_job(const task_t &task, const perm_t &perm, const size_t pos, sched_t &sched, jobs_list_t &jobs_list)
    {
        const size_t job = perm[pos];
        const size_t next_job = perm[pos + 1];

        sched[job] = sched[next_job] - get_processing_time(task, perm, pos);

        if (sched[job] > task[job].due)
            add_free_hanging_job(task, perm, pos, sched, jobs_list);
        else
            add_early_job(task, perm, pos, sched, jobs_list);
    }
    
}


sched_t perm2sched(const task_t &task, const perm_t &perm)
{
    assert (!task.empty());
    assert (task.size() == perm.size());

    const size_t n = task.size();

    jobs_list_t jobs_list;
    sched_t sched(n);
    
    add_last_job(task, perm, sched, jobs_list);
    for (int pos = n - 2; pos >= 0; --pos)
        add_job(task, perm, pos, sched, jobs_list);
    
    for (size_t pos = 1; pos < n; ++pos)
    {
        const size_t job = perm[pos];
        const size_t prev_job = perm[pos - 1];
        sched[job] = std::max<moment_t>(sched[prev_job] + get_processing_time(task, perm, pos - 1), sched[job]);
    }
    
    return sched;
}


#if 0
sched_t perm2sched1(const task_t &task, const perm_t &perm)
{
    assert (!task.empty());
    assert (task.size() == perm.size());

    const size_t n = task.size();

    jobs_list_t jobs_list;
    moment_t offset = 0;

    sched_t sched(n);

    // Hanging last job
    const size_t last_job = perm.back();
    const moment_t m = 0;

    auto v = std::make_pair(m, task[last_job].tweight);
                                      
    jobs_list.insert(v);
    sched[last_job] = task[last_job].due;

    vector<std::pair<moment_t, cost_t> > to_add;


    for (int pos = n - 2; pos >= 0; --pos)
    {
        to_add.clear();
        
        const size_t job = perm[pos];
        const size_t next_job = perm[pos + 1];
        
        const moment_t original_time = sched[next_job] - get_processing_time(task, perm, pos);
        moment_t final_time;
        cost_t weight;
        
        if (original_time >= task[job].due)
        {
            // Free-hanging job
            final_time = task[job].due;
            //to_add.push_back(std::make_pair(moment_t(0), task[job].tweight));
        }
        else
        {
            bool reached_due = true;
            cost_t gathered_penalty = -task[job].eweight;
            
            // Early job
            jobs_list_t::iterator it;
            moment_t last_push = 0;
            for (it = jobs_list.begin(); it != jobs_list.end(); ++it)
            {
                // Job at its due date, no need to move further
                if (it->first - offset > task[job].due - original_time)
                    break;
                
                // Enough penalty gathered
                if (gathered_penalty > 0)
                {
                    reached_due = false;
                    final_time = last_push - offset + original_time;
                    break;
                }
                
                // Both calculating sum of penalties for comparison 
                // and updating offset for penalties to be removed
                gathered_penalty += it->second;
                last_push = it->first;
            }
            // Removing penalties for late jobs (added to offset)
            jobs_list.erase(jobs_list.begin(), it);
            
            if (reached_due)
            {
                final_time = task[job].due;
                weight += task[job].tweight;
                //to_add.push_back(jobs_list_t::value_type(moment_t(0), gathered_penalty));
            }
            else
            {
                const moment_t moment1 = 0;
                const moment_t moment2 = task[job].due - final_time;
                
                //to_add.push_back(jobs_list_t::value_type(moment1, gathered_penalty));
                //to_add.push_back(jobs_list_t::value_type(moment2, task[job].eweight + task[job].tweight));
            }
        }

        offset += final_time - original_time;
        /*for (auto it = to_add.cbegin(); it != to_add.cend(); ++it)
        {
            auto val = *it;
            val.second += offset;
            jobs_list.insert(val);
        }*/
        
        sched[job] = final_time;
    }

    for (size_t pos = 1; pos < n; ++pos)
    {
        const size_t job = perm[pos];
        const size_t prev_job = perm[pos - 1];
        sched[job] = std::max<moment_t>(sched[prev_job] + get_processing_time(task, perm, pos - 1), 
                                        sched[job]);
    }
    
    return sched;
}
#endif
