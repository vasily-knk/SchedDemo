#ifndef QWT_DEMO_2_H
#define QWT_DEMO_2_H

class qwt_demo_2 : public QwtPlot
{
    Q_OBJECT

public:
    qwt_demo_2(QWidget *parent);
    ~qwt_demo_2();
public:
    void updateData(const task_t &task, const perm_t &perm, const sched_t &sched);
private:
    class Histogram;
    void initGrid();
    void updateSpecificData(const task_t &task, const perm_t &perm, const sched_t &sched, aircraft_t::aircraft_priority priority, Histogram *lb, Histogram *ub, Histogram *aircrafts);

    Histogram *red_lower_bounds, *red_upper_bounds, *red_aircrafts;
    Histogram *yellow_lower_bounds, *yellow_upper_bounds, *yellow_aircrafts;
    Histogram *green_lower_bounds, *green_upper_bounds, *green_aircrafts;
    vector<QwtPlotMarker*> names;
    
};

#endif // QWT_DEMO_2_H
