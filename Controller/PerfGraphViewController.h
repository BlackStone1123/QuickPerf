#pragma once
#include "../CommonDefines.h"
#include <QMap>

class ChannelDataModel;
class TreeModel;
class SingleChannelController;
class DataGenerator;
class PerfGraphViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TreeModel* graphModel MEMBER mDataModel NOTIFY modelChanged)

public:
    using ControllerList = QMap<QString, QPointer<SingleChannelController>>;

    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

    Q_INVOKABLE void registerSingleChannelController(const QString& key, SingleChannelController*);
    Q_INVOKABLE void unRegisterSingleChannelController(const QString& key);
    Q_INVOKABLE DataGenerator* getDataGenerator(const QString& value);

signals:
    void modelChanged();
    void insertRowBefore(int index);

public slots:
    void onWheelScaled(const qreal& , const QPointF&);
    void onLeftKeyPressed();
    void onRightKeyPressed();
    void onSliderPositionChanged(int position);
    void onSplitterDragging(int stride,bool left, bool forward);

private:
    SingleChannelController* getTopController();

    SingleChannelController* mTopController {nullptr};
    //QPointer<ChannelDataModel> mDataModel;
    QPointer<TreeModel> mDataModel;
    ControllerList mControllerList;
};
