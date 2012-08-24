#pragma 

perm_t order_solver(const task_t &t, const perm_t &src);
perm_t due_dates_solver(const task_t &t, const perm_t &src);
perm_t random_solver(const task_t &t, const perm_t &src, size_t n_iters);
perm_t all_pairs_solver(const task_t &t, const perm_t &src);
perm_t annealing_solver(const task_t &t, const perm_t &src);
perm_t all_triples_solver(const task_t &t, const perm_t &src, size_t n_iters);


class sliding_window_solver
{
public:
    typedef boost::function<void(size_t)> callback_t;

    sliding_window_solver(size_t window_size, const callback_t &callback = NULL);
public:
    perm_t operator()(const task_t &t, const perm_t &src) const;

private:
    void reorder_window(const task_t &t, perm_t &perm, const size_t pos) const;

private:
    const size_t window_size_;
    const callback_t callback_;
    cost_t best_cost_;
private:
    sliding_window_solver &operator=(const sliding_window_solver&);
};
