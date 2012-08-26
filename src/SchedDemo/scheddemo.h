#ifndef SCHEDDEMO_H
#define SCHEDDEMO_H

class SchedScene;
class QwtSchedDemo;

class SchedDemo : public QWidget
{
	Q_OBJECT

public:
	SchedDemo(size_t num_planes, moment_t timespan, moment_t bounds_span, moment_t window_span, QWidget *parent = NULL, Qt::WFlags flags = 0);
	~SchedDemo();

	void updateCost();
    void updateOffset(size_t offset);
    void updateSubtask();

    void deleteJob(size_t pos);

    void updateTime();

protected slots:
    void runSolver(int i);
    /*void reschedule();
    void advanceSubtask();
    void resetSubtask();
    void playDemo();
    void pauseDemo();
    void resetDemo();
    void playTick();*/
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
    const task_t original_task_;
    const perm_t original_perm_;

    size_t jobs_removed_;

    task_t task_;
	perm_t perm_;
	sched_t sched_;
    //perm_t original_perm_, due_dates_perm_;

    task_t deleted_jobs_;

    moment_t window_pos_, window_span_;
private:

    QTimer *play_timer_;
    QScrollBar *speedBar_;
    
    cost_t cost_;

	SchedScene *scene_;
    vector<solver_slot_t> solver_slots_;
    size_t selected_solver_;
    QLabel *cost_display_;

    QwtSchedDemo *qwt_demo_;


};

#endif // SCHEDDEMO_H
