#include "stdafx.h"
#include "task.h"
#include "schedscene.h"
#include "scheditem.h"



struct Marker : QGraphicsItem
{
    Marker() : size(10) {}

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    const int size;
};

QRectF Marker::boundingRect() const
{
    return QRectF (-size, -size, size * 2, size);
}

void Marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(Qt::black, 2));
    painter->drawLine(-size, -size, 0, 0);
    painter->drawLine(0, 0, size, -size);
}



SchedScene::SchedScene(task_t *task, perm_t *perm, sched_t *sched, QObject *parent)
    : QGraphicsScene(parent)
    , task_(task)
    , perm_(perm)
    , sched_(sched)
    , jobs_(task->size())
    , marker(new Marker)
    , tRect_(NULL)

	, lbLines_ (task->size())
	, ubLines_ (task->size())
	, dueLines_(task->size())

	, normalLine(QBrush(Qt::lightGray), 1.0)
	, thickLine(QBrush(Qt::black), 1.0)

	, weightScale_(1.0)
	, timeScale_  (1.0)

	, DATES_HEIGHT(0)
	, JOBS_HEIGHT (100)

{

    //marker->setPos(0, 0);
    //addItem(marker);

	for (size_t i = 0; i < task->size(); ++i)
	{
		jobs_[i] = new SchedItem(this, i);
		addItem(jobs_[i]);

		lbLines_ [i] = addLine(QLineF(), normalLine);
		ubLines_ [i] = NULL;
		dueLines_[i] = addLine(QLineF(), normalLine);
	}

	tRect_ = addRect(0, 0, 0, 0);
	tRect_->setPen(QPen(Qt::red, 1));
	tRect_->setBrush(QBrush(Qt::red, Qt::Dense5Pattern));

	datesTimeline_ = addLine(QLineF());
	jobsTimeline_ = addLine(QLineF());

	invalidateItems();
}

void SchedScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    /*QPointF point = mouseEvent->scenePos();


    size_t best = 0;
    for (size_t i = 0; i < sched_->n; ++i)
    {
        if (std::abs(sched_->times[i] - point.x()) < std::fabs(sched_->times[best] - point.x()))
            best = i;
    }
    point.setX(sched_->times[best]);
    point.setY(0);
    marker->setPos(point);*/

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void SchedScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Space)
    {
        invalidateItems();
    }
}

void SchedScene::clickItem(size_t id)
{
    if (!selected_.is_initialized())
    {
        selectItem(id);
    }
    else if (*selected_ == id)
    {
        deselectItem();
    }
    else
    {
        swapItems(*selected_, id);
        //deselectItem();
    }
}

void SchedScene::swapItems(size_t i, size_t j)
{
    std::swap((*perm_)[i], (*perm_)[j]);


/*
    jobs_[i]->updateData(getItemWidth(i));
    if (i > 0)
        jobs_[i-1]->updateData(getItemWidth(i-1));
    jobs_[j]->updateData(getItemWidth(j));
    if (j > 0)
        jobs_[j-1]->updateData(getItemWidth(j-1));
*/

    updateCost();
    invalidateItems();
}

void SchedScene::selectItem(size_t i)
{
    selected_.reset(i);
    invalidate(jobs_[i]->boundingRect());
    //items_[i]->updateData();
}

void SchedScene::deselectItem()
{
    if (selected_.is_initialized())
    {
        invalidate(jobs_[*selected_]->boundingRect());
        //items_[*selected_]->updateData();
    }

    selected_.reset();
}

void SchedScene::showTRect(size_t item)
{
    const size_t job = item2job(item);

    moment_t tardiness = (*sched_)[job] - (*task_)[job].due;

    if (tardiness > 0)
    {
        tRect_->setRect(time2coord((*task_)[job].due), JOBS_HEIGHT, time2coord(tardiness), /*JOBS_HEIGHT + */weight2coord((*task_)[job].tweight));
        tRect_->setVisible(true);
    }
    else
        tRect_->setVisible(false);

	//lbLines_[item]->setPen(thickLine);
	dueLines_[item]->setPen(thickLine);

    //tRect_->update();
}

void SchedScene::hideTRect(size_t item)
{
    tRect_->setVisible(false);

	//lbLines_[item]->setPen(normalLine);
	dueLines_[item]->setPen(normalLine);
}


size_t SchedScene::item2job(size_t item) const
{
    return (*perm_)[item];
}

size_t SchedScene::job2item(size_t job) const
{
    return (std::find(perm_->begin(), perm_->end(), job) - perm_->begin());
}

/*
qreal SchedScene::getItemWidth(size_t i) const
{
    if (i < task_->size() - 1)
    {
        const size_t job_i = (*perm_)[i];
        const size_t next_job = (*perm_)[i + 1];
        return time2coord((*task_)[job_i].spans[next_job]);
    }
    return 20; // FIXME!
}*/

void SchedScene::updateItems()
{
	for (size_t i = 0; i < jobs_.size(); ++i)
	{
		const size_t index = (*perm_)[i];
        jobs_[i]->setPos(time2coord((*sched_)[index]), JOBS_HEIGHT);

		//lbLines_ [i]->setLine(time2coord((*sched_)[index]), JOBS_HEIGHT, time2coord((*task_)[index].min_bound), DATES_HEIGHT);
		dueLines_[i]->setLine(time2coord((*sched_)[index]), JOBS_HEIGHT, time2coord((*task_)[index].due),       DATES_HEIGHT);
	}

	const qreal length = jobs_.back()->pos().x();

	datesTimeline_->setLine(time2coord(0), DATES_HEIGHT, length, DATES_HEIGHT);
	jobsTimeline_->setLine(time2coord(0), JOBS_HEIGHT, length, JOBS_HEIGHT);

    invalidate();
}

void SchedScene::updateCost()
{
	if (cost_clb_ != NULL)
		cost_clb_();
}

void SchedScene::invalidateItems()
{
    vector<cost_t> tards(jobs_.size());


    for (size_t i = 0; i < jobs_.size(); ++i)
    {
        const size_t job_i = (*perm_)[i];

        tards[i] = std::max<cost_t>((*sched_)[job_i] - (*task_)[job_i].due, 0) * (*task_)[job_i].tweight;
        
        qreal width = 5; // FIXME

        if (i < jobs_.size() - 1)
        {
            const size_t next_job = (*perm_)[i + 1];
            width = time2coord((*task_)[job_i].spans[next_job]);
        }
        
        jobs_[i]->updateData(width, weight2coord((*task_)[job_i].tweight));
    }
    const cost_t max_tard = *std::max_element(tards.begin(), tards.end());

    for (size_t i = 0; i < tards.size(); ++i)
    {
        if (true || tards[i] < 0 || std::fabs(max_tard) < 0.00001)
            jobs_[i]->setColor(QColor(0, 255, 255));
        else
        {
            float ratio = tards[i] / max_tard;
            int green = std::min(static_cast<int>(255.0 * 2.0 * (1.0 - ratio)), 255);
            int red = std::min(static_cast<int>(255.0 * 2.0 * ratio), 255);
            jobs_[i]->setColor(QColor(red, green, 0));
        }
    }

    if (current.is_initialized())
        jobs_[*current]->setColor(QColor(255, 0, 0));



    updateItems();
}

