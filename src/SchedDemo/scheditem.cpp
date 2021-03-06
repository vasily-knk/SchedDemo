#include "stdafx.h"
#include "scheditem.h"
#include "schedscene.h"

SchedItem::SchedItem(SchedScene *scene, size_t id)
    : scene_(scene)
    , id_(id)
    , width_(0)

    , is_dragged_(false)
{
    setAcceptsHoverEvents(true);
}

QRectF SchedItem::boundingRect() const
{
    return QRectF(0, 0, width_, height_);
}

void SchedItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor color(color_);
    if (scene_->selected_.is_initialized() && *(scene_->selected_) == id_)
        color = Qt::blue;

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(color));

    QRectF rect = boundingRect();
    painter->drawRect(rect);
    
    if (id_ >= scene_->perm_->size())
        return;

    QString text = QString::number((*scene_->perm_)[id_]) ;//+ ": " + QString::number(rect.width()) + ", "+ QString::number(rect.height());
    painter->drawText(rect, text);

}

void SchedItem::updateData(qreal width, qreal height)
{
    width_ = width;
    height_ = height;
    prepareGeometryChange();
}


void SchedItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //setCursor(Qt::ClosedHandCursor);
    //is_dragged_ = true;
    scene_->clickItem(id_);
    prepareGeometryChange();
}

void SchedItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!is_dragged_)
        return;

    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    mime->setText("Dragging");
    drag->setMimeData(mime);

    drag->exec();

    QMessageBox box;
    box.setText("Hello!");
    box.exec();

    is_dragged_ = false;
}

void SchedItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //setCursor(Qt::OpenHandCursor);
    //is_dragged_ = false;
}


void SchedItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    scene_->showTRect(id_);
}

void SchedItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    scene_->hideTRect(id_);
}
