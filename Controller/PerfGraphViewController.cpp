#include "PerfGraphViewController.h"
#include "../DataModel/ChannelDataModel.h"
#include <iostream>
#include <QPointF>

namespace  {
    const int minimumDisplayingCount{10};
}

PerfGraphViewController::PerfGraphViewController(QObject* parent)
    :QObject(parent)
{
    mDataModel = new ChannelDataModel(this);
    QObject::connect(this, &PerfGraphViewController::insertRowBefore, mDataModel, &ChannelDataModel::addChannelDataBefore);
    QObject::connect(this, &PerfGraphViewController::fetchMore, mDataModel, &ChannelDataModel::fetchMoreData, Qt::QueuedConnection);
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
