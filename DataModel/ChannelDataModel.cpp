#include "ChannelDataModel.h"
#include <iostream>
#include "DataGenerator.h"
#include <cmath>
////////////////////////////////////////////////////////////////////////////
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
    int index = -1;
    emit getPeresistentIndex(this, &index);
    std::cout<< "begin append datas channel index:"<< index << " data size:" << datasToAppend.count() << std::endl;

    auto endPos = m_Amplitudes.count();

    beginInsertRows(QModelIndex(), endPos, endPos + datasToAppend.count() -1);
    m_Amplitudes.append(datasToAppend);
    endInsertRows();
}

void BarSetModel::setInitialDatas(const QList<float>& datas)
{
    m_Amplitudes = datas;
}

void BarSetModel::onDataLoadedArrived(const QVariant& data)
{
    appendDatas(data.value<QList<float>>());
}

void BarSetModel::setDataGenerator(QPointer<DataGenerator> gen)
{
    // Bind the new row with data generator
    QObject::connect(gen, &DataGenerator::dataLoadFinished, this, &BarSetModel::onDataLoadedArrived);
    mGenerator = gen;
}

////////////////////////////////////////////////////////////////////////////
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

void ChannelDataModel::addChannelDataBefore(size_t index, QPointer<DataGenerator> generator)
{
    std::cout<< "begin insert row at position:" << index << std::endl;
    ChannelDataRow newRow = generateChannelDataRow(index, generator);

    // update the model with new row
    beginInsertRows(QModelIndex(), index, index);
    mRows.insert(mRows.begin() + index, newRow);
    endInsertRows();
}

ChannelDataRow ChannelDataModel::generateChannelDataRow(size_t index, QPointer<DataGenerator> generator)
{
    ChannelDataRow newRow;

    newRow.barSetModel = new BarSetModel(this);
    newRow.barSetModel->setInitialDatas(generator->generate(mRange, true).value<QList<float>>());
    newRow.barSetModel->setDataGenerator(generator);

    QObject::connect(newRow.barSetModel, &BarSetModel::getPeresistentIndex, this, &ChannelDataModel::getBarSetModelIndex);

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

void ChannelDataModel::fetchMoreData(size_t count)
{
    size_t batchNum = std::ceil((float)count/mBatchSize);
    size_t loadSize = mBatchSize * batchNum;
    std::cout<< "Ready to load data count:" << loadSize << std::endl;

    for (auto _one : mRows)
    {
        _one.barSetModel->getDataGenerator()->generate(loadSize);
    }
    mRange += loadSize;
    emit rangeChanged();
}

void ChannelDataModel::getBarSetModelIndex(void* item, int* res)
{
    if(item)
    {
        for (int i = 0; i< mRows.count(); i++)
        {
            if(mRows[i].barSetModel == item)
            {
                *res = i;
            }
        }
    }
    return;
}
