#ifndef SCHEDSCENE_H
#define SCHEDSCENE_H


class QGraphicsItem;
class SchedItem;

class SchedScene : public QGraphicsScene
{
    Q_OBJECT

    friend class SchedItem;
public:
    SchedScene(task_t *task, perm_t *perm, sched_t *sched, QObject *parent = 0);
    
/*signals:
    
public slots:*/

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyPressEvent(QKeyEvent *keyEvent);


public:
    const task_t* getTask() const {return task_;}
    const sched_t* getSched() const {return sched_;}
    void clickItem(size_t id);
    void swapItems(size_t i, size_t j);

    void selectItem(size_t i);
    void deselectItem();

    void showTRect(size_t item);
    void hideTRect();

    size_t item2job(size_t item) const;
    size_t job2item(size_t job) const;
    qreal getItemWidth(size_t item) const;

private:
    void updateItems();
    void updateCost();
private:
    task_t *task_;
    perm_t *perm_;
    sched_t *sched_;

    vector<SchedItem*> items_;
    optional<size_t> selected_;
    QGraphicsItem *marker;

    QGraphicsRectItem *tRect_;
};

#endif // SCHEDSCENE_H
