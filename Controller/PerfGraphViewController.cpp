#include "PerfGraphViewController.h"
#include <QPointF>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QtMath>

#include "../DataModel/ChannelDataModel.h"
#include "../Json/JsonEntry.h"
#include "../DataModel/TreeItem.h"
#include "../DataModel/TreeModel.h"
#include "../DataModel/DataCenter.h"

#include "SingleChannelController.h"

namespace  {
    void loadValue(const QJsonValue& value, TreeItem* parent, QString& tracker, QString& type)
    {
        if(value.isObject()) {

            auto object = value.toObject();
            for(auto it=object.begin(); it!=object.end(); ++it)
            {
                if(it.value().isObject())
                {
                    if(it.key() == "top")
                    {
                        loadValue(it.value(), nullptr, tracker, type);
                    }
                    else
                    {
                        auto child = new TreeItem();

                        QString tracker;
                        QString type;
                        loadValue(it.value(), child, tracker, type);

                        JsonEntry entry;
                        entry.setKey(it.key());
                        entry.setValue(tracker);
                        entry.setType(type);

                        child->setData(QVariant::fromValue(entry));
                        parent->appendChild(child);
                    }
                }
                else {
                    tracker = it.key() == "tracker" ? it.value().toString() : tracker;
                    type = it.key() == "type" ? it.value().toString(): type;
                }
            }
        }
    }

    void populateTree(TreeItem* rootItem)
    {
        QFile jsonFile;
        jsonFile.setFileName("pfa_test_config_3.json");

        if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            qCritical() << "error: json file cannot be open";
            return;
        }

        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(jsonFile.readAll(), &error);
        qDebug() << "loading json file:" << error.errorString();

        auto root = doc.isArray() ? QJsonValue(doc.array()) : QJsonValue(doc.object());

        QString tracker;
        QString type;
        loadValue(root, rootItem, tracker, type);
    }

}

PerfGraphViewController::PerfGraphViewController(QObject* parent)
    :QObject(parent)
{
    TreeItem* root = new TreeItem;
    populateTree(root);
    mTreeModel = new TreeModel(root, this);
    mListModel = new ChannelDataModel(this);
    mRangeConverter = new CycleRangeConverter(1,0, INITIAL_DISPLAYING_DATA_RANGE * 100, this);
}

PerfGraphViewController::~PerfGraphViewController()
{
    qDebug() << "PerfGraphViewController deletion";
}

void PerfGraphViewController::registerSingleChannelController(const QString& key, SingleChannelController* controller, bool up)
{
    __registerSingleChannelController(up ? mUpControllerList : mControllerList, key, controller);
}

void PerfGraphViewController::unRegisterSingleChannelController(const QString& columnName, bool up)
{
    __unRegisterSingleChannelController(up ? mUpControllerList : mControllerList, columnName);
}

DataGenerator* PerfGraphViewController::getDataGenerator(const QString& key, const QString& type, const QString& value)
{
    if(value.isEmpty() || type.isEmpty())
        return nullptr;

    DataGenerator* pGenerator = ExcelDataCenter::creatDataGenerator(
                type == "Counter" ? DataType::Count :
                type == "Event" ? DataType::Event:
                DataType::Slice, key, value);

    if(pGenerator)
    {
        return pGenerator;
    }

    return  nullptr;
}

CycleRangeConverter* PerfGraphViewController::getRangeConverter(const QString& type)
{
    return new CycleRangeConverter(type == "Counter" ? 100 : 1);
}

void PerfGraphViewController::onWheelScaled(const qreal& ratio, const QPointF& point)
{
    if(mCtl.isEmpty())
        return;

    int currentDisplayingCount = mRangeConverter->getConvertedDisplayingRange();
    bool zoomIn = point.y() > 0;

    int scaledNumber =  currentDisplayingCount * (zoomIn ? 0.8 : 1.2);
    scaledNumber = std::max(scaledNumber, MINIMUM_DISPLAYING_DATA_COUNT);
    scaledNumber = requestForZoomStride(scaledNumber);

    int leftSliderMoveStride = qCeil(qFabs(currentDisplayingCount - scaledNumber) * ratio);
    int rightSliderMoveStride = qFabs(currentDisplayingCount - scaledNumber) - leftSliderMoveStride;

    if(!zoomIn)
    {
        leftSliderMoveStride = requestForMoveStride(leftSliderMoveStride, false);
        rightSliderMoveStride = requestForMoveStride(rightSliderMoveStride, true);
    }

    mRangeConverter->boundaryMove(leftSliderMoveStride, true, zoomIn);
    mRangeConverter->boundaryMove(rightSliderMoveStride, false, !zoomIn);

    if(currentDisplayingCount != scaledNumber){
        for(auto controller: mCtl){
            controller->sliderMove(leftSliderMoveStride, true, zoomIn);
            controller->sliderMove(rightSliderMoveStride, false, !zoomIn);
        }
    }
    else{
        qDebug()<< "can not zoom more data";
    }
}

