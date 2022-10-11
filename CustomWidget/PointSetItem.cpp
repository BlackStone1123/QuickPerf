#include "PointSetItem.h"
#include <QPainter>
#include <QPen>
#include <QRectF>

PointSetItem::PointSetItem(QQuickItem* parent)
    : QQuickPaintedItem (parent)
{

}

void PointSetItem::paint(QPainter *painter)
{
    painter->save();

    QPen pen = painter->pen();
    pen.setWidth(3);
    pen.setBrush(Qt::red);
    painter->drawEllipse( QRectF(0, 0, width(), height()).adjusted(1,1,-1,-1) );

    painter->restore();
}
