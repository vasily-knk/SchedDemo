#pragma once

typedef float moment_t;
typedef float cost_t;

/*struct job_t
{
    moment_t due;
    vector<moment_t> spans;
    cost_t eweight, tweight;
    moment_t min_bound, max_bound;
};*/

#include "aircraft.h"
typedef aircraft_t job_t;


struct perm_t : vector<size_t> 
{
	perm_t()
    {

    }

    perm_t(size_t n)
		: vector<size_t>(n)
	{
		for (iterator it = begin(); it != end(); ++it)
			*it = (it - begin());
	}
};

class perm_generator
{
public:
    typedef perm_t::value_type T;

    perm_generator(const T& start_value = 0) : val_(start_value)
    {

    }

    T operator() ()
    {
        return val_++;
    }
private:
    T val_;
};


typedef vector<job_t> task_t;

/*
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
*/

typedef vector<moment_t> sched_t;

inline moment_t get_processing_time(const task_t &task, const perm_t &perm, const size_t pos)
{
    if (pos == perm.size() - 1)
        return task[perm[pos]].span(task[perm[pos]]);

    return task[perm[pos]].span(task[perm[pos + 1]]);
}

sched_t slow_and_broken_perm2sched(const task_t &task, const perm_t &perm);
sched_t perm2sched(const task_t &task, const perm_t &perm);
cost_t get_cost(const task_t &task, const sched_t &sched);
cost_t get_cost_partial(const task_t &task, const sched_t &sched, const perm_t &perm, const size_t start, const size_t end);


cost_t calculate_cost(const task_t &task, const perm_t &perm);
bool check_feasible(const task_t &task, const perm_t &perm);
size_t get_unfeasible_pos(const task_t &task, const perm_t &perm);

typedef boost::function<perm_t(const task_t &, const perm_t &)> solver_t;

task_t apply_permutation(const task_t &task, const perm_t &perm);

perm_t due_dates_perm(const task_t &t);
perm_t select_sub_task(const task_t &src_task, const sched_t &sched, const moment_t start, const moment_t end);
