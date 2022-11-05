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
    using ControllerList = QMap<QString, QPointer<SingleChannelController>>;

    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

    Q_INVOKABLE void registerSingleChannelController(const QString& key, SingleChannelController*);
    Q_INVOKABLE void unRegisterSingleChannelController(const QString& key);
    Q_INVOKABLE DataGenerator* getDataGenerator(const QString& value);
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
    SingleChannelController* mTopController {nullptr};
    ControllerList mControllerList;

    QPointer<ChannelDataModel> mListModel;
    QPointer<TreeModel> mTreeModel;
};
