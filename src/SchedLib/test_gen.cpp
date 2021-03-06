#include "stdafx.h"

void fast_job(const moment_t due, const cost_t weight, const moment_t proc, job_t *job)
{
    job->due = due;
    job->eweight = job->tweight = weight;
    job->min_bound = -10000;
    job->max_bound = 10000;
    std::fill(job->spans.begin(), job->spans.end(), proc);
}

job_t fast_job(const moment_t due, const cost_t weight, const moment_t proc, const size_t n)
{
    job_t job;
    job.due = due;
    job.eweight = job.tweight = weight;
    job.min_bound = -10000;
    job.max_bound = 10000;
    job.spans.resize(n, proc);

    return job;
}

job_t fast_job(const moment_t due, const cost_t weight, const moment_t proc, const moment_t lb, const moment_t ub, const size_t n)
{
    job_t job;
    job.due = due;
    job.eweight = job.tweight = weight;
    job.min_bound = lb;
    job.max_bound = ub;
    job.spans.resize(n, proc);

    return job;
}

task_t gen_task1()
{
    const size_t n = 3;

    task_t task;
    //task.push_back(fast_job(0, 2.5, 2, n));
    task.push_back(fast_job(0, 2.0, 2, n));
    task.push_back(fast_job(-3, 1.0, 2, n));
    task.push_back(fast_job(0, 1.8, 2, n));

    task[1].max_bound = -2.5;

    return task;
}


task_t gen_task2()
{
    const size_t n = 3;

    task_t task;
    //task.push_back(fast_job(4, 1, 3, n));
    task.push_back(fast_job(2, 2, 1, n));
    task.push_back(fast_job(9, 2.5, 1, n));
    task.push_back(fast_job(5, 3, 1, n));
    
    return task;
}

task_t gen_task3()
{
    const size_t n = 2;

    task_t task;

    task.push_back(fast_job(2.0, 1.0, 10.0, 1.0, 3.0, n));
    task.push_back(fast_job(4.0, 1.5, 3.0, 3.0, 5.0, n));

    return task;
}


