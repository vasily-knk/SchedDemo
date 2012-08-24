#include "stdafx.h"
#include "scheddemo.h"
#include "schedscene.h"
#include "qwtscheddemo.h"

#include "sliding_window.h"

void planes_task(float timespan, task_t &t);
task_t planes_task_with_bounds(const size_t num_planes, const moment_t timespan, const moment_t bound_timespan);

perm_t order_solver(const task_t &t, const perm_t &src);
perm_t due_dates_solver(const task_t &t, const perm_t &src);
perm_t random_solver(const task_t &t, const perm_t &src, size_t n_iters);
perm_t all_pairs_solver(const task_t &t, const perm_t &src);
perm_t annealing_solver(const task_t &t, const perm_t &src);
perm_t all_triples_solver(const task_t &t, const perm_t &src, size_t n_iters);

sched_t perm2sched(const task_t &task, const perm_t &perm);

task_t gen_task1();
task_t gen_task2();
task_t gen_task3();

SchedDemo::SchedDemo(const size_t num_planes, const moment_t timespan, const moment_t bounds, const moment_t window_span, QWidget *parent/* = 0*/, Qt::WFlags flags/* = 0*/)
	: QWidget(parent, flags)
    , window_pos_(0)
    , window_span_(window_span)
    , subtask_begin_(0)
    , subtask_end_(0)
{
    task_ = planes_task_with_bounds(num_planes, timespan, bounds);    
    original_perm_.resize(num_planes);
    std::generate(original_perm_.begin(), original_perm_.end(), perm_generator());

    due_dates_perm_ = due_dates_perm(task_);

    perm_ = original_perm_;
    sched_ = perm2sched(task_, perm_);

	scene_ = new SchedScene(&task_, &perm_, &sched_);
	
	auto clb = boost::bind(&SchedDemo::updateCost, this);
	scene_->setCostCallback(clb);

    scene_->setWeightScale(100);
    scene_->setTimeScale(20);
	
	QGraphicsView *view = new QGraphicsView(scene_);
	view->setMouseTracking(true);
    
    solver_slots_.push_back(solver_slot_t("Original", order_solver));
    solver_slots_.push_back(solver_slot_t("Due dates", due_dates_solver));
    selected_solver_ = solver_slots_.size() - 1;
    solver_slots_.push_back(solver_slot_t("Random pair", boost::bind(random_solver, _1, _2, 10000)));
    solver_slots_.push_back(solver_slot_t("Best pair", all_pairs_solver));
    solver_slots_.push_back(solver_slot_t("Best triple", boost::bind(all_triples_solver, _1, _2, 7)));
    solver_slots_.push_back(solver_slot_t("Annealing", annealing_solver));

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
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(runSolver(int)));
    
    sideLayout->addWidget(new QLabel("Cost:"), solver_slots_.size(), 0);

    /*QPushButton *advanceBtn = new QPushButton("Advance");
    sideLayout->addWidget(advanceBtn, solver_slots_.size() + 1, 0);
    connect(advanceBtn, SIGNAL(clicked()), this, SLOT(advanceSubtask()));

    QPushButton *resetBtn = new QPushButton("Reset");
    sideLayout->addWidget(resetBtn, solver_slots_.size() + 2, 0);
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetSubtask()));*/

    QPushButton *playBtn = new QPushButton("Play", this);
    sideLayout->addWidget(playBtn, solver_slots_.size() + 1, 0);
    connect(playBtn, SIGNAL(clicked()), this, SLOT(playDemo()));

    QPushButton *pauseBtn = new QPushButton("Pause", this);
    sideLayout->addWidget(pauseBtn, solver_slots_.size() + 1, 1);
    connect(pauseBtn, SIGNAL(clicked()), this, SLOT(pauseDemo()));

    QPushButton *resetBtn = new QPushButton("Reset", this);
    sideLayout->addWidget(resetBtn, solver_slots_.size() + 2, 0);
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetDemo()));

    speedBar_ = new QScrollBar(Qt::Horizontal, this);
    sideLayout->addWidget(speedBar_, solver_slots_.size() + 3, 0, 1, 2);

    play_timer_ = new QTimer(this);
    connect(play_timer_, SIGNAL(timeout()), this, SLOT(playTick()));
    
    cost_display_ = new QLabel("AAA", this);
    sideLayout->addWidget(cost_display_, solver_slots_.size(), 1);
    sidePanel->setLayout(sideLayout);

    layout->addWidget(view, 0, 0);

    qwt_demo_ = new QwtSchedDemo(this);
    layout->addWidget(qwt_demo_, 1, 0);

    layout->addWidget(sidePanel, 0, 1, 2, 1);

	setLayout(layout);
    scene_->invalidateItems();
    updateCost();
}

SchedDemo::~SchedDemo()
{

}

