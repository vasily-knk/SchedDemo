#ifndef TASK_H
#define TASK_H

typedef float moment_t;
typedef float cost_t;

struct job_t
{
    moment_t due;
    vector<moment_t> spans;
    cost_t tweight;
    moment_t min_bound;
};


struct perm_t : vector<moment_t> 
{
	perm_t(size_t n)
		: vector<moment_t>(n)
	{
		for (iterator it = begin(); it != end(); ++it)
			*it = (it - begin());
	}
};


struct task_t : vector<job_t>
{
	task_t(size_t n)
		: vector<job_t>(n)
	{
		for (iterator it = begin(); it != end(); ++it)
			it->spans.resize(n);
	}
};

typedef vector<moment_t> sched_t;

/*struct sched_t : vector<moment_t> 
{
	sched_t(size_t n)
		: vector<moment_t>(n)
	{
		for (iterator it = begin(); it != end(); ++it)
			*it = (it - begin());
	}
};

*/


void perm2sched(const task_t &task, const perm_t &perm, sched_t &out);
cost_t get_cost(const task_t &task, const sched_t &sched);

#endif // TASK_H
