#pragma once
#include <QQuickPaintedItem>
#include <QPointer>
#include <QColor>
#include "../DataModel/ChannelDataModel.h"

class PointSetItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int startPos WRITE setStartPosition NOTIFY startPositionChanged)
    Q_PROPERTY(int numPoints WRITE setNumPoints NOTIFY numPointsChanged)
    Q_PROPERTY(QColor color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QPointer<BarSetModel> model WRITE setModel)

public:
    PointSetItem(QQuickItem* parent = nullptr);
    virtual void paint(QPainter *painter) override;

    void setStartPosition(int startPos);
    void setNumPoints(int numPoints);
    void setModel(BarSetModel* model);
    void setColor(const QColor& color);

signals:
    void startPositionChanged();
    void numPointsChanged();

private:
    QPointer<BarSetModel> mModel{nullptr};
    QColor mColor;

    int mStartPos;
    int mNumPoints;
};
