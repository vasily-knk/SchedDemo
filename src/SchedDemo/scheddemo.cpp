#include "stdafx.h"
#include "scheddemo.h"
#include "qwtscheddemo.h"
#include "solvers.h"
#include "sliding_window.h"
#include "qwt_demo_2.h"


void planes_task(float timespan, task_t &t);
task_t planes_task_with_bounds(const size_t num_planes, const moment_t timespan, const moment_t bound_timespan);
task_t gen_task3();

SchedDemo::SchedDemo(const size_t num_planes, const moment_t timespan, const moment_t bounds, const moment_t window_span, QWidget *parent/* = 0*/, Qt::WFlags flags/* = 0*/)
	: QWidget(parent, flags)
    , jobs_removed_(0)
    , original_task_(planes_task_with_bounds(num_planes, timespan, bounds))
    //, original_perm_(num_planes)
    , window_pos_(-2)
    , window_span_(window_span)
    , next_job_(0)
    , total_cost(0)
{
    resetDemo();

    sched_ = perm2sched(task_, perm_);
    sched_src_ = perm2sched(task_src_, perm_);


	
	auto clb = boost::bind(&SchedDemo::updateCost, this);
	//scene_->setCostCallback(clb);

    //scene_->setWeightScale(100);
    //scene_->setTimeScale(20);
	
	//QGraphicsView *view = new QGraphicsView(scene_);
	//view->setMouseTracking(true);
    
    //solver_slots_.push_back(solver_slot_t("Original", order_solver));
    solver_slots_.push_back(solver_slot_t("Due dates", due_dates_solver));
    selected_solver_ = solver_slots_.size() - 1;
    solver_slots_.push_back(solver_slot_t("Random pair", boost::bind(random_solver, _1, _2, 10000)));
    solver_slots_.push_back(solver_slot_t("Best pair", all_pairs_solver));
    //solver_slots_.push_back(solver_slot_t("Best triple", boost::bind(all_triples_solver, _1, _2, 7)));
    //solver_slots_.push_back(solver_slot_t("Annealing", annealing_solver));

    sliding_window_solver sws(5, boost::bind(&SchedDemo::updateOffset, this, _1));
    solver_slots_.push_back(solver_slot_t("Window", sws));

    QGridLayout *layout = new QGridLayout;

    QWidget *sidePanel = new QWidget;
    QGridLayout *sideLayout = new QGridLayout;
    sideLayout->setAlignment(Qt::AlignTop);

    QSignalMapper *mapper = new QSignalMapper(this);

    for (size_t i = 0; i < solver_slots_.size(); ++i)
    {
        solver_slots_[i].btn = new QPushButton(solver_slots_[i].name, this);

        mapper->setMapping(solver_slots_[i].btn, i);
        connect(solver_slots_[i].btn, SIGNAL(clicked()), mapper, SLOT(map()));

        solver_slots_[i].lbl = new QLabel(this);
        sideLayout->addWidget(solver_slots_[i].btn, i, 0);
        sideLayout->addWidget(solver_slots_[i].lbl, i, 1);

    }
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(setSolver(int)));
    

