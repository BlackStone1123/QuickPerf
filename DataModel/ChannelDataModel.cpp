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
    if (index.row() < 0 || index.row() >= mAmplitudes.count())
        return QVariant();

    if (role == (int) BarSetModelDataRoles::Amplitude)
        return mAmplitudes[index.row()];

    return QVariant();
}

int BarSetModel::rowCount(const QModelIndex & parent) const
{
    //return std::min(mAmplitudes.count(), MAXIMUM_BAR_SET_DATA_COUNT);
    return mAmplitudes.count();
}

void BarSetModel::appendDatas(const QList<qreal>& datasToAppend)
{
    int index = -1;
    emit getPeresistentIndex(this, &index);
    std::cout<< "begin append datas channel index:"<< index
             << " data size:" << datasToAppend.count()
             << " pending loading:" << mPendingLoading <<std::endl;

    auto endPos = mAmplitudes.count();

    beginInsertRows(QModelIndex(), endPos, endPos + datasToAppend.count() -1);
    mAmplitudes.append(datasToAppend);
    endInsertRows();

    emit bundleUpdated();
}

void BarSetModel::setInitialDatas(const QList<qreal>& datas)
{
    mAmplitudes = datas;
}

void BarSetModel::onDataLoadedArrived(const QVariant& data)
{
    mPendingLoading--;
    if(mPendingLoading == 0)
    {
        mLoading = false;
        emit loadingUpdated();
    }

    appendDatas(data.value<QList<qreal>>());
}

void BarSetModel::setDataGenerator(QPointer<DataGenerator> gen)
{
    // Bind the new row with data generator
    QObject::connect(gen, &DataGenerator::dataLoadFinished, this, &BarSetModel::onDataLoadedArrived);
    mGenerator = gen;
}

void BarSetModel::startLoading(size_t loadSize)
{
    if(!mLoading)
    {
        mLoading = true;
        emit loadingUpdated();
    }

    mPendingLoading++;
    mGenerator->generate(loadSize);
}

void BarSetModel::onDisplayingDataCountChanged(size_t count)
{
    LoaderType type = count > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;

    if(type != mLoaderType)
    {
        mLoaderType = type;
        emit loaderTypeChanged();
    }
}

void BarSetModel::onDisplayingPositionChanged(size_t pos)
{

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

void ChannelDataModel::move(size_t count, bool forward)
{
    if(count > 0)
    {
        mRangStartPos = mRangStartPos + (forward ? count : -count);
        emit displayingPositionChanged(mRangStartPos);
    }

    updateModel();
}

void ChannelDataModel::zoomTo(size_t count)
{
    if(mDisplayingRange != count)
    {
        mDisplayingRange = count;
        emit displayingDataCountChanged(mDisplayingRange);
    }

    updateModel();
}

size_t ChannelDataModel::requestForMoveStride(size_t preferSize, bool forward)
{
    size_t backendSize = mRows.front().barSetModel->getDataGenerator()->getBackEndDataSize();
    size_t residual = forward ? (backendSize - mRangStartPos - mDisplayingRange) : mRangStartPos;

    return std::min(residual, preferSize);
}

size_t ChannelDataModel::requestForZoomStride(size_t count)
{
    size_t backendSize = mRows.front().barSetModel->getDataGenerator()->getBackEndDataSize();
    return std::min(count, backendSize - mRangStartPos);
}

ChannelDataRow ChannelDataModel::generateChannelDataRow(size_t index, QPointer<DataGenerator> generator)
{
    ChannelDataRow newRow;

    newRow.barSetModel = new BarSetModel(this);
    newRow.barSetModel->setInitialDatas(generator->generate(mTotalRange, true).value<QList<qreal>>());
    newRow.barSetModel->setDataGenerator(generator);

    QObject::connect(newRow.barSetModel, &BarSetModel::getPeresistentIndex, this, &ChannelDataModel::getBarSetModelIndex);
    QObject::connect(this, &ChannelDataModel::displayingDataCountChanged, newRow.barSetModel, &BarSetModel::onDisplayingDataCountChanged);
    QObject::connect(this, &ChannelDataModel::displayingPositionChanged, newRow.barSetModel, &BarSetModel::onDisplayingPositionChanged);

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

    mTotalRange += loadSize;
    emit totalRangeChanged();

    for (auto _one : mRows)
    {
        _one.barSetModel->startLoading(loadSize);
    }
}

void ChannelDataModel::updateModel()
{
    int diff = mDisplayingRange + mRangStartPos - mTotalRange;
    if(diff>0)
    {
        std::cout << "Out of boundary, more data is required:"<< diff << std::endl;
        fetchMoreData(diff);
    }
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
