#include "stdafx.h"
#include "scheddemo.h"
#include "qwtscheddemo.h"
#include "qwt_demo_2.h"

void SchedDemo::playDemo()
{
    play_timer_->start(30);
}

void SchedDemo::playTick()
{
    if (jobs_processed_ == task_.size())
    {
        return;
    }
    
    moment_t speed = (static_cast<moment_t>(speedBar_->value()) - static_cast<moment_t>(speedBar_->minimum())) / (static_cast<moment_t>(speedBar_->maximum()) - static_cast<moment_t>(speedBar_->minimum()));
    speed *= 2;
    window_pos_ += speed;

    //setWindowTitle("Jobs removed: " + QString::number(jobs_removed_));

    updateSubtask();
//    qwt_demo_->setAxisScale(QwtPlot::xBottom, window_pos_ - 1, window_pos_ + window_span_ + 10);
    //qwt_demo_->replot();
    demo2->setAxisScale(QwtPlot::yLeft, window_pos_ - 1, window_pos_ + window_span_ + 10);
    demo2->replot();
}

void SchedDemo::pauseDemo()
{
    play_timer_->stop();
}

void SchedDemo::resetDemo()
{
    task_ = original_task_;
    window_pos_ = -2;
    perm_.clear();
    sched_.clear();
    jobs_removed_ = 0;
    jobs_processed_ = 0;
    total_cost = 0;
    next_job_ = 0;
    total_min_bound_ = 0;
}


void SchedDemo::updateSubtask()
{
    bool changed = false;

    auto checker = [&](const size_t &job) -> bool
    {
        return (sched_.at(job) < window_pos_ - 0.001);
    };

    for (size_t pos = 0; pos < perm_.size(); ++pos)
    {
        const size_t job = perm_[pos];
        
        if (checker(job))
        {
            const moment_t job_sched = sched_.at(job);
            const cost_t job_cost = get_job_cost(task_[job], job_sched);
            total_cost += job_cost;
            ++jobs_processed_;
        }
    }
    for (size_t pos = 0; pos < perm_.size(); ++pos)
    {
        const size_t job = perm_[pos];

        if (checker(job))
        {
            total_min_bound_ = std::max(total_min_bound_, sched_.at(job) + 0.0f/* + get_processing_time(task_, perm_, pos)*/);
        }
    }
    
    perm_t::iterator new_end = std::remove_if (perm_.begin(), perm_.end(), checker);
    if (new_end != perm_.end())
    {
        perm_.resize(new_end - perm_.begin());
        changed = true;
    }

    for (; next_job_ < task_.size(); ++next_job_)
    {
        if (task_[next_job_].due > window_pos_ + window_span_)
            break;
        perm_.push_back(next_job_);
        changed = true;
    }
    
    if (changed)
    {
        //setSolver(selected_solver_);
        runSolver();
        //sched_ = perm2sched(original_task_, perm_);
        updateCost();
    }
}
