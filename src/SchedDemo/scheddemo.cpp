#include "stdafx.h"
#include "scheddemo.h"
#include "schedscene.h"

#include "sliding_window.h"

namespace 
{
	const size_t DEFAULT_N = 3;

}
void planes_task(float timespan, task_t &t);

perm_t order_solver(const task_t &t, const perm_t &src);
perm_t due_dates_solver(const task_t &t, const perm_t &src);
perm_t random_solver(const task_t &t, const perm_t &src, size_t n_iters);
perm_t all_pairs_solver(const task_t &t, const perm_t &src);
perm_t annealing_solver(const task_t &t, const perm_t &src);
perm_t all_triples_solver(const task_t &t, const perm_t &src, size_t n_iters);

sched_t perm2sched(const task_t &task, const perm_t &perm);

task_t gen_task1();
task_t gen_task2();

SchedDemo::SchedDemo(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
	, task_ (DEFAULT_N)
	, perm_ (DEFAULT_N)
	, sched_(DEFAULT_N)
    , cost_(0)
    , reschedule_index_(DEFAULT_N - 1)
{
	const moment_t timespan = 40;

    
    task_ = gen_task2();

    //planes_task(timespan, task_);
	//slow_perm2sched(task_, perm_, sched_);
    sched_ = perm2sched(task_, perm_);

	scene_ = new SchedScene(&task_, &perm_, &sched_);
	
	auto clb = boost::bind(&SchedDemo::updateCost, this);
	scene_->setCostCallback(clb);

    scene_->setWeightScale(100);
    scene_->setTimeScale(20);
	
	QGraphicsView *view = new QGraphicsView(scene_);
	view->setMouseTracking(true);
    
	//view->setRenderHint(QPainter::Antialiasing);



    solver_slots_.push_back(solver_slot_t("Original", order_solver));
    solver_slots_.push_back(solver_slot_t("Due dates", due_dates_solver));
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
        solver_slots_[i].btn = new QPushButton(solver_slots_[i].name);

        mapper->setMapping(solver_slots_[i].btn, i);
        connect(solver_slots_[i].btn, SIGNAL(clicked()), mapper, SLOT(map()));

        solver_slots_[i].lbl = new QLabel;
        sideLayout->addWidget(solver_slots_[i].btn, i, 0);
        sideLayout->addWidget(solver_slots_[i].lbl, i, 1);

    }

    connect(mapper, SIGNAL(mapped(int)), this, SLOT(runSolver(int)));

    
    sideLayout->addWidget(new QLabel("Cost:"), solver_slots_.size(), 0);

    QPushButton *rescheduleBtn = new QPushButton("Reschedule");
    sideLayout->addWidget(rescheduleBtn, solver_slots_.size() + 1, 0);
    connect(rescheduleBtn, SIGNAL(clicked()), this, SLOT(reschedule()));

    
    cost_display_ = new QLabel("AAA");
    sideLayout->addWidget(cost_display_, solver_slots_.size(), 1);
    sidePanel->setLayout(sideLayout);



    layout->addWidget(view, 0, 0);
    layout->addWidget(sidePanel, 0, 1);

	setLayout(layout);
    scene_->invalidateItems();
    updateCost();
}

SchedDemo::~SchedDemo()
{

}

void SchedDemo::updateCost()
{
	//slow_perm2sched(task_, perm_, sched_);
    sched_ = perm2sched(task_, perm_);

    reschedule_index_ = task_.size() - 1;
    scene_->current.reset();

	const cost_t cost = get_cost(task_, sched_);
    cost_display_->setText(QString::number(cost));
}

void SchedDemo::runSolver(int i)
{
    //solver_slots_[i].lbl->setText (QString::number(rand()));
    if (solver_slots_[i].solver == NULL)
        return;

    perm_ = solver_slots_[i].solver(task_, perm_);
    updateCost();
    const cost_t cost = get_cost(task_, sched_);
    solver_slots_[i].lbl->setText(QString::number(cost));
    scene_->invalidateItems();
}

void SchedDemo::updateOffset(size_t offset)
{
    const QString str = QString("Offset: ") + QString::number(offset);
    setWindowTitle(str);
}


inline moment_t get_processing_time(const task_t &task, const perm_t &perm, const size_t pos);
void add_job(const task_t &task, const perm_t &perm, const size_t pos, sched_t &out_sched);

void SchedDemo::reschedule()
{

    add_job(task_, perm_, reschedule_index_, sched_);
    for (int pos = reschedule_index_ - 1; pos >= 0; --pos)
        sched_[perm_[pos]] = sched_[perm_[pos + 1]] - get_processing_time(task_, perm_, pos) - 1.0;

    
    scene_->current.reset(reschedule_index_);

    if (reschedule_index_ == 0)
        reschedule_index_ = task_.size() - 1;
    else
        --reschedule_index_;

    scene_->invalidateItems();
    
}

