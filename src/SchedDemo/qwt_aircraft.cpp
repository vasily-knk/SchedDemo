#include "StdAfx.h"
#include "qwt_aircraft.h"


qwt_aircraft::qwt_aircraft()
{
}


qwt_aircraft::~qwt_aircraft()
{
}

void qwt_aircraft::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect) const
{
    QRectF rect (xMap.transform(0), yMap.transform(0), xMap.transform(100), yMap.transform(200));
    painter->drawRect(rect);
}
