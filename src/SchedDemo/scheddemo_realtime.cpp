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
/*
    if (jobs_processed_ + jobs_removed_ == task_.size())
    {
        return;
    }
*/
    
    moment_t speed = (static_cast<moment_t>(speedBar_->value()) - static_cast<moment_t>(speedBar_->minimum())) / (static_cast<moment_t>(speedBar_->maximum()) - static_cast<moment_t>(speedBar_->minimum()));
    speed *= 2;
    window_pos_ += speed;

    //setWindowTitle("Jobs removed: " + QString::number(jobs_removed_));

    updateSubtask(task_, perm_, sched_, next_job_, selected_solver_);
    updateSubtask(task_src_, perm_src_, sched_src_, next_job_src_, 0);
//    qwt_demo_->setAxisScale(QwtPlot::xBottom, window_pos_ - 1, window_pos_ + window_span_ + 10);
    //qwt_demo_->replot();
    demo2_src->setAxisScale(QwtPlot::yLeft, window_pos_ - 1, window_pos_ + window_span_ + 10);
    demo2_src->updateMinBound(window_pos_);
    demo2_src->replot();

    demo2->setAxisScale(QwtPlot::yLeft, window_pos_ - 1, window_pos_ + window_span_ + 10);
    demo2->updateMinBound(window_pos_);
    demo2->replot();
}

void SchedDemo::pauseDemo()
{
    play_timer_->stop();
}

void SchedDemo::resetDemo()
{
    task_ = original_task_;
    perm_.clear();
    sched_.clear();
    
    task_src_ = original_task_;
    perm_src_.clear();
    sched_src_.clear();

    window_pos_ = -2;
    jobs_removed_ = 0;
    jobs_processed_ = 0;
    total_cost = 0;
    next_job_ = 0;
    next_job_src_ = 0;
    total_min_bound_ = 0;
}


void SchedDemo::updateSubtask(task_t &task, perm_t &perm, sched_t &sched, size_t &next_job, size_t slv)
{
    bool changed = false;
    bool flag = false;

    auto checker = [&](const size_t &job) -> bool
    {
        return (sched.at(job)  < window_pos_ - 0.001);
    };

    for (size_t pos = 0; pos < perm.size(); ++pos)
    {
        const size_t job = perm[pos];
        
        if (checker(job))
        {
            const moment_t job_sched = sched.at(job);
            const cost_t job_cost = get_job_cost(original_task_[job], job_sched);
            total_cost += job_cost;
            ++jobs_processed_;
            flag = true;
        }
    }
    for (size_t pos = 0; pos < perm.size(); ++pos)
    {
        const size_t job = perm[pos];

        if (checker(job))
        {
            total_min_bound_ = std::max(total_min_bound_, sched.at(job)  + get_processing_time(task, perm, pos));
        }
    }
    
    perm_t::iterator new_end = std::remove_if(perm.begin(), perm.end(), checker);
    
    if (new_end != perm.end())
    {
        perm.resize(new_end - perm.begin());
        changed = true;
    }

    for (; next_job < task.size(); ++next_job)
    {
        if (task[next_job].due > window_pos_ + window_span_)
            break;
        perm.push_back(next_job);
        changed = true;
    }
    
    if (changed)
    {
        //setSolver(selected_solver_);
        size_t solver_temp = selected_solver_;
        selected_solver_ = slv;
        runSolver(task, perm, sched);
        selected_solver_ = solver_temp;
        //sched = perm2sched(original_task_, perm);
        updateCost();
    }
}
