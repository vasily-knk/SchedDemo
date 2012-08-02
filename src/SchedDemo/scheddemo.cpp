#include "stdafx.h"
#include "scheddemo.h"
#include "schedscene.h"

namespace 
{
	const size_t DEFAULT_N = 10;

}
void planes_task(float timespan, task_t &t);


SchedDemo::SchedDemo(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
	, task_ (DEFAULT_N)
	, perm_ (DEFAULT_N)
	, sched_(DEFAULT_N)
    , cost_(0)
{
	const moment_t timespan = 300;

	planes_task(timespan, task_);
	perm2sched(task_, perm_, sched_);

	scene_ = new SchedScene(&task_, &perm_, &sched_);
	
	auto clb = boost::bind(&SchedDemo::updateCost, this);
	scene_->setCostCallback(clb);
	
	QGraphicsView *view = new QGraphicsView(scene_);
	view->setMouseTracking(true);
	//view->setRenderHint(QPainter::Antialiasing);



    solver_slots_.push_back(solver_slot_t("Current"));
    solver_slots_.push_back(solver_slot_t("Random"));
    solver_slots_.push_back(solver_slot_t("Pairs"));

    QGridLayout *layout = new QGridLayout;

    QWidget *sidePanel = new QWidget;
    QGridLayout *sideLayout = new QGridLayout;
    sideLayout->setAlignment(Qt::AlignTop);

    QSignalMapper *mapper = new QSignalMapper(this);

    for (size_t i = 0; i < solver_slots_.size(); ++i)
    {
        solver_slots_[i].btn = new QPushButton(solver_slots_[i].name);

        //connect(solver_slots_[i].btn, SIGNAL(clicked()), this, SLOT())
        mapper->setMapping(solver_slots_[i].btn, i);
        connect(solver_slots_[i].btn, SIGNAL(clicked()), mapper, SLOT(map()));

        solver_slots_[i].lbl = new QLabel("0");
        sideLayout->addWidget(solver_slots_[i].btn, i, 0);
        sideLayout->addWidget(solver_slots_[i].lbl, i, 1);

    }

    connect(mapper, SIGNAL(mapped(int)), this, SLOT(runSolver(int)));

    sideLayout->addWidget(new QPushButton("All tests"), solver_slots_.size(), 0, 1, 2);
    sidePanel->setLayout(sideLayout);

    layout->addWidget(view, 0, 0);
    layout->addWidget(sidePanel, 0, 1);

	setLayout(layout);
}

SchedDemo::~SchedDemo()
{

}

void SchedDemo::updateCost()
{
	perm2sched(task_, perm_, sched_);
	const cost_t cost = get_cost(task_, sched_);
	const QString text = QString("Cost: %1").arg(cost);
	setWindowTitle(text);
}

void SchedDemo::runSolver(int i)
{
    solver_slots_[i].lbl->setText (QString::number(rand()));
}

void SchedDemo::runSolver1()
{
    solver_slots_[0].lbl->setText (QString::number(rand()));
}
