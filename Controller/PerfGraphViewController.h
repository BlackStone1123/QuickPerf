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
    Q_PROPERTY(TreeModel* graphModel MEMBER mTreeModel NOTIFY treeModelChanged)
    Q_PROPERTY(ChannelDataModel* listModel MEMBER mListModel NOTIFY listModelChanged)
    Q_PROPERTY(SingleChannelController* topController MEMBER mTopController NOTIFY topControllerChanged)

public:
    using Controller = QPointer<SingleChannelController>;
    using ControllerList = QMap<QString, Controller>;
    using ChannelPinStatus = QMap<QString, bool>;

    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

    Q_INVOKABLE void registerSingleChannelController(const QString& key, SingleChannelController*, bool up);
    Q_INVOKABLE void unRegisterSingleChannelController(const QString& key, bool up);
    Q_INVOKABLE DataGenerator* getDataGenerator(const QString& type, const QString& value);
    SingleChannelController* getTopController();

signals:
    void treeModelChanged();
    void listModelChanged();
    void insertRowBefore(int index);
    void topControllerChanged();

public slots:
    void onWheelScaled(const qreal& , const QPointF&);
    void onLeftKeyPressed();
    void onRightKeyPressed();
    void onSliderPositionChanged(int position);
    void onSplitterDragging(int stride,bool left, bool forward);
    void onPinButtonToggled(const QString& key, const QString& value, bool checked, bool down);

private:
    void __registerSingleChannelController(ControllerList& dst, const QString& key, SingleChannelController*);
    void __unRegisterSingleChannelController(ControllerList& dst, const QString& key);

    SingleChannelController* mTopController {nullptr};
    ControllerList mControllerList;
    ControllerList mUpControllerList;
    QList<Controller> mCtl;
    ChannelPinStatus mChannelStatus;

    QPointer<ChannelDataModel> mListModel;
    QPointer<TreeModel> mTreeModel;

    Controller mCounterTopCtl;
    Controller mEventTopCtl;
};
