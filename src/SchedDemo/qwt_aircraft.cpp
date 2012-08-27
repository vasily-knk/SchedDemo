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
    QRectF rect (xMap.transform(0), yMap.transform(100), xMap.transform(400), yMap.transform(25));
    QwtPainter::drawRect(painter, rect);
}

QRectF qwt_aircraft::boundingRect() const
{
    return QRectF (0, 0, 100, 200);
}
