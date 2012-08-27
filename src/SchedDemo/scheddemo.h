#ifndef SCHEDDEMO_H
#define SCHEDDEMO_H

class SchedScene;
class QwtSchedDemo;
class qwt_demo_2;

class SchedDemo : public QWidget
{
	Q_OBJECT

public:
	SchedDemo(size_t num_planes, moment_t timespan, moment_t bounds_span, moment_t window_span, QWidget *parent = NULL, Qt::WFlags flags = 0);
	~SchedDemo();

	void updateCost();
    void updateOffset(size_t offset);
    void updateSubtask(task_t &task, perm_t &perm, sched_t &sched, size_t &next_job, size_t slv);

    void deleteJob(size_t pos);

    void updateTime();

private:
    void runSolver(task_t &task, perm_t &perm, sched_t &sched);
protected slots:
    void setSolver(int i);
    //void advanceSubtask();
     void playTick();
    void playDemo();
    void pauseDemo();
    void resetDemo();
    /*void reschedule();
    void advanceSubtask();
    void resetSubtask();
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
    task_t task_, task_src_;
    perm_t perm_, perm_src_;
    sched_t sched_, sched_src_;
    bool fake_task_selected_;
    //const perm_t original_perm_;

    size_t jobs_processed_, jobs_removed_;

    task_t deleted_jobs_;

    moment_t window_pos_, window_span_;
    size_t next_job_, next_job_src_;
    cost_t total_cost;
    moment_t total_min_bound_;
private:

    QTimer *play_timer_;
    QScrollBar *speedBar_;
    
    cost_t cost_;

	//SchedScene *scene_;
    vector<solver_slot_t> solver_slots_;
    size_t selected_solver_;
    QLabel *cost_display_, *processed_display_, *removed_display_, *in_progress_display_;

    //QwtSchedDemo *qwt_demo_;
    qwt_demo_2 *demo2;
    qwt_demo_2 *demo2_src;


};

#endif // SCHEDDEMO_H
