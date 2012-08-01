#ifndef SCHEDITEM_H
#define SCHEDITEM_H

#include <QGraphicsItem>

class SchedScene;

class SchedItem : public QGraphicsItem
{
public:
    SchedItem(SchedScene *scene, size_t id);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void updateData(qreal width);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    SchedScene *scene_;
    size_t id_;

    qreal width_;

    bool is_dragged_;
};

#endif // SCHEDITEM_H
