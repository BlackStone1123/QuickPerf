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

    QObject::connect(this, &PerfGraphViewController::fetchMore, mDataModel, &ChannelDataModel::fetchMoreData);
}

PerfGraphViewController::~PerfGraphViewController()
{
    std::cout << "PerfGraphViewController deletion" << std::endl;
}

void PerfGraphViewController::onWheelScaled(const QPointF& point)
{
    int scaledNumber = mDisplayingDataCount * (point.y() > 0 ? 0.8 : 1.2);
    scaledNumber = std::max(scaledNumber, MINIMUM_DISPLAYING_DATA_COUNT);

    if(mDisplayingDataCount != scaledNumber)
    {
        mDisplayingDataCount = scaledNumber;

        emit displayingDataCountChanged();

        if(mDisplayingDataCount > MAXIMUM_DISPLAYING_DATA_COUNT && !mSwitchDelegate)
        {
            mSwitchDelegate = true;
            emit switchDelegateChanged();
        }

        if(mDisplayingDataCount < MAXIMUM_DISPLAYING_DATA_COUNT && mSwitchDelegate)
        {
            mSwitchDelegate = false;
            emit switchDelegateChanged();
        }

        int diff = (size_t)mDisplayingDataCount + mRangeStart - mDataModel->getRange();
        if(diff>0)
        {
            std::cout << "Out of boundary, more data is required:"<< diff << std::endl;
            emit fetchMore(diff);
        }
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

    if(mRangeStart-dataStride < 0)
    {
        dataStride = mRangeStart;
    }
    mRangeStart-=dataStride;

    //mDataModel->move(dataStride, false);
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

    if(mDataModel->getRange()-rangeEnd < dataStride)
    {
        dataStride = mDataModel->getRange() - rangeEnd;
    }
    mRangeStart += dataStride;

    //mDataModel->move(dataStride, true);
    emit rangeStartPosChanged();
}
