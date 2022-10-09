#pragma once
#include "../CommonDefines.h"

class ChannelDataModel;
class PerfGraphViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ChannelDataModel* graphModel MEMBER mDataModel NOTIFY modelChanged)
    Q_PROPERTY(int displayingDataCount MEMBER mDisplayingDataCount NOTIFY displayingDataCountChanged)
public:
    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

signals:
    void modelChanged();
    void insertRowBefore(int index);
    void displayingDataCountChanged();

public slots:
    void onWheelScaled(const QPointF&);

private:
    QPointer<ChannelDataModel> mDataModel;
    int mDisplayingDataCount{100};
};
