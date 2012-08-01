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


typedef vector<size_t> perm_t;

typedef vector<job_t> task_t;
typedef vector<moment_t> sched_t;




void perm2sched(const task_t &task, const perm_t &perm, sched_t &out);
cost_t get_cost(const task_t &task, const sched_t &sched);

#endif // TASK_H
