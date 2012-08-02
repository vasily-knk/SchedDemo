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

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(view);
	
	setLayout(layout);
}

SchedDemo::~SchedDemo()
{

}

void SchedDemo::updateCost()
{
	perm2sched(task_, perm_, sched_);
	scene_->updateItems();
	const cost_t cost = get_cost(task_, sched_);
	const QString text = QString("Cost: %1").arg(cost);
	setWindowTitle(text);
}
