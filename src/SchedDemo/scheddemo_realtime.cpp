#include "stdafx.h"
#include "scheddemo.h"
#include "qwtscheddemo.h"

void SchedDemo::playDemo()
{
    play_timer_->start(100);
}

void SchedDemo::playTick()
{
    moment_t speed = (static_cast<moment_t>(speedBar_->value()) - static_cast<moment_t>(speedBar_->minimum())) / (static_cast<moment_t>(speedBar_->maximum()) - static_cast<moment_t>(speedBar_->minimum()));
    speed *= 2;
    window_pos_ += speed;

    //setWindowTitle("Jobs removed: " + QString::number(jobs_removed_));

    updateSubtask();
    qwt_demo_->setAxisScale(QwtPlot::xBottom, window_pos_ - 1, window_pos_ + window_span_ + 10);
    updateCost();
}

void SchedDemo::pauseDemo()
{
    play_timer_->stop();
}

void SchedDemo::resetDemo()
{
}


void SchedDemo::updateSubtask()
{
    bool changed = false;
    
    perm_t::iterator new_end = std::remove_if (perm_.begin(), perm_.end(), [&](const size_t &job) -> bool
    {
        return(sched_.at(job) < window_pos_);
    });

    if (new_end != perm_.end())
    {
        perm_.resize(new_end - perm_.begin());
        changed = true;
    }

    for (; next_job_ < original_task_.size(); ++next_job_)
    {
        if (original_task_[next_job_].due > window_pos_ + window_span_)
            break;
        perm_.push_back(next_job_);
        changed = true;
    }
    
    if (changed)
    {
        sched_ = perm2sched(original_task_, perm_);
    }
}
