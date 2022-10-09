#include "ChannelDataModel.h"
#include <iostream>

ChannelDataModel::ChannelDataModel(QObject* parent)
    : QAbstractListModel(parent)
{
    for (size_t i = 0; i < 10; i++)
    {
        mRows.append( generateRandomDataRow(i));
    }
    
}

ChannelDataModel::~ChannelDataModel()
{
    std::cout << "ChannelDataModel deletion" << std::endl;
}

QHash<int,QByteArray> ChannelDataModel::roleNames() const 
{
    QHash<int, QByteArray> roles;
    roles[(int) ChannelDataRoles::BarSetModel] = "BarSetModel";
    roles[(int) ChannelDataRoles::Color] = "Color";

    return roles;
}

QVariant ChannelDataModel::data(const QModelIndex &index, int role) const 
{
    if (index.row() < 0 || index.row() >= mRows.count())
        return QVariant();

    const ChannelDataRow& channelRow = mRows[index.row()];

    if (role == (int) ChannelDataRoles::BarSetModel)
        return QVariant::fromValue<BarSetModel*>(channelRow.barSetModel);
    else if (role == (int) ChannelDataRoles::Color ){
        return channelRow.color;
    }
    
    
    return QVariant();
}

int ChannelDataModel::rowCount(const QModelIndex & parent) const 
{
    return mRows.count();
}

void ChannelDataModel::addChannelDataBefore(size_t index)
{
    auto endPos = index;
    std::cout<< "begin insert row at position:" << endPos << std::endl;

    beginInsertRows(QModelIndex(), endPos, endPos);
    mRows.insert(mRows.begin() + index, generateRandomDataRow(endPos));
    endInsertRows();
}

ChannelDataRow ChannelDataModel::generateRandomDataRow(size_t index)
{
    ChannelDataRow row;
    row.barSetModel = new BarSetModel(this);
    row.barSetModel->setInitialDatas(mGenerator.generate(1000));

    switch (index % 4)
    {
    case 0:
        row.color = Qt::red;
        break;
    case 1:
        row.color = Qt::green;
        break;
    case 2:
        row.color = Qt::blue;
        break;
    case 3:
        row.color = Qt::yellow;
        break;
    default:
        break;
    }
    return row;
}

//////////////////////////////////////////////////////////////////////////
BarSetModel::BarSetModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

BarSetModel::~BarSetModel()
{
    std::cout << "BarSetModel deletion" << std::endl;
}

QHash<int,QByteArray> BarSetModel::roleNames() const 
{
    QHash<int, QByteArray> roles;
    roles[(int) BarSetModelDataRoles::Amplitude] = "Amplitude";

    return roles;
}

QVariant BarSetModel::data(const QModelIndex &index, int role) const 
{
    if (index.row() < 0 || index.row() >= m_Amplitudes.count())
        return QVariant();

    if (role == (int) BarSetModelDataRoles::Amplitude)
        return m_Amplitudes[index.row()];
    
    return QVariant();
}

int BarSetModel::rowCount(const QModelIndex & parent) const 
{
    return m_Amplitudes.count();
}

void BarSetModel::appendDatas(const QList<float>& datasToAppend)
{
    auto endPos = m_Amplitudes.count();
    std::cout<< "begin append datas"<< std::endl;

    beginInsertRows(QModelIndex(), endPos, endPos + datasToAppend.count() -1);
    m_Amplitudes.append(datasToAppend);
    endInsertRows();
}

void BarSetModel::setInitialDatas(const QList<float>& datas)
{
    m_Amplitudes = datas;
}
