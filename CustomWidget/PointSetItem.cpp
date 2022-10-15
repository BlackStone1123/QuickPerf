#include "PointSetItem.h"
#include <QPainter>
#include <QPen>
#include <QRectF>

//PointSetItem::PointSetItem(QQuickItem* parent)
//    : QQuickPaintedItem (parent)
//{
//    setAntialiasing(true);
//}

//void PointSetItem::paint(QPainter *painter)
//{
//    painter->save();

//    painter->setRenderHint(QPainter::Antialiasing);

//    QPen pen = painter->pen();
//    pen.setWidth(2);
//    pen.setBrush(Qt::red);
//    painter->drawEllipse( QRectF(0, 0, width(), height()).adjusted(1,1,-1,-1) );

//    for (int start = mStartPos; start < start + mNumPoints; start++)
//    {

//    }
//    painter->restore();
//}

//void PointSetItem::setStartPosition(int startPos)
//{
//    if(mStartPos != startPos)
//    {
//        mStartPos = startPos;
//        update();
//    }
//}

//void PointSetItem::setNumPoints(int numPoints)
//{
//    if(mNumPoints != numPoints)
//    {
//        mNumPoints = numPoints;
//        update();
//    }
//}

//void PointSetItem::setModel(BarSetModel* model)
//{
//    if(mModel != nullptr)
//        return;

//    mModel = model;
//    update();
//}

//void PointSetItem::setColor(const QColor& color)
//{
//    if(mColor != color)
//    {
//        mColor = color;
//        update();
//    }
//}
