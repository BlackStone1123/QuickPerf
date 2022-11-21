#include "ChannelDataModel.h"
#include <QDebug>
#include <cmath>
#include "DataCenter.h"
#include <algorithm>

ChannelDataModel::ChannelDataModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

ChannelDataModel::~ChannelDataModel()
{
    qDebug() << "ChannelDataModel deletion";
}

QHash<int,QByteArray> ChannelDataModel::roleNames() const 
{
    QHash<int, QByteArray> roles;
    roles[(int) ChannelDataRoles::label] = "Label";
    roles[(int) ChannelDataRoles::columnName] = "ColumnName";
    roles[(int) ChannelDataRoles::dataType] = "DataType";

    return roles;
}

QVariant ChannelDataModel::data(const QModelIndex &index, int role) const 
{
    if (index.row() < 0 || index.row() >= mRows.count())
        return QVariant();

    const ChannelDataRow& channelRow = mRows[index.row()];

    if (role == (int) ChannelDataRoles::label)
        return channelRow.key;
    else if (role == (int) ChannelDataRoles::columnName ){
        return channelRow.value;
    }
    else if (role == (int) ChannelDataRoles::dataType){
        return channelRow.type;
    }
    
    return QVariant();
}

int ChannelDataModel::rowCount(const QModelIndex & parent) const 
{
    return mRows.count();
}

void ChannelDataModel::addChannelDataBefore(size_t index, const QString& key, const QString& value, const QString& type)
{
    qDebug()<< "begin insert row at position:" << index;
    ChannelDataRow newRow = generateChannelDataRow(key, value, type);

    // update the model with new row
    beginInsertRows(QModelIndex(), index, index);
    mRows.insert(mRows.begin() + index, newRow);
    endInsertRows();
}

void ChannelDataModel::appendChannelData(const QString& key, const QString& value, const QString& type)
{
    addChannelDataBefore(mRows.count(), key, value, type);
}

void ChannelDataModel::removeChannelData(const QString& key)
{
    auto itr = std::find_if(mRows.begin(), mRows.end(),[&key](auto& _one){
        return _one.key == key;
    });

    if(itr != mRows.end())
    {
        int index = itr - mRows.begin();
        beginRemoveRows(QModelIndex(),index, index);
        mRows.erase(itr);
        endRemoveRows();
    }
}

ChannelDataRow ChannelDataModel::generateChannelDataRow(const QString& key, const QString& value, const QString& type)
{
    ChannelDataRow newRow;
    newRow.key = key;
    newRow.value = value;
    newRow.type = type;

    return newRow;
}
