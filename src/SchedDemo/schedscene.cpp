#include "stdafx.h"
#include "task.h"
#include "schedscene.h"
#include "scheditem.h"

perm_t random_solver(const task_t &t, const perm_t &src, size_t n_iters);


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
    , items_(task->size())
    , marker(new Marker)
    , tRect_(NULL)

{
    for (size_t i = 0; i < task->size(); ++i)
    {
        items_[i] = new SchedItem(this, i);
        items_[i]->setPos(sched->operator[](i), 0);

        addItem(items_[i]);
        items_[i]->updateData(getItemWidth(i));
    }

    marker->setPos(0, 0);
    addItem(marker);

    tRect_ = addRect(0, 0, 0, 0);
    tRect_->setPen(QPen(Qt::red, 1));
    tRect_->setBrush(QBrush(Qt::red, Qt::Dense5Pattern));


    updateItems();
    updateCost();
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
        *perm_ = random_solver(*task_, *perm_, 10000);
        for (size_t i = 0; i < items_.size(); ++i)
        {
            items_[i]->updateData(getItemWidth(i));
        }
        updateItems();
        updateCost();
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
        deselectItem();
    }
}

void SchedScene::swapItems(size_t i, size_t j)
{
    std::swap((*perm_)[i], (*perm_)[j]);


    items_[i]->updateData(getItemWidth(i));
    if (i > 0)
        items_[i-1]->updateData(getItemWidth(i-1));
    items_[j]->updateData(getItemWidth(j));
    if (j > 0)
        items_[j-1]->updateData(getItemWidth(j-1));

    updateItems();
    updateCost();
}

void SchedScene::selectItem(size_t i)
{
    selected_.reset(i);
    invalidate(items_[i]->boundingRect());
    //items_[i]->updateData();
}

void SchedScene::deselectItem()
{
    if (selected_.is_initialized())
    {
        invalidate(items_[*selected_]->boundingRect());
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
        tRect_->setRect((*task_)[job].due, 0, tardiness, (*task_)[job].tweight);
        tRect_->setVisible(true);
    }
    else
        tRect_->setVisible(false);

    //tRect_->update();
}

void SchedScene::hideTRect()
{
    tRect_->setVisible(false);
    //tRect_->update();
}


size_t SchedScene::item2job(size_t item) const
{
    return (*perm_)[item];
}

size_t SchedScene::job2item(size_t job) const
{
    return (std::find(perm_->begin(), perm_->end(), job) - perm_->begin());
}

qreal SchedScene::getItemWidth(size_t i) const
{
    if (i < task_->size() - 1)
    {
        const size_t job_i = (*perm_)[i];
        const size_t next_job = (*perm_)[i + 1];
        return (*task_)[job_i].spans[next_job];
    }
    return 20; // FIXME!
}

void SchedScene::updateItems()
{
    perm2sched(*task_, *perm_, *sched_);

    for (size_t i = 0; i < items_.size(); ++i)
        items_[i]->setPos((*sched_)[(*perm_)[i]], 0);

    invalidate();
}

void SchedScene::updateCost()
{
    qDebug() << "Cost: " << get_cost(*task_, *sched_);
}

