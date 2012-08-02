#include "stdafx.h"
#include "scheddemo.h"
#include "schedscene.h"

namespace 
{
	const size_t DEFAULT_N = 50;

}
void planes_task(float timespan, task_t &t);

perm_t due_dates_solver(const task_t &t, const perm_t &src);
perm_t random_solver(const task_t &t, const perm_t &src, size_t n_iters);

SchedDemo::SchedDemo(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
	, task_ (DEFAULT_N)
	, perm_ (DEFAULT_N)
	, sched_(DEFAULT_N)
    , cost_(0)
{
	const moment_t timespan = 60;

	planes_task(timespan, task_);
	perm2sched(task_, perm_, sched_);

	scene_ = new SchedScene(&task_, &perm_, &sched_);
	
	auto clb = boost::bind(&SchedDemo::updateCost, this);
	scene_->setCostCallback(clb);

    scene_->setWeightScale(100);
    scene_->setTimeScale(20);
	
	QGraphicsView *view = new QGraphicsView(scene_);
	view->setMouseTracking(true);
	//view->setRenderHint(QPainter::Antialiasing);



    solver_slots_.push_back(solver_slot_t("Due dates", due_dates_solver));
    solver_slots_.push_back(solver_slot_t("Random pairs", boost::bind(random_solver, _1, _2, 10000)));
    solver_slots_.push_back(solver_slot_t("Best pair", NULL));

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
	perm2sched(task_, perm_, sched_);
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