void PerfGraphViewController::onLeftKeyPressed()
{
    if(mCtl.isEmpty())
        return;

    int currentDisplayingCount = mRangeConverter->getConvertedDisplayingRange();
    size_t dataStride = requestForMoveStride(currentDisplayingCount / 5, false);

    if(dataStride == 0)
    {
        qDebug() << "reach to the left end, can not scroll any more!";
        return;
    }

    mRangeConverter->integralMove(dataStride, false);

    for (auto controller: mCtl)
    {
        controller->integralMove(dataStride, false);
    }
}

void PerfGraphViewController::onRightKeyPressed()
{
    if(mCtl.isEmpty())
        return;

    int currentDisplayingCount = mRangeConverter->getConvertedDisplayingRange();
    size_t dataStride = requestForMoveStride(currentDisplayingCount / 5, true);

    if(dataStride == 0)
    {
        qDebug()<< "reach to the right end, can not scroll any more!";
        return;
    }

    mRangeConverter->integralMove(dataStride, true);

    for (auto& controller: mCtl)
    {
        controller->integralMove(dataStride, true);
    }
}

void PerfGraphViewController::onSliderPositionChanged(int position)
{
    mRangeConverter->integralMoveTo(position);

    for (auto& controller: mCtl)
    {
        controller->integralMoveTo(position);
    }
}
void PerfGraphViewController::onSplitterDragging(int stride, bool left, bool forward)
{
    for(auto controller: mCtl){
        controller->sliderMove(stride, left, forward);
    }
}

void PerfGraphViewController::onPinButtonToggled(const QString& key, const QString& value, const QString& type, bool checked, bool down)
{
    if(down)
    {
        if(checked)
        {
            mListModel->appendChannelData(key, value, type);
        }
        else {
            mListModel->removeChannelData(key);
        }
        mChannelStatus[key] = checked;
        mControllerList[key]->updatePinStatus(checked, true);
    }
    else{
        if(checked)
        {
            qDebug() << "impossible case!! the status of up pin button should always be checked";
        }
        else {
            mListModel->removeChannelData(key);
            mChannelStatus[key] = false;

            if(mControllerList.find(key) != mControllerList.end())
            {
                mControllerList[key]->updatePinStatus(false, false);
            }
        }
    }
}

void PerfGraphViewController::__registerSingleChannelController(ControllerList& dst, const QString& key, SingleChannelController* controller)
{
    if(!key.isEmpty() && controller->getDataGenerator() != nullptr)
    {
        if(dst.find(key) == dst.end())
        {
            dst[key] = controller;
            controller->setKey(key);

            int rangeStart = mRangeConverter->getConvertedRangeStart();
            int displayingCount = mRangeConverter->getConvertedDisplayingRange();

            InitialStatus status;
            status.totalCycleRange = rangeStart + displayingCount;
            status.displayingCycleRange = displayingCount;
            status.pinding = mChannelStatus.find(key) == mChannelStatus.end() ? false : mChannelStatus[key];

            controller->loadInitialDatas(status);

            if(!dst.isEmpty())
            {
                controller->integralMoveTo(rangeStart);
                controller->zoomTo(displayingCount);
            }
            mCtl.append(controller);
        }
        else {
            qDebug() << "the key is duplicated :" << key;
        }
    }
}

void PerfGraphViewController::__unRegisterSingleChannelController(ControllerList& dst, const QString& key)
{
    auto itr = dst.find(key);
    if(itr != dst.end())
    {
        mCtl.removeAll(itr.value());
        dst.erase(itr);
    }
}

int PerfGraphViewController::requestForMoveStride(size_t preferSize, bool forward)
{
    int rangeStart = mRangeConverter->getConvertedRangeStart();
    int displayingCount = mRangeConverter->getConvertedDisplayingRange();

    size_t backendSize = mTotalCycleRange;
    size_t residual = forward ? (backendSize - rangeStart - displayingCount) : rangeStart;

    return std::min(residual, preferSize);
}

int PerfGraphViewController::requestForZoomStride(size_t count)
{
    return std::min(count, mTotalCycleRange);
}
