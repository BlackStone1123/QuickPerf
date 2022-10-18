#pragma once
#include "../CommonDefines.h"

class ChannelDataModel;
class TreeModel;
class SingleChannelController;
class DataGenerator;
class PerfGraphViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TreeModel* graphModel MEMBER mDataModel NOTIFY modelChanged)

public:
    using ControllerList = QList<QPointer<SingleChannelController>>;

    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

    Q_INVOKABLE void registerSingleChannelController(SingleChannelController*);
    Q_INVOKABLE DataGenerator* getDataGenerator(QString key);

signals:
    void modelChanged();
    void insertRowBefore(int index);

public slots:
    void onWheelScaled(const QPointF&);
    void onLeftKeyPressed();
    void onRightKeyPressed();

private:
    //QPointer<ChannelDataModel> mDataModel;
    QPointer<TreeModel> mDataModel;
    ControllerList mControllerList;
};
