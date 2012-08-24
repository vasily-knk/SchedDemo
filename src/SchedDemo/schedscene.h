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

    void invalidateItems();

    void selectItem(size_t i);
    void deselectItem();

    void showTRect(size_t item);
    void hideTRect(size_t item);

    size_t item2job(size_t item) const;
    size_t job2item(size_t job) const;
//     qreal getItemWidth(size_t item) const;

	
	typedef boost::function<void()> cost_clb_t;
	void setCostCallback(const cost_clb_t &clb)
	{
		cost_clb_ = clb;
	}

	void updateItems();

    qreal getWeightScale() const 
    { 
        return weightScale_; 
    }
    
    void setWeightScale(qreal val) 
    { 
        weightScale_ = val; 
    }
    
    qreal getTimeScale() const 
    { 
        return timeScale_; 
    }
    void setTimeScale(qreal val) 
    { 
        timeScale_ = val; 
    }

    void setSubTask(const size_t begin, const size_t end)
    {
        subtask_begin_ = begin;
        subtask_end_ = end;
    }
private:
    void updateCost();

	qreal weight2coord(cost_t weight) const
	{
		return weight * weightScale_;
	}

	qreal time2coord(moment_t time) const
	{
		return time * timeScale_;
	}


private:
    task_t const *task_;
    perm_t *perm_;
    sched_t const *sched_;

    optional<size_t> selected_;
    QGraphicsItem *marker;

    QGraphicsRectItem *tRect_;

	QGraphicsLineItem *datesTimeline_, *jobsTimeline_;
	vector<QGraphicsLineItem*> lbLines_, ubLines_, dueLines_;
	vector<SchedItem*> jobs_;

	const QPen normalLine, thickLine;

	qreal weightScale_, timeScale_;

    const qreal DATES_HEIGHT, JOBS_HEIGHT;

	cost_clb_t cost_clb_;

    size_t subtask_begin_, subtask_end_;

public:
    boost::optional<size_t> current;
};

#endif // SCHEDSCENE_H