/*
    QPushButton *advanceBtn = new QPushButton("Advance");
    sideLayout->addWidget(advanceBtn, solver_slots_.size() + 1, 0);
    connect(advanceBtn, SIGNAL(clicked()), this, SLOT(advanceSubtask()));*/

    /*QPushButton *resetBtn = new QPushButton("Reset");
    sideLayout->addWidget(resetBtn, solver_slots_.size() + 2, 0);
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetSubtask()));*/

    QPushButton *playBtn = new QPushButton("Play", this);
    sideLayout->addWidget(playBtn, solver_slots_.size() + 1, 0);
    connect(playBtn, SIGNAL(clicked()), this, SLOT(playDemo()));

    QPushButton *pauseBtn = new QPushButton("Pause", this);
    sideLayout->addWidget(pauseBtn, solver_slots_.size() + 1, 1);
    connect(pauseBtn, SIGNAL(clicked()), this, SLOT(pauseDemo()));

    speedBar_ = new QScrollBar(Qt::Horizontal, this);
    sideLayout->addWidget(speedBar_, solver_slots_.size() + 3, 0, 1, 2);

    play_timer_ = new QTimer(this);
    connect(play_timer_, SIGNAL(timeout()), this, SLOT(playTick()));

    QPushButton *resetBtn = new QPushButton("Reset", this);
    sideLayout->addWidget(resetBtn, solver_slots_.size() + 2, 0);
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetDemo()));

    size_t pos_offset = solver_slots_.size() + 5;
    sideLayout->addWidget(new QLabel("Cost:"), pos_offset, 0);
    cost_display_ = new QLabel(this);
    sideLayout->addWidget(cost_display_, pos_offset, 1);
    ++pos_offset;

    sideLayout->addWidget(new QLabel("Aircrafts Processed:"), pos_offset, 0);
    processed_display_ = new QLabel(this);
    sideLayout->addWidget(processed_display_, pos_offset, 1);
    ++pos_offset;

    sideLayout->addWidget(new QLabel("Aircrafts Removed:"), pos_offset, 0);
    removed_display_ = new QLabel(this);
    sideLayout->addWidget(removed_display_, pos_offset, 1);
    ++pos_offset;

    sideLayout->addWidget(new QLabel("Aircrafts in progress:"), pos_offset, 0);
    in_progress_display_ = new QLabel(this);
    sideLayout->addWidget(in_progress_display_, pos_offset, 1);
    ++pos_offset;

    sidePanel->setLayout(sideLayout);

    //layout->addWidget(view, 0, 0);

/*
    qwt_demo_ = new QwtSchedDemo(this);
    layout->addWidget(qwt_demo_, 1, 0);
*/


    demo2_src = new qwt_demo_2(this);
    demo2_src->setAxisScale(QwtPlot::xBottom, -30, 30);
    demo2 = new qwt_demo_2(this);
    demo2->setAxisScale(QwtPlot::xBottom, -30, 30);

    layout->addWidget(demo2_src, 0, 0);
    layout->addWidget(demo2, 0, 1);
    layout->addWidget(sidePanel, 0, 3, 2, 1);

	setLayout(layout);
    //scene_->invalidateItems();
    updateCost();

}

SchedDemo::~SchedDemo()
{

}

void SchedDemo::updateCost()
{
	const cost_t cost = get_cost(task_, sched_);
    cost_display_->setText(QString::number(total_cost));
    processed_display_->setText(QString::number(jobs_processed_));
    removed_display_->setText(QString::number(jobs_removed_));
    in_progress_display_->setText(QString::number(perm_.size()));

    demo2->updateData(original_task_, perm_, sched_);
    demo2_src->updateData(original_task_, perm_src_, sched_src_);
}

/*
perm_t shrink_perm(const task_t &, const perm_t &perm, const size_t removed_item)
{
    perm_t dst;
    for (auto it = perm.begin(); it != perm.end(); ++it)
    {
        if (*it < removed_item)
            dst.push_back(*it);
        else if (*it > removed_item)
            dst.push_back(*it - 1);
    }
    return dst;
}*/


void SchedDemo::runSolver(task_t &task, perm_t &perm, sched_t &sched)
{
    BOOST_FOREACH(size_t job, perm)
    {
        task[job] = original_task_[job];
        if (selected_solver_ == 0)
            task[job].min_bound = task[job].due;
        else
        {
            
        }
        task[job].min_bound = std::max(task[job].min_bound, total_min_bound_);
        task[job].due = std::max(task[job].due, task[job].min_bound);
    }
    
    if (solver_slots_[selected_solver_].solver == NULL)
        return;

    perm = solver_slots_[selected_solver_].solver(task, perm);
    size_t job_to_remove;

    while ((job_to_remove = get_unfeasible_pos(task, perm)) != perm.size())
    {
        perm.erase(perm.begin() + job_to_remove);
        ++jobs_removed_;
        perm = solver_slots_[selected_solver_].solver(task, perm);
    }

    sched = perm2sched(task, perm);
    
    updateCost();
    const cost_t cost = get_cost(task, sched);
    solver_slots_[selected_solver_].lbl->setText(QString::number(cost));
}

void SchedDemo::setSolver(const int solver_index)
{
    selected_solver_ = solver_index;

}


void SchedDemo::updateOffset(size_t offset)
{
    const QString str = QString("Offset: ") + QString::number(offset);
}

void SchedDemo::updateTime()
{
}

