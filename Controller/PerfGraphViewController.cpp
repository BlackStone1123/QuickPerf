#include "PerfGraphViewController.h"
#include <iostream>
#include <QPointF>

#include "../DataModel/ChannelDataModel.h"
#include "../DataModel/DataGenerator.h"

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
    mDataModel->zoomTo(mDisplayingDataCount);
}

PerfGraphViewController::~PerfGraphViewController()
{
    std::cout << "PerfGraphViewController deletion" << std::endl;
}

void PerfGraphViewController::onWheelScaled(const QPointF& point)
{
    int scaledNumber = mDisplayingDataCount * (point.y() > 0 ? 0.8 : 1.2);
    scaledNumber = std::max(scaledNumber, MINIMUM_DISPLAYING_DATA_COUNT);
    scaledNumber = mDataModel->requestForZoomStride(scaledNumber);

    if(mDisplayingDataCount != scaledNumber)
    {
        mDisplayingDataCount = scaledNumber;
        emit displayingDataCountChanged();

        mDataModel->zoomTo(mDisplayingDataCount);
    }
    else
    {
        std::cout<< "can not zoom more data" << std::endl;
    }
}

void PerfGraphViewController::onLeftKeyPressed()
{
    size_t dataStride = mDataModel->requestForMoveStride(mDisplayingDataCount / 5, false);

    if(dataStride == 0)
    {
        std::cout << "reach to the left end, can not scroll any more!" << std::endl;
        return;
    }

    mRangeStart-=dataStride;
    emit rangeStartPosChanged();

    mDataModel->move(dataStride, false);
}

void PerfGraphViewController::onRightKeyPressed()
{
    size_t dataStride = mDataModel->requestForMoveStride(mDisplayingDataCount / 5, true);

    if(dataStride == 0)
    {
        std::cout << "reach to the right end, can not scroll any more!" << std::endl;
        return;
    }

    mRangeStart += dataStride;
    emit rangeStartPosChanged();

    mDataModel->move(dataStride, true);
}
