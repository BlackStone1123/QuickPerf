#include "PerfGraphViewController.h"
#include "../DataModel/ChannelDataModel.h"
#include <iostream>

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
