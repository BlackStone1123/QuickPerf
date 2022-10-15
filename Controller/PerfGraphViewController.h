#pragma once
#include "../CommonDefines.h"

class ChannelDataModel;
class SingleChannelController;
class PerfGraphViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ChannelDataModel* graphModel MEMBER mDataModel NOTIFY modelChanged)

public:
    using ControllerList = QList<QPointer<SingleChannelController>>;

    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

    Q_INVOKABLE void registerSingleChannelController(SingleChannelController*);

signals:
    void modelChanged();
    void insertRowBefore(int index);

public slots:
    void onWheelScaled(const QPointF&);
    void onLeftKeyPressed();
    void onRightKeyPressed();

private:
    QPointer<ChannelDataModel> mDataModel;
    ControllerList mControllerList;
};
