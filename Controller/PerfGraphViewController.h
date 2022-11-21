#pragma once
#include "../CommonDefines.h"
#include <QMap>

class ChannelDataModel;
class TreeModel;
class SingleChannelController;
class DataGenerator;
class CycleRangeConverter;
class PerfGraphViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TreeModel* graphModel MEMBER mTreeModel NOTIFY treeModelChanged)
    Q_PROPERTY(ChannelDataModel* listModel MEMBER mListModel NOTIFY listModelChanged)
    Q_PROPERTY(CycleRangeConverter* rangeConverter MEMBER mRangeConverter NOTIFY rangeConverterChanged)
    Q_PROPERTY(int totalCycleRange MEMBER mTotalCycleRange NOTIFY totalCycleRangeChanged)

public:
    using Controller = QPointer<SingleChannelController>;
    using ControllerList = QMap<QString, Controller>;
    using ChannelPinStatus = QMap<QString, bool>;

    PerfGraphViewController(QObject* parent = nullptr);
    virtual ~PerfGraphViewController();

    Q_INVOKABLE void registerSingleChannelController(const QString& key, SingleChannelController*, bool up);
    Q_INVOKABLE void unRegisterSingleChannelController(const QString& key, bool up);
    Q_INVOKABLE DataGenerator* getDataGenerator(const QString& key, const QString& type, const QString& value);
    Q_INVOKABLE CycleRangeConverter* getRangeConverter(const QString& type);

signals:
    void treeModelChanged();
    void listModelChanged();
    void totalCycleRangeChanged();
    void rangeConverterChanged();

public slots:
    void onWheelScaled(const qreal& , const QPointF&);
    void onLeftKeyPressed();
    void onRightKeyPressed();
    void onSliderPositionChanged(int position);
    void onSplitterDragging(int stride,bool left, bool forward);
    void onPinButtonToggled(const QString& key, const QString& value, const QString& type, bool checked, bool down);

private:
    int requestForMoveStride(size_t preferSize, bool forward);
    int requestForZoomStride(size_t count);

    void __registerSingleChannelController(ControllerList& dst, const QString& key, SingleChannelController*);
    void __unRegisterSingleChannelController(ControllerList& dst, const QString& key);

    ControllerList mControllerList;
    ControllerList mUpControllerList;
    QList<Controller> mCtl;
    ChannelPinStatus mChannelStatus;

    QPointer<ChannelDataModel> mListModel;
    QPointer<TreeModel> mTreeModel;
    QPointer<CycleRangeConverter> mRangeConverter;

    size_t mTotalCycleRange{279400};
};
