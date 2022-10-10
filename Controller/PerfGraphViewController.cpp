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
    for(int i = 0; i < 10; i++)
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

    int diff = (size_t)mDisplayingDataCount - mDataModel->getRange();
    if(diff>0)
    {
        std::cout << "Out of boundary, more data is required:"<< diff << std::endl;
        emit fetchMore(mDisplayingDataCount-mDataModel->getRange());
    }
}
