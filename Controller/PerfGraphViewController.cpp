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

#include "SingleChannelController.h"

namespace  {

    void loadValue(const QJsonValue& value, TreeItem* parent)
    {
        if(value.isObject()) {
            auto object = value.toObject();
            for(auto it=object.begin(); it!=object.end(); ++it){
                JsonEntry entry;
                entry.setKey(it.key());
                entry.setType(QJsonValue::Object);
                if(it.value().isArray() || it.value().isObject()){
                    auto child = new TreeItem(QVariant::fromValue(entry));
                    parent->appendChild(child);
                    loadValue(it.value(), child);
                }
                else {
                    entry.setValue(it.value().toVariant());
                    auto child = new TreeItem(QVariant::fromValue(entry));
                    parent->appendChild(child);
                }
            }
        }
        else if(value.isArray()){
            int index = 0;
            auto array = value.toArray();
            for(auto&& element: array){
                JsonEntry entry;
                entry.setKey("[" + QString::number(index) + "]");
                entry.setType(QJsonValue::Array);
                auto child = new TreeItem(QVariant::fromValue(entry));
                parent->appendChild(child);
                loadValue(element, child);
                ++index;
            }
        }
    }

    void populateTree(TreeItem* rootItem)
    {
        QFile jsonFile;
        jsonFile.setFileName("D:\\QuickPerf\\sample.json");

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
//    QList<QPointer<DataGenerator>> genList;
//    for(int i = 0; i < 10; i++)
//    {
//        genList.append(new RandomDataGenerator(this));
//    }

//    mDataModel = new ChannelDataModel(genList, this);
//    QObject::connect(this, &PerfGraphViewController::insertRowBefore, [this](int index){
//        QPointer<DataGenerator> gen = new RandomDataGenerator(this);
//        mDataModel->addChannelDataBefore(index, gen);
//    });

    TreeItem* root = new TreeItem;
    populateTree(root);
    mDataModel = new TreeModel(root, this);
}

PerfGraphViewController::~PerfGraphViewController()
{
    std::cout << "PerfGraphViewController deletion" << std::endl;
}

void PerfGraphViewController::registerSingleChannelController(SingleChannelController* controller)
{
    controller->loadInitialDatas();
    mControllerList.push_back(controller);
}

DataGenerator* PerfGraphViewController::getDataGenerator(QString key)
{
    return new RandomDataGenerator(this);
}

void PerfGraphViewController::onWheelScaled(const QPointF& point)
{
    auto singleController = mControllerList.front();
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
    auto singleController = mControllerList.front();
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
    auto singleController = mControllerList.front();
    int currentDisplayingCount = singleController->getDisplayingDataCount();

    size_t dataStride = singleController->requestForMoveStride(currentDisplayingCount / 5, true);

    if(dataStride == 0)
    {
        std::cout << "reach to the right end, can not scroll any more!" << std::endl;
        return;
    }

    for (auto controller: mControllerList)
    {
        controller->move(dataStride, true);
    }
}
