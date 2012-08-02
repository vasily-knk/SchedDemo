#include "stdafx.h"

enum plane_class
{
    LIGHT = 0, MEDIUM = 1, HEAVY = 2
};

float get_class_wait(plane_class i, plane_class j)
{
    if (i == LIGHT)
        return 1;
    else if (i == HEAVY)
        return 4;
    else
    {
        if (j == HEAVY)
            return 2;
        else
            return 3;
    }

}

bool job_cmp(const job_t &job1, const job_t &job2)
{
    return (job1.due < job2.due);
}

void planes_task(float timespan, task_t &t)
{
    typedef mt19937 gen;
    gen randgen(static_cast<unsigned int>(std::time(0)));

    uniform_real_distribution<float> classes_distr(0, 1);
    uniform_real_distribution<float> dates_distr(0, timespan);

    //variate_generator<gen, uniform_real_distribution<float>> classes_gen(randgen, classes_distr);

    //boost::variate_generator<gen, uniform_real_distribution<float>> dates_gen(randgen, dates_distr);


    const size_t n = t.size();

    vector<plane_class> classes(t.size());
    for (size_t i = 0; i < t.size(); ++i)
    {
        float p = classes_distr(randgen);
        if (p < 0.2)
            classes[i] = LIGHT;
        else if (p > 0.8)
            classes[i] = HEAVY;
        else
            classes[i] = MEDIUM;
    }


    for (size_t i = 0; i < t.size(); ++i)
    {
        t[i].spans.resize(t.size());
        for (size_t j = 0; j < t.size(); ++j)
        {
            t[i].spans[j] = get_class_wait(classes[i], classes[j]);
        }
    }


    for (size_t i = 0; i < n; ++i)
    {
        t[i].due = dates_distr(randgen);
        t[i].min_bound = t[i].due;
        t[i].tweight = .2 + classes_distr(randgen) * 1.8;
    }




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

