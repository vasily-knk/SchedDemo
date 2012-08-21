#ifndef SLIDING_WINDOW_H
#define SLIDING_WINDOW_H

class sliding_window_solver
{
public:
    typedef boost::function<void(size_t)> callback_t;

    sliding_window_solver(size_t window_size, const callback_t &callback = NULL)
        : window_size_(window_size)
        , callback_(callback)
    {
        
    }

public:
    perm_t operator()(const task_t &t, const perm_t &src) const
    {
        perm_t dst (src);
        perm_t best (src);

        cost_t best_cost = calculate_cost(t, best);

        size_t last = dst.size() - window_size_;

        for (size_t i = 0; i <= last; ++i)
        {
            dst = best;

            auto it = dst.begin() + i;

            std::sort(it, it + window_size_);

            int counter = 0;
            do {
                ++counter;
                cost_t curr_cost = calculate_cost(t, dst);
                if (curr_cost < best_cost)
                {
                    //cout << " improvement: " << best_cost << " -> " << curr_cost << endl;
                    best = dst;
                    best_cost = curr_cost;
                }
            } while (std::next_permutation(it, it + window_size_));

            //cout << " offset " << i << " " << offset << endl;
            if (callback_ != NULL)
                callback_(i);
        }

        return best;

    }
private:
    const size_t window_size_;
    const callback_t callback_;

private:
    sliding_window_solver &operator=(const sliding_window_solver&);
};

#endif