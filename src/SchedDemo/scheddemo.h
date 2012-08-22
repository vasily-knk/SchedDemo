#ifndef SCHEDDEMO_H
#define SCHEDDEMO_H

class SchedScene;
class QwtSchedDemo;

class SchedDemo : public QWidget
{
	Q_OBJECT

public:
	SchedDemo(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SchedDemo();

	void updateCost();
    void updateOffset(size_t offset);

protected slots:
    void runSolver(int i);
    void reschedule();
private:
    struct solver_slot_t
    {
        solver_slot_t(const char* name, const solver_t &solver)
            : name(name)
            , solver(solver)
            , btn(NULL)
            , lbl(NULL)
        {

        }
        QString name;

        solver_t solver;
        QPushButton *btn;
        QLabel *lbl;
    };
private:
    
    task_t task_;
	perm_t perm_;
	sched_t sched_;

    size_t subtask_begin_, subtask_end_;
    
    cost_t cost_;

	SchedScene *scene_;
    vector<solver_slot_t> solver_slots_;
    QLabel *cost_display_;

    QwtSchedDemo *qwt_demo_;

    size_t reschedule_index_;

};

#endif // SCHEDDEMO_H
