#include "ChannelDataModel.h"
#include <iostream>
#include <cmath>
#include "DataGenerator.h"

ChannelDataModel::ChannelDataModel(GeneratorList generators, QObject* parent)
    : QAbstractListModel(parent)
{
    for (size_t i = 0; i < (size_t)generators.count(); i++)
    {
        mRows.append( generateChannelDataRow(i, generators[i]));
    }
    
}

ChannelDataModel::~ChannelDataModel()
{
    std::cout << "ChannelDataModel deletion" << std::endl;
}

QHash<int,QByteArray> ChannelDataModel::roleNames() const 
{
    QHash<int, QByteArray> roles;
    roles[(int) ChannelDataRoles::Generator] = "Generator";
    roles[(int) ChannelDataRoles::Color] = "Color";

    return roles;
}

QVariant ChannelDataModel::data(const QModelIndex &index, int role) const 
{
    if (index.row() < 0 || index.row() >= mRows.count())
        return QVariant();

    const ChannelDataRow& channelRow = mRows[index.row()];

    if (role == (int) ChannelDataRoles::Generator)
        return QVariant::fromValue<DataGenerator*>(channelRow.generator);
    else if (role == (int) ChannelDataRoles::Color ){
        return channelRow.color;
    }
    
    
    return QVariant();
}

int ChannelDataModel::rowCount(const QModelIndex & parent) const 
{
    return mRows.count();
}

void ChannelDataModel::addChannelDataBefore(size_t index, QPointer<DataGenerator> generator)
{
    std::cout<< "begin insert row at position:" << index << std::endl;
    ChannelDataRow newRow = generateChannelDataRow(index, generator);

    // update the model with new row
    beginInsertRows(QModelIndex(), index, index);
    mRows.insert(mRows.begin() + index, newRow);
    endInsertRows();
}

ChannelDataRow ChannelDataModel::generateChannelDataRow(size_t index, QPointer<DataGenerator> dataGenerator)
{
    ChannelDataRow newRow;
    newRow.generator = dataGenerator;

    switch (index % 4)
    {
    case 0:
        newRow.color = Qt::red;
        break;
    case 1:
        newRow.color = Qt::green;
        break;
    case 2:
        newRow.color = Qt::blue;
        break;
    case 3:
        newRow.color = Qt::yellow;
        break;
    default:
        break;
    }

    return newRow;
}
