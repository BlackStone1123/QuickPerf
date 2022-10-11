#include "PerfGraphViewController.h"
#include <iostream>
#include <QPointF>

#include "../DataModel/ChannelDataModel.h"
#include "../DataModel/DataGenerator.h"

namespace  {
    const int minimumDisplayingCount{10};
}

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

    QObject::connect(this, &PerfGraphViewController::fetchMore, mDataModel, &ChannelDataModel::fetchMoreData);
}

PerfGraphViewController::~PerfGraphViewController()
{
    std::cout << "PerfGraphViewController deletion" << std::endl;
}

void PerfGraphViewController::onWheelScaled(const QPointF& point)
{
    int scaledNumber = mDisplayingDataCount * (point.y() > 0 ? 0.8 : 1.2);
    mDisplayingDataCount = scaledNumber > minimumDisplayingCount ? scaledNumber : minimumDisplayingCount;

    emit displayingDataCountChanged();

    int diff = (size_t)mDisplayingDataCount + mRangeStart - mDataModel->getRange();
    if(diff>0)
    {
        std::cout << "Out of boundary, more data is required:"<< diff << std::endl;
        emit fetchMore(diff);
    }
}

void PerfGraphViewController::onLeftKeyPressed()
{
    int dataStride = mDisplayingDataCount / 5;

    if(mRangeStart <= 0)
    {
        std::cout << "reach to the left end, can not scroll any more!" << std::endl;
        return;
    }
    else if(mRangeStart-dataStride < 0)
    {
        mRangeStart = 0;
    }
    else
    {
        mRangeStart-=dataStride;
    }
    emit rangeStartPosChanged();
}

void PerfGraphViewController::onRightKeyPressed()
{
    size_t dataStride = mDisplayingDataCount / 5;
    size_t rangeEnd = mDisplayingDataCount + mRangeStart;

    if(rangeEnd >= mDataModel->getRange())
    {
        std::cout << "reach to the right end, can not scroll any more!" << std::endl;
        return;
    }
    else if(mDataModel->getRange()-rangeEnd < dataStride)
    {
        mRangeStart += mDataModel->getRange() - rangeEnd;
    }
    else
    {
        mRangeStart += dataStride;
    }
    emit rangeStartPosChanged();
}