void SchedDemo::updateCost()
{
    //sched_ = perm2sched(task_, perm_);

    reschedule_index_ = task_.size() - 1;
    scene_->current.reset();

	const cost_t cost = get_cost_partial(task_, sched_, perm_, 0, subtask_end_);
    cost_display_->setText(QString::number(cost));

    qwt_demo_->updateData(task_, perm_, sched_, subtask_begin_, subtask_end_);
}

void SchedDemo::runSolver(const int solver_index)
{
    //solver_slots_[i].lbl->setText (QString::number(rand()));
    if (solver_slots_[solver_index].solver == NULL)
        return;

    selected_solver_ = solver_index;

    perm_t mappings(subtask_end_ - subtask_begin_);
    
    if (mappings.empty())
        return;

    std::copy(perm_.begin() + subtask_begin_, perm_.begin() + subtask_end_, mappings.begin());

    task_t subtask = apply_permutation(task_, mappings);

    if (subtask_begin_ > 0)
    {
        const moment_t total_min_bound = sched_[perm_[subtask_begin_ - 1]] + get_processing_time(task_, perm_, subtask_begin_ - 1);
        for (task_t::iterator it = subtask.begin(); it != subtask.end(); ++it)
        {
            it->min_bound = std::max(it->min_bound, total_min_bound);
            it->due = std::max(it->due, it->min_bound);
        }
    }


    perm_t subperm(mappings.size());
    std::generate(subperm.begin(), subperm.end(), perm_generator());

    subperm = solver_slots_[solver_index].solver(subtask, subperm);
    const sched_t subsched = perm2sched(subtask, subperm);
    
    for (size_t i = subtask_begin_; i < subtask_end_; ++i)
    {
        perm_[i] = mappings[subperm[i - subtask_begin_]];
        sched_[perm_[i]] = subsched[subperm[i - subtask_begin_]];
    }

    //perm_ = solver_slots_[i].solver(task_, perm_);
    updateCost();
    const cost_t cost = get_cost_partial(task_, sched_, perm_, 0, subtask_end_);
    solver_slots_[solver_index].lbl->setText(QString::number(cost));
    scene_->invalidateItems();
}

void SchedDemo::updateOffset(size_t offset)
{
    const QString str = QString("Offset: ") + QString::number(offset);
    setWindowTitle(str);
}

void SchedDemo::reschedule()
{

    /*add_job(task_, perm_, reschedule_index_, sched_);
    for (int pos = reschedule_index_ - 1; pos >= 0; --pos)
        sched_[perm_[pos]] = sched_[perm_[pos + 1]] - get_processing_time(task_, perm_, pos) - 1.0;

    
    scene_->current.reset(reschedule_index_);

    if (reschedule_index_ == 0)
        reschedule_index_ = task_.size() - 1;
    else
        --reschedule_index_;

    scene_->invalidateItems();*/
    
}

void SchedDemo::advanceSubtask()
{
    subtask_begin_ = std::min(task_.size(), subtask_begin_ + 1);
    subtask_end_ = std::min(task_.size(), subtask_end_ + 1);
    runSolver(selected_solver_);
    updateCost();
}

void SchedDemo::resetSubtask()
{
    perm_ = original_perm_;
    sched_ = perm2sched(task_, perm_);
    subtask_begin_ = 0;
    subtask_end_ = std::min<size_t>(task_.size(), 50);
    runSolver(selected_solver_);
    updateCost();
}

void SchedDemo::updateSubtask()
{
    size_t pos = 0;
    for (; pos < task_.size(); ++pos)
        if (sched_[due_dates_perm_[pos]] > window_pos_)
            break;
    subtask_begin_ = pos;

    for (; pos < task_.size(); ++pos)
        if (task_[due_dates_perm_[pos]].due > window_pos_ + window_span_)
            break;
    subtask_end_ = pos;

    qwt_demo_->setAxisScale(QwtPlot::xBottom, window_pos_ - 1, window_pos_ + window_span_ + 10);
    scene_->setSubTask(subtask_begin_, subtask_end_);

    runSolver(selected_solver_);
    updateCost();
}

void SchedDemo::playDemo()
{
    play_timer_->start(100);
}

void SchedDemo::playTick()
{
    moment_t speed = (static_cast<moment_t>(speedBar_->value()) - static_cast<moment_t>(speedBar_->minimum())) / (static_cast<moment_t>(speedBar_->maximum()) - static_cast<moment_t>(speedBar_->minimum()));
    speed *= 2;
    window_pos_ += speed;
    
    setWindowTitle(QString::number(window_pos_));
    updateSubtask();
}

void SchedDemo::pauseDemo()
{
    play_timer_->stop();
}

void SchedDemo::resetDemo()
{
    window_pos_ = 0;
    play_timer_->stop();
    perm_ = due_dates_perm_;
    updateSubtask();
}

