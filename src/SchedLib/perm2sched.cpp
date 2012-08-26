#include "stdafx.h"

namespace {

    class perm2sched_context
    {
    public:
        perm2sched_context(const task_t &task, const perm_t &perm);
        const sched_t &calculate_sched ();

        const sched_t &get_sched() const {return sched;}
    private:
        void move_jobs_list(moment_t move);
        void insert_job(moment_t moment, cost_t cost);

        void add_last_job();
        void add_free_hanging_job(const size_t pos);
        void add_early_job(const size_t pos);
        void add_job(const size_t pos);
    private:
        typedef multimap<moment_t, cost_t> jobs_list_t;

        const task_t &task;
        const perm_t &perm;
        sched_t sched;
        jobs_list_t jobs_list;
        moment_t offset;
        moment_t max_push;

    private:
        perm2sched_context &operator=(const perm2sched_context &);
    };

    perm2sched_context::perm2sched_context(const task_t &task, const perm_t &perm)
        : task(task)
        , perm(perm)
        , sched(task.size())
    {
        assert (!task.empty());
        assert (task.size() == perm.size());
    }

    const sched_t &perm2sched_context::calculate_sched()
    {
        const size_t n = task.size();

        sched.resize(n, 0);
        jobs_list.clear();
        offset = 0;

        add_last_job();
        for (int pos = n - 2; pos >= 0; --pos)
            add_job(pos);

        for (size_t pos = 1; pos < n; ++pos)
        {
            const size_t job = perm[pos];
            const size_t prev_job = perm[pos - 1];
            sched[job] = std::max<moment_t>(sched[prev_job] + get_processing_time(task, perm, pos - 1), sched[job]);
        }

        return sched;
    }




    void perm2sched_context::move_jobs_list(const moment_t move)
    {
        offset += move;
        max_push += move;
    }

    void perm2sched_context::insert_job(moment_t moment, cost_t cost)
    {
        const jobs_list_t::value_type value(moment - offset, cost);
        jobs_list.insert(value);
    }

    void perm2sched_context::add_last_job()
    {
        const size_t job = perm.back();
        insert_job(moment_t(0), task[job].tweight());
        sched[job] = task[job].due;
        max_push = task[job].max_bound - sched[job];
    }

    void perm2sched_context::add_job(const size_t pos)
    {
        const size_t job = perm[pos];
        const size_t next_job = perm[pos + 1];

        sched[job] = sched[next_job] - get_processing_time(task, perm, pos);

        max_push = std::min(max_push, task[job].max_bound - sched[job]);

        if (sched[job] > task[job].due)
            add_free_hanging_job(pos);
        else
            add_early_job(pos);
    }

    void perm2sched_context::add_free_hanging_job(const size_t pos)
    {
        const size_t job = perm[pos];

        move_jobs_list(sched[job] - task[job].due);
        insert_job(moment_t(0), task[job].tweight());
        sched[job] = task[job].due;
    }

    void perm2sched_context::add_early_job(const size_t pos)
    {
        const size_t job = perm[pos];

        const moment_t original_time = sched[job];
        moment_t final_time = original_time;

        insert_job(task[job].due - original_time, task[job].eweight() + task[job].tweight());

        cost_t gathered_penalty = -task[job].eweight();
        jobs_list_t::iterator it;
        for (it = jobs_list.begin(); it != jobs_list.end(); ++it)
        {
            const moment_t push = it->first + offset;

            if (gathered_penalty > 0 && push > task[job].min_bound - original_time)
                break;

            gathered_penalty += it->second;

            if (gathered_penalty > 0)
                final_time = original_time + push;
        }

        assert(gathered_penalty > 0);
        final_time = std::max(final_time, task[job].min_bound);
        final_time = std::min(final_time, original_time + max_push);

        sched[job] = final_time;

        jobs_list.erase(jobs_list.begin(), it);

        move_jobs_list(-final_time + original_time);
        insert_job(moment_t(0), gathered_penalty);
    }

}

sched_t perm2sched(const task_t &task, const perm_t &perm)
{
    perm2sched_context context(task, perm);
    return context.calculate_sched();
}
