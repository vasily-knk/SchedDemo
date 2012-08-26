#include "StdAfx.h"
#include "qwt_demo_2.h"
#include "qwt_aircraft.h"

qwt_demo_2::qwt_demo_2(QWidget *parent)
    : QwtPlot(parent)
{
    qwt_aircraft *p = new qwt_aircraft;
    p->attach(this);
}

qwt_demo_2::~qwt_demo_2()
{

}
