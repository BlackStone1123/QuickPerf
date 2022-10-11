#pragma once
#include <QQuickPaintedItem>

class PointSetItem : public QQuickPaintedItem
{
    Q_OBJECT
public:
    PointSetItem(QQuickItem* parent = nullptr);
    virtual void paint(QPainter *painter) override;

signals:

public slots:
};
