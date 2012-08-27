#include "StdAfx.h"
#include "qwtscheddemo.h"

namespace {

class Histogram: public QwtPlotHistogram
{
public:
    Histogram(const QString &, const QColor &, int alpha);

    void setColor(const QColor &, int alpha);
    void setValues(uint numValues, const double *);
};

Histogram::Histogram(const QString &title, const QColor &symbolColor, const int alpha):
QwtPlotHistogram(title)
{
    setStyle(QwtPlotHistogram::Columns);

    setColor(symbolColor, alpha);
}

void Histogram::setColor(const QColor &symbolColor, const int alpha)
{
    QColor color = symbolColor;
    color.setAlpha(alpha);

    setPen(QPen(Qt::black));
    setBrush(QBrush(color));

    QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
    symbol->setFrameStyle(QwtColumnSymbol::Raised);
    symbol->setLineWidth(1);
    symbol->setPalette(QPalette(color));
    setSymbol(symbol);
}

void Histogram::setValues(uint numValues, const double *values)
{
    QVector<QwtIntervalSample> samples(numValues);
    for ( uint i = 0; i < numValues; i++ )
    {
        QwtInterval interval(double(i), i + 0.5);
        interval.setBorderFlags(QwtInterval::ExcludeMaximum);

        samples[i] = QwtIntervalSample(values[i], interval);
    }

    setData(new QwtIntervalSeriesData(samples));
}

} // anonymous namespace


QwtSchedDemo::QwtSchedDemo(QWidget *parent)
    : QwtPlot(parent)
{
    this->canvas()->setCursor(Qt::ArrowCursor);
    initGrid();
    initData();
    
   
    replot();
}

QwtSchedDemo::~QwtSchedDemo()
{

}

void QwtSchedDemo::initGrid()
{
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableX(false);
    grid->enableY(true);
    grid->enableXMin(false);
    grid->enableYMin(false);
    grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
    grid->attach(this);
}

void QwtSchedDemo::initData()
{
    Histogram *hist_min_bounds = new Histogram("Bounds", Qt::darkGray, 255);
    hist_min_bounds->attach(this);
    hist_min_bounds_ = hist_min_bounds;

    Histogram *hist_max_bounds = new Histogram("Bounds", Qt::darkGray, 255);
    hist_max_bounds->attach(this);
    hist_max_bounds_ = hist_max_bounds;

    Histogram *hist_planes = new Histogram("Planes", Qt::blue, 150);
    hist_planes->attach(this);
    hist_planes_ = hist_planes;

    Histogram *hist_planes_subtask = new Histogram("Planes", Qt::red, 150);
    hist_planes_subtask->attach(this);
    hist_planes_subtask_ = hist_planes_subtask;
}

void QwtSchedDemo::updateData(const task_t &task, const perm_t &perm, const sched_t &sched, const size_t /*subtask_begin_*/, const size_t /*subtask_end_*/)
{
    //assert(subtask_end_ >= subtask_begin_);
    //assert(subtask_end_ <= perm.size());

    QVector<QwtIntervalSample> samples_min_bounds(perm.size());
    QVector<QwtIntervalSample> samples_max_bounds(perm.size());
    QVector<QwtIntervalSample> samples_planes(perm.size());
    
    for (size_t pos = 0; pos < perm.size(); ++pos)
    {
        const size_t job = perm[pos];

        QwtInterval interval(sched.at(job), sched.at(job) + get_processing_time(task, perm, pos));
        interval.setBorderFlags(QwtInterval::ExcludeMaximum);

        samples_min_bounds[pos] = QwtIntervalSample(task[job].min_bound - task[job].due, interval);
        samples_max_bounds[pos] = QwtIntervalSample(task[job].max_bound - task[job].due, interval);
        samples_planes    [pos] = QwtIntervalSample(sched.at(job)       - task[job].due, interval);
    }

    hist_min_bounds_->setData(new QwtIntervalSeriesData(samples_min_bounds));
    hist_max_bounds_->setData(new QwtIntervalSeriesData(samples_max_bounds));
    hist_planes_    ->setData(new QwtIntervalSeriesData(samples_planes));
    
    replot();
}

