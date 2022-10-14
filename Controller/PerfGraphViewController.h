#pragma once
#include "../CommonDefines.h"

class ChannelDataModel;
class PerfGraphViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ChannelDataModel* graphModel MEMBER mDataModel NOTIFY modelChanged)
    Q_PROPERTY(int displayingDataCount MEMBER mDisplayingDataCount NOTIFY displayingDataCountChanged)
    Q_PROPERTY(int rangeStartPos MEMBER mRangeStart NOTIFY rangeStartPosChanged)

public:
    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

signals:
    void modelChanged();
    void insertRowBefore(int index);
    void rangeStartPosChanged();
    void displayingDataCountChanged();

public slots:
    void onWheelScaled(const QPointF&);
    void onLeftKeyPressed();
    void onRightKeyPressed();

private:
    QPointer<ChannelDataModel> mDataModel;

    int mDisplayingDataCount{INITIAL_DISPLAYING_DATA_RANGE};
    int mRangeStart{0};
};
