#pragma once
#include "../CommonDefines.h"

class ChannelDataModel;
class PerfGraphViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ChannelDataModel* graphModel MEMBER mDataModel NOTIFY modelChanged)
    Q_PROPERTY(int displayingDataCount MEMBER mDisplayingDataCount NOTIFY displayingDataCountChanged)
    Q_PROPERTY(int rangeStartPos MEMBER mRangeStart NOTIFY rangeStartPosChanged)
    Q_PROPERTY(bool switchDelegate MEMBER mSwitchDelegate NOTIFY switchDelegateChanged)

public:
    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

signals:
    void modelChanged();
    void insertRowBefore(int index);
    void displayingDataCountChanged();
    void fetchMore(size_t count);
    void rangeStartPosChanged();
    void switchDelegateChanged();

public slots:
    void onWheelScaled(const QPointF&);
    void onLeftKeyPressed();
    void onRightKeyPressed();

private:
    QPointer<ChannelDataModel> mDataModel;

    int mDisplayingDataCount{100};
    int mRangeStart{0};
    bool mSwitchDelegate{false};
};
