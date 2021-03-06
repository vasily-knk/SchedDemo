#include "stdafx.h"

namespace
{

    enum plane_class
    {
        LIGHT = 0, MEDIUM = 1, HEAVY = 2
    };

    float get_class_wait(plane_class i, plane_class j)
    {
        if (i == LIGHT)
            return 1;
        else if (i == HEAVY)
            return 3;
        else
        {
            return 2;
            /*if (j == HEAVY)
                return 2;
            else
                return 3;*/
        }

    }

}

inline bool job_cmp(const job_t &job1, const job_t &job2)
{
    return (job1.due < job2.due);
}

void planes_task_test(float, task_t &out_task)
{
    out_task[0].due = 2;
    out_task[1].due = 0;
    out_task[2].due = 1;

    out_task[0].eweight = out_task[0].tweight = 2.5;
    out_task[1].eweight = out_task[1].tweight = 1;
    out_task[2].eweight = out_task[2].tweight = 2;

    out_task[0].min_bound = -100;
    out_task[1].min_bound = -100;
    out_task[2].min_bound = -100;

    std::fill(out_task[0].spans.begin(), out_task[0].spans.end(), 4.0f);
    std::fill(out_task[1].spans.begin(), out_task[1].spans.end(), 4.0f);
    std::fill(out_task[2].spans.begin(), out_task[2].spans.end(), 4.0f);
}

void planes_task(float timespan, task_t &out_task)
{
    typedef mt19937 gen;
    gen randgen(static_cast<unsigned int>(std::time(0)));

    uniform_real_distribution<float> classes_distr(0, 1);
    uniform_real_distribution<float> dates_distr(0, timespan);

    //variate_generator<gen, uniform_real_distribution<float>> classes_gen(randgen, classes_distr);

    //boost::variate_generator<gen, uniform_real_distribution<float>> dates_gen(randgen, dates_distr);


    const size_t n = out_task.size();

    vector<plane_class> classes(out_task.size());
    for (size_t i = 0; i < out_task.size(); ++i)
    {
        float p = classes_distr(randgen);
        if (p < 0.2)
            classes[i] = LIGHT;
        else if (p > 0.8)
            classes[i] = HEAVY;
        else
            classes[i] = MEDIUM;
    }


    for (size_t i = 0; i < out_task.size(); ++i)
    {
        out_task[i].spans.resize(out_task.size());
        for (size_t j = 0; j < out_task.size(); ++j)
        {
            out_task[i].spans[j] = get_class_wait(classes[i], classes[j]);
        }
    }


    for (size_t i = 0; i < n; ++i)
    {
        out_task[i].due = dates_distr(randgen);
        out_task[i].min_bound = -100.0f;//t[i].due;
        out_task[i].eweight = out_task[i].tweight = .2f + classes_distr(randgen) * 1.8f;
    }

    //out_task[3].eweight = out_task[3].tweight = 5;



    /*std::generate(t.get_due(), t.get_due() + n, dates_gen);
    std::sort (t.get_due(), t.get_due() + n);
    std::copy (t.get_due(), t.get_due() + n, t.get_min_bound());
    std::fill (t.get_max_bound(), t.get_max_bound() + n, std::numeric_limits<moment>::max());

    for (size_t i = 0; i < n; ++i)
    {
        t.get_tweight() [i] = classes_gen();
        t.get_eweight() [i] = 0;
    }*/

    //return t;
}

void planes_task1(const float timespan, task_t &out_task)
{
    const moment_t span = timespan / out_task.size();
    for (size_t i = 0; i < out_task.size(); ++i)
    {
        out_task[i].due = i * span;
        out_task[i].min_bound = 0;
        out_task[i].eweight = out_task[i].tweight = i * 10.0f + 1.0f;
        
        std::fill(out_task[i].spans.begin(), out_task[i].spans.end(), span * 0.5f);
    }
}

task_t planes_task_with_bounds(const size_t num_planes, const moment_t timespan, const moment_t bound_timespan)
{
    mt19937 randgen(static_cast<unsigned int>(std::time(0)));
    uniform_real_distribution<float> classes_distr(0, 1);
    uniform_real_distribution<moment_t> bounds_distr(bound_timespan * 0.7, bound_timespan);
    uniform_real_distribution<moment_t> dates_distr(bound_timespan, timespan);
    uniform_real_distribution<cost_t> weights_distr(0.2, 1.0);

    task_t task(num_planes);

    vector<plane_class> classes(num_planes);
    for (size_t i = 0; i < num_planes; ++i)
    {
        float p = classes_distr(randgen);
        if (p < 0.2)
            classes[i] = LIGHT;
        else if (p > 0.8)
            classes[i] = HEAVY;
        else
            classes[i] = MEDIUM;
    }

    for (size_t i = 0; i < num_planes; ++i)
        for (size_t j = 0; j < num_planes; ++j)
            task[i].spans[j] = get_class_wait(classes[i], classes[j]);

    for (size_t i = 0; i < num_planes; ++i)
    {
        task[i].due = dates_distr(randgen);
        task[i].min_bound = std::max(task[i].due - bounds_distr(randgen), moment_t(0));
        //task[i].min_bound = task[i].due;
        task[i].max_bound = task[i].due + bounds_distr(randgen);
        
        task[i].eweight = task[i].tweight = weights_distr(randgen);
    }

    perm_t due_perm = due_dates_perm(task);
    task = apply_permutation(task, due_perm);
    return task;
}

