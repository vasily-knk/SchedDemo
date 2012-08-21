#pragma once

typedef float moment_t;
typedef float cost_t;

struct job_t
{
    moment_t due;
    vector<moment_t> spans;
    cost_t eweight, tweight;
    moment_t min_bound, max_bound;
};


struct perm_t : vector<size_t> 
{
	perm_t(size_t n)
		: vector<size_t>(n)
	{
		for (iterator it = begin(); it != end(); ++it)
			*it = (it - begin());
	}
};


struct task_t : vector<job_t>
{
    task_t()
    {
    }

	task_t(size_t n)
		: vector<job_t>(n)
	{
		for (iterator it = begin(); it != end(); ++it)
			it->spans.resize(n);
	}
};

typedef vector<moment_t> sched_t;

inline moment_t get_processing_time(const task_t &task, const perm_t &perm, const size_t pos)
{
    if (pos == task.size() - 1)
        return 0;

    return task[perm[pos]].spans[perm[pos + 1]];
}

sched_t slow_perm2sched(const task_t &task, const perm_t &perm);
sched_t perm2sched(const task_t &task, const perm_t &perm);
cost_t get_cost(const task_t &task, const sched_t &sched);
cost_t calculate_cost(const task_t &task, const perm_t &perm);
bool check_feasible(task_t &task, const perm_t &perm);

typedef boost::function<perm_t(const task_t &, const perm_t &)> solver_t;

