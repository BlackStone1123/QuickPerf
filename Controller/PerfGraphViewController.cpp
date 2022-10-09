#include "PerfGraphViewController.h"
#include "../DataModel/ChannelDataModel.h"
#include <iostream>
#include <QPointF>

PerfGraphViewController::PerfGraphViewController(QObject* parent)
    :QObject(parent)
{
    mDataModel = new ChannelDataModel(this);
    QObject::connect(this, &PerfGraphViewController::insertRowBefore, mDataModel, &ChannelDataModel::addChannelDataBefore);
}

PerfGraphViewController::~PerfGraphViewController()
{
    std::cout << "PerfGraphViewController deletion" << std::endl;
}

void PerfGraphViewController::onWheelScaled(const QPointF& point)
{
    std::cout << "Zoom scaled for angle" << point.y()<< std::endl;

    int batchSize = mDataModel->getBatchSize();
    int scaledNumber = mDisplayingDataCount + (point.y() > 0 ? -batchSize : batchSize);
    mDisplayingDataCount = scaledNumber > 10 ? scaledNumber : 10;

    emit displayingDataCountChanged();

    if((size_t)mDisplayingDataCount > mDataModel->getRange())
    {
        std::cout << "Out of boundary, fetch more data"<< std::endl;
        mDataModel->fetchMoreData();
    }
}
