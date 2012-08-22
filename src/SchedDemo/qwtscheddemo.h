#ifndef QWT_DEMO_H
#define QWT_DEMO_H


class QwtSchedDemo : public QwtPlot
{
    Q_OBJECT

public:
    QwtSchedDemo(QWidget *parent);
    ~QwtSchedDemo();

    void updateData(const task_t &task, const perm_t &perm, const sched_t &sched);

private:
    void initGrid();
    void initData();

private:
    QwtPlotHistogram *hist_planes_, *hist_min_bounds_, *hist_max_bounds_;
    
};

#endif // QWT_DEMO_H
