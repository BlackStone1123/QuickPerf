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

    QString loadValue(const QJsonValue& value, TreeItem* parent)
    {
        if(value.isObject()) {
            auto object = value.toObject();
            for(auto it=object.begin(); it!=object.end(); ++it){
                JsonEntry entry;
                entry.setKey(it.key());
                if(it.value().isObject()){
                    entry.setType(QJsonValue::Object);
                    auto child = new TreeItem();
                    entry.setValue(loadValue(it.value(), child));
                    child->setData(QVariant::fromValue(entry));
                    parent->appendChild(child);
                }
                else {
                    if(it.key() == "top")
                    {
                        return it.value().toString();
                    }
                    entry.setType(QJsonValue::String);
                    entry.setValue(it.value().toVariant());
                    auto child = new TreeItem(QVariant::fromValue(entry));
                    parent->appendChild(child);
                }
            }
        }
        return QString();
    }

    void populateTree(TreeItem* rootItem)
    {
        QFile jsonFile;
        jsonFile.setFileName("pfa_test_config.json");

        if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            qCritical() << "error: json file cannot be open";
            return;
        }

        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(jsonFile.readAll(), &error);
        qDebug() << "loading json file:" << error.errorString();

        auto root = doc.isArray() ? QJsonValue(doc.array()) : QJsonValue(doc.object());
        loadValue(root, rootItem);
    }

}

PerfGraphViewController::PerfGraphViewController(QObject* parent)
    :QObject(parent)
{
    TreeItem* root = new TreeItem;
    populateTree(root);
    mTreeModel = new TreeModel(root, this);
    mListModel = new ChannelDataModel(this);
}

PerfGraphViewController::~PerfGraphViewController()
{
    qDebug() << "PerfGraphViewController deletion";
}

void PerfGraphViewController::registerSingleChannelController(const QString& key, SingleChannelController* controller)
{
    if(!key.isEmpty() && controller->getDataGenerator() != nullptr)
    {
        if(mControllerList.find(key) == mControllerList.end())
        {
            mControllerList[key] = controller;
            controller->setKey(key);
            controller->loadInitialDatas(mControllerList.isEmpty() ? -1 : getTopController()->getTotalRange());

            if(!mControllerList.isEmpty())
            {
                controller->integralMoveTo(getTopController()->getRangeStartPosition());
                controller->zoomTo(getTopController()->getDisplayingDataCount());
            }
        }
        else {
            qDebug() << "the key is duplicated :" << key;
        }
    }
}

void PerfGraphViewController::unRegisterSingleChannelController(const QString& columnName)
{
    auto itr = mControllerList.find(columnName);
    if(itr != mControllerList.end())
    {
        if(itr.value() == mTopController)
        {
            mTopController = nullptr;
        }
        mControllerList.erase(itr);
    }
}

DataGenerator* PerfGraphViewController::getDataGenerator(const QString& value)
{
    if(value.isEmpty())
        return nullptr;

    DataGenerator* pGenerator = ExcelDataCenter::creatDataGenerator(value);

    if(pGenerator)
    {
        return pGenerator;
    }

    return  nullptr;
}

void PerfGraphViewController::onWheelScaled(const qreal& ratio, const QPointF& point)
{
    if(mControllerList.isEmpty())
        return;

    auto singleController = getTopController();
    int currentDisplayingCount = singleController->getDisplayingDataCount();
    bool zoomIn = point.y() > 0;

    int scaledNumber =  currentDisplayingCount * (zoomIn ? 0.8 : 1.2);
    scaledNumber = std::max(scaledNumber, MINIMUM_DISPLAYING_DATA_COUNT);
    scaledNumber = singleController->requestForZoomStride(scaledNumber);

    int leftSliderMoveStride = qCeil(qFabs(currentDisplayingCount - scaledNumber) * ratio);
    int rightSliderMoveStride = qFabs(currentDisplayingCount - scaledNumber) - leftSliderMoveStride;

    if(!zoomIn)
    {
        leftSliderMoveStride = singleController->requestForMoveStride(leftSliderMoveStride, false);
        rightSliderMoveStride = singleController->requestForMoveStride(rightSliderMoveStride, true);
    }

    if(currentDisplayingCount != scaledNumber){
        for(auto controller: mControllerList){
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
    if(mControllerList.isEmpty())
        return;

    auto singleController = getTopController();
    int currentDisplayingCount = singleController->getDisplayingDataCount();

    size_t dataStride = singleController->requestForMoveStride(currentDisplayingCount / 5, false);

    if(dataStride == 0)
    {
        qDebug() << "reach to the left end, can not scroll any more!";
        return;
    }

    for (auto controller: mControllerList)
    {
        controller->integralMove(dataStride, false);
    }
}

void PerfGraphViewController::onRightKeyPressed()
{
    if(mControllerList.isEmpty())
        return;

    auto singleController = getTopController();
    int currentDisplayingCount = singleController->getDisplayingDataCount();

    size_t dataStride = singleController->requestForMoveStride(currentDisplayingCount / 5, true);

    if(dataStride == 0)
    {
        qDebug()<< "reach to the right end, can not scroll any more!";
        return;
    }

    for (auto& controller: mControllerList)
    {
        controller->integralMove(dataStride, true);
    }
}

void PerfGraphViewController::onSliderPositionChanged(int position)
{
    for (auto& controller: mControllerList)
    {
        controller->integralMoveTo(position);
    }
}
void PerfGraphViewController::onSplitterDragging(int stride, bool left, bool forward)
{
    for(auto controller: mControllerList){
        controller->sliderMove(stride, left, forward);
    }
}

void PerfGraphViewController::onPinButtonToggled(const QString& key, const QString& value, bool checked, bool down)
{
    if(down)
    {
        if(checked)
        {
            mListModel->appendChannelData(key + "_on", value);
        }
        else {
            mListModel->removeChannelData(key + "_on");
        }
    }
    else{
        if(checked)
        {
            qDebug() << "impossible case!! the status of up pin button should always be checked";
        }
        else {
            mListModel->removeChannelData(key);
        }
    }
}

SingleChannelController* PerfGraphViewController::getTopController()
{
    if(mTopController)
        return mTopController;

    for(auto& controller: mControllerList)
    {
        if(controller->getDataGenerator() != nullptr)
        {
            mTopController = controller;
            emit topControllerChanged();
            break;
        }
    }
    return mTopController;
}
