#pragma once

class qwt_aircraft :
    public QwtPlotItem
{
public:
    qwt_aircraft(void);
    ~qwt_aircraft(void);
public:
    virtual void draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
};

