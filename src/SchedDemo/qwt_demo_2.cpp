#include "StdAfx.h"
#include "qwt_demo_2.h"
#include "qwt_aircraft.h"

class qwt_demo_2::Histogram: public QwtPlotHistogram
{
public:
    Histogram(const QString &, const QColor &, int alpha);

    void setColor(const QColor &, int alpha);
};

qwt_demo_2::Histogram::Histogram(const QString &title, const QColor &symbolColor, const int alpha):
QwtPlotHistogram(title)
{
    setStyle(QwtPlotHistogram::Columns);
    setColor(symbolColor, alpha);
    setOrientation(Qt::Horizontal);
}

void qwt_demo_2::Histogram::setColor(const QColor &symbolColor, const int alpha)
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

qwt_demo_2::qwt_demo_2(QWidget *parent)
    : QwtPlot(parent)
{
    red_lower_bounds = new Histogram("red lower bounds", Qt::darkGray, 150);
    red_upper_bounds = new Histogram("red upper bounds", Qt::darkGray, 150);
    red_aircrafts = new Histogram("red aircrafts", Qt::red, 255);
    red_lower_bounds->attach(this);
    red_upper_bounds->attach(this);
    red_aircrafts->attach(this);

    yellow_lower_bounds = new Histogram("yellow lower bounds", Qt::darkGray, 150);
    yellow_upper_bounds = new Histogram("yellow upper bounds", Qt::darkGray, 150);
    yellow_aircrafts = new Histogram("yellow aircrafts", Qt::yellow, 255);
    yellow_lower_bounds->attach(this);
    yellow_upper_bounds->attach(this);
    yellow_aircrafts->attach(this);

    green_lower_bounds = new Histogram("green lower bounds", Qt::darkGray, 150);
    green_upper_bounds = new Histogram("green upper bounds", Qt::darkGray, 150);
    green_aircrafts = new Histogram("green aircrafts", Qt::green, 255);
    green_lower_bounds->attach(this);
    green_upper_bounds->attach(this);
    green_aircrafts->attach(this);

    names.resize(50);
    for (size_t i = 0; i < names.size(); ++i)
    {
        names[i] = new QwtPlotMarker;
        names[i]->setLabelAlignment(Qt::AlignRight);
        names[i]->attach(this);
    }

    QwtSymbol *sym = new QwtSymbol(QwtSymbol::HLine, QBrush(Qt::red), QPen(Qt::red), QSize(-100,100));
    min_bound_marker_ = new QwtPlotMarker;
    min_bound_marker_->setSymbol(sym);
    min_bound_marker_->attach(this);

    initGrid();
}

qwt_demo_2::~qwt_demo_2()
{

}

void qwt_demo_2::updateData(const task_t &task, const perm_t &perm, const sched_t &sched)
{
    updateSpecificData(task, perm, sched, aircraft_t::RED, red_lower_bounds, red_upper_bounds, red_aircrafts);
    updateSpecificData(task, perm, sched, aircraft_t::YELLOW, yellow_lower_bounds, yellow_upper_bounds, yellow_aircrafts);
    updateSpecificData(task, perm, sched, aircraft_t::GREEN, green_lower_bounds, green_upper_bounds, green_aircrafts);
    for (size_t i = 0; i < names.size(); ++i)
    {
        names[i]->setValue(QPointF(0, 0));
        names[i]->setVisible(false);
    }
    
    for (size_t pos = 0; pos < perm.size(); ++pos)
    {
        const size_t job = perm[pos];
        if (pos < names.size())
        {
            QFont font;
            //font.setBold(true);
            font.setPointSize(12);
            QString str;
            switch (task[job].class_)
            {
            case aircraft_t::LIGHT:
                str = "L";
                break;
            case aircraft_t::MEDIUM:
                str = "M";
                break;
            case aircraft_t::HEAVY:
                str = "H";
                break;
            }
            str += "  ";
            str += QString::fromStdString(task[job].name);
            QwtText text(str);
            text.setFont(font);
            
            names[pos]->setLabel(text);
            names[pos]->setValue(0, sched.at(job) + get_processing_time(task, perm, pos) / 2);
            names[pos]->setVisible(true);
        }
    }
    replot();
}

void qwt_demo_2::updateSpecificData(const task_t &task, const perm_t &perm, const sched_t &sched, 
    aircraft_t::aircraft_priority priority, Histogram *lb, Histogram *ub, Histogram *aircrafts)
{
    QVector<QwtIntervalSample> samples_lower_bounds;
    QVector<QwtIntervalSample> samples_upper_bounds;
    QVector<QwtIntervalSample> samples_planes;
    

    for (size_t pos = 0; pos < perm.size(); ++pos)
    {
        const size_t job = perm[pos];
        if (task[job].priority_ != priority)
            continue;
        
        QwtInterval interval(sched.at(job), sched.at(job) + get_processing_time(task, perm, pos));
        interval.setBorderFlags(QwtInterval::ExcludeMaximum);

        samples_lower_bounds.push_back(QwtIntervalSample(task[job].min_bound - task[job].due, interval));
        samples_upper_bounds.push_back(QwtIntervalSample(task[job].max_bound - task[job].due, interval));
        samples_planes.push_back(QwtIntervalSample(sched.at(job) - task[job].due, interval));
    }

    lb->setData(new QwtIntervalSeriesData(samples_lower_bounds));
    ub->setData(new QwtIntervalSeriesData(samples_upper_bounds));
    aircrafts->setData(new QwtIntervalSeriesData(samples_planes));
}

void qwt_demo_2::initGrid()
{
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableX(true);
    grid->enableY(true);
    grid->enableXMin(false);
    grid->enableYMin(false);
    grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
    grid->attach(this);

}

void qwt_demo_2::updateMinBound(moment_t min_bound)
{
    min_bound_marker_->setYValue(min_bound);
}



