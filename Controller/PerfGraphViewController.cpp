#include "PerfGraphViewController.h"
#include <iostream>
#include <QPointF>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "../DataModel/ChannelDataModel.h"
#include "../DataModel/DataGenerator.h"
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
        jsonFile.setFileName("..\\..\\pfa_test_config.json");

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
    mDataModel = new TreeModel(root, this);
}

PerfGraphViewController::~PerfGraphViewController()
{
    std::cout << "PerfGraphViewController deletion" << std::endl;
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
                controller->moveTo(getTopController()->getRangeStartPosition());
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

void PerfGraphViewController::onWheelScaled(const QPointF& point)
{
    if(mControllerList.isEmpty())
        return;

    auto singleController = getTopController();
    int currentDisplayingCount = singleController->getDisplayingDataCount();

    bool zoomIn = point.y() > 0;
    int scaledNumber =  currentDisplayingCount * (zoomIn ? 0.8 : 1.2);
    scaledNumber = std::max(scaledNumber, MINIMUM_DISPLAYING_DATA_COUNT);
    scaledNumber = singleController->requestForZoomStride(scaledNumber);

    if(currentDisplayingCount != scaledNumber)
    {
        for(auto controller: mControllerList)
        {
            controller->zoomTo(scaledNumber);
        }
    }
    else
    {
        std::cout<< "can not zoom more data" << std::endl;
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
        std::cout << "reach to the left end, can not scroll any more!" << std::endl;
        return;
    }

    for (auto controller: mControllerList)
    {
        controller->move(dataStride, false);
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
        std::cout << "reach to the right end, can not scroll any more!" << std::endl;
        return;
    }

    for (auto& controller: mControllerList)
    {
        controller->move(dataStride, true);
    }
}

void PerfGraphViewController::onSliderPositionChanged(int position)
{
    for (auto& controller: mControllerList)
    {
        controller->moveTo(position);
    }
}

void PerfGraphViewController::onSliderRangeChanged(int range)
{

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
            break;
        }
    }
    return mTopController;
}
