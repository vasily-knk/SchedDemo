#include "stdafx.h"

namespace
{
    const string names[] = 
    {
#include "names.txt"
    };
}


task_t planes_task_with_bounds(const size_t num_planes, const moment_t timespan, const moment_t bound_timespan)
{
    mt19937 randgen(static_cast<unsigned int>(std::time(0)));
    uniform_real_distribution<float> classes_distr(0, 1);
    uniform_real_distribution<float> priorities_distr(0, 1);
    uniform_real_distribution<moment_t> bounds_distr(bound_timespan * moment_t(0.7), bound_timespan);
    uniform_real_distribution<moment_t> dates_distr(bound_timespan, timespan);
    uniform_int_distribution<size_t> names_distr(0, sizeof(names) / sizeof(string));

    task_t task(num_planes);

    for (size_t i = 0; i < num_planes; ++i)
    {
        float p = classes_distr(randgen);
        if (p < 0.2)
            task[i].class_ = aircraft_t::LIGHT;
        else if (p > 0.8)
            task[i].class_ = aircraft_t::HEAVY;
        else
            task[i].class_ = aircraft_t::MEDIUM;
    }

    for (size_t i = 0; i < num_planes; ++i)
    {
        float p = priorities_distr(randgen);
        if (p < 0.2)
            task[i].priority_ = aircraft_t::RED;
        else if (p > 0.8)
            task[i].priority_ = aircraft_t::GREEN;
        else
            task[i].priority_ = aircraft_t::YELLOW;
    }

    for (size_t i = 0; i < num_planes; ++i)
    {
        task[i].name = ::names[i];
        task[i].due = dates_distr(randgen);
        task[i].min_bound = std::max(task[i].due - bounds_distr(randgen), moment_t(0));
        //task[i].min_bound = task[i].due;
        task[i].max_bound = task[i].due + bounds_distr(randgen);
    }

    perm_t due_perm = due_dates_perm(task);
    task = apply_permutation(task, due_perm);
    return task;
}
