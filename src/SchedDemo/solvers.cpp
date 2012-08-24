#include "stdafx.h"
#include "solvers.h"

perm_t order_solver(const task_t &t, const perm_t &/*src*/)
{
    perm_t dst(t.size());
    return dst;
}


perm_t due_dates_solver(const task_t &t, const perm_t &/*src*/)
{
    return due_dates_perm(t);
}

perm_t random_solver(const task_t &t, const perm_t &src, size_t n_iters)
{
    perm_t dst (src);
    int counter = 0;
    for (size_t iter = 0; iter < n_iters; ++iter)
    {
        size_t i = rand() % (t.size() - 1);
        size_t j = i + 1 + rand() % (t.size() - i - 1);

        cost_t before = calculate_cost(t, dst);
        std::swap(dst[i], dst[j]);
        cost_t after = calculate_cost(t, dst);

        if (after >= before)
            std::swap(dst[i], dst[j]);
        else
            ++counter;
    }
    //cout << " " << counter << "/" << n_iters << " swaps" << endl;
    return dst;
}

perm_t all_pairs_solver(const task_t &t, const perm_t &src)
{
    perm_t dst (src);
    cost_t orig_cost = calculate_cost(t, src);


    size_t best_i = 0, best_j = 0;
    cost_t best_cost = orig_cost;

    bool success = true;
    int iter;
    for (iter = 0; success; ++iter)
    {
        success = false;
        for (size_t i = 0; i < t.size()/* && !success*/; ++i)
        {
            for (size_t j = i + 1; j < t.size()/* && !success*/; ++j)
            {
                std::swap(dst[i], dst[j]);
                const cost_t after = calculate_cost(t, dst);

                if (after < best_cost)
                {
                    best_i = i;
                    best_j = j;
                    best_cost = after;
                    success = true;
                }

                std::swap(dst[i], dst[j]);
            }
        }

        if (success)
            std::swap(dst[best_i], dst[best_j]);

    } 
    return dst;        
}

perm_t annealing_solver(const task_t &t, const perm_t &src)
{
    typedef mt19937 gen;
    gen randgen(static_cast<unsigned int>(std::time(0)));

    uniform_real_distribution<float> distr(0, 1);


    perm_t dst(src);
    const float initial = 10000;
    const float frozen = 0.1f;
    const float cooling = 0.9f;
    const size_t n_iter = 100;

    for (float temperature = initial; temperature > frozen; temperature *= cooling)
    {
        for (size_t iter = 0; iter < n_iter; ++iter)
        {
            size_t i = rand() % (t.size() - 1);
            size_t j = i + 1 + rand() % (t.size() - i - 1);


            cost_t before = calculate_cost(t, dst);
            std::swap(dst[i], dst[j]);
            cost_t after = calculate_cost(t, dst);

            if (after < before)
                continue;

            // after >= before
            float p = 1.0f / (1.0f + exp((after - before) / temperature));
            float h = distr(randgen);
            if (p > h)
                continue;

            std::swap(dst[i], dst[j]);
        }
    }

    return dst;
}

perm_t all_triples_solver(const task_t &t, const perm_t &src, size_t n_iters)
{
    perm_t dst (src);


    for (size_t iter = 0; iter < n_iters; ++iter)
    {
        int counter = 0;
        for (size_t i = 0; i < t.size() - 2; ++i)
        {
            for (size_t j = i + 1; j < t.size() - 1; ++j)
            {
                for (size_t k = j + 1; k < t.size(); ++k)
                {
                    size_t triple[] = {i, j, k};
                    perm_t best(dst);
                    cost_t best_cost = calculate_cost(t, best);

                    perm_t orig(dst);
                    bool flag = false;
                    do 
                    {
                        dst[i] = orig[triple[0]];
                        dst[j] = orig[triple[1]];
                        dst[k] = orig[triple[2]];

                        cost_t curr_cost = calculate_cost(t, dst);
                        if (curr_cost < best_cost)
                        {
                            best = dst;
                            best_cost = curr_cost;
                            flag = true;
                        }
                    } while (std::next_permutation(triple, triple + 3));

                    if (flag)
                    {
                        dst = best;
                        ++counter;
                    }
                    else
                        dst = orig;

                }
            }
        }
        //cout << " iter " << iter << ": " << counter << " swaps" << endl;
    }
    return dst;        
}


sliding_window_solver::sliding_window_solver(size_t window_size, const callback_t &callback/* = NULL*/)
    : window_size_(window_size)
    , callback_(callback)
{

}

perm_t sliding_window_solver::operator()(const task_t &t, const perm_t &src) const
{
    if (src.size() < window_size_)
        return src;
    
    perm_t perm = src;

    const int last = src.size() - window_size_;

    for (int i = 0; i <= last; ++i)
        reorder_window(t, perm, i);

    for (int i = last; i >= 0; --i)
        reorder_window(t, perm, i);

    return perm;
}

void sliding_window_solver::reorder_window(const task_t &t, perm_t &perm, const size_t pos) const
{
    const perm_t::iterator begin = perm.begin() + pos;
    const perm_t::iterator end = perm.begin() + pos + window_size_;

    perm_t best = perm;
    cost_t best_cost = calculate_cost(t, best);

    std::sort(begin, end);
    do {
        cost_t curr_cost = calculate_cost(t, perm);
        if (curr_cost < best_cost)
        {
            best = perm;
            best_cost = curr_cost;
        }
    } while (std::next_permutation(begin, end));

    perm = best;
    
    if (callback_ != NULL)
        callback_(pos);
}
