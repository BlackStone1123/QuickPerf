#include "PerfGraphViewController.h"
#include <iostream>
#include <QPointF>

#include "../DataModel/ChannelDataModel.h"
#include "../DataModel/DataGenerator.h"
#include "SingleChannelController.h"

PerfGraphViewController::PerfGraphViewController(QObject* parent)
    :QObject(parent)
{
    QList<QPointer<DataGenerator>> genList;
    for(int i = 0; i < 6; i++)
    {
        genList.append(new RandomDataGenerator(this));
    }

    mDataModel = new ChannelDataModel(genList, this);
    QObject::connect(this, &PerfGraphViewController::insertRowBefore, [this](int index){
        QPointer<DataGenerator> gen = new RandomDataGenerator(this);
        mDataModel->addChannelDataBefore(index, gen);
    });
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
