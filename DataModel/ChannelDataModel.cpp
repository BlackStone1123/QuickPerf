#include "ChannelDataModel.h"
#include <iostream>
#include "DataGenerator.h"
#include <cmath>

////////////////////////////////////////////////////////////////////////////
BarSetModel::BarSetModel(const QList<qreal>& datas, QObject* parent)
    : QAbstractListModel(parent)
    , mDatas(datas)
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
    if (index.row() < 0 || index.row() >= MAXIMUM_RECTANGLE_DATA_COUNT)
        return QVariant();

    if (role == (int) BarSetModelDataRoles::Amplitude)
        return mDatas[index.row() + mBaseOffset];

    return QVariant();
}

int BarSetModel::rowCount(const QModelIndex & parent) const
{
    return MAXIMUM_RECTANGLE_DATA_COUNT;
}

void BarSetModel::setBaseOffset(size_t baseOffset)
{
    if(mBaseOffset != baseOffset)
    {
        mBaseOffset = baseOffset;
        emit baseOffsetChanged();
        emit dataChanged(index(0), index(MAXIMUM_RECTANGLE_DATA_COUNT - 1));
    }
}
////////////////////////////////////////////////////////////////////////////
PointSetModel::PointSetModel(QObject* parent)
    : QObject(parent)
{
}

PointSetModel::~PointSetModel()
{
    std::cout << "BarSetModel deletion" << std::endl;
}

void PointSetModel::appendDatas(const QList<qreal>& datasToAppend)
{
    int index = -1;
    emit getPeresistentIndex(this, &index);
    std::cout<< "begin append datas channel index:"<< index
             << " data size:" << datasToAppend.count()
             << " pending loading:" << mPendingLoading <<std::endl;

    mAmplitudes.append(datasToAppend);
    emit bundleUpdated();

    if(mLoaderType == Rectangle)
    {
        mBarSetModel->setBaseOffset(mRangStartPos);
    }
}

void PointSetModel::setInitialDatas(const QList<qreal>& datas)
{
    mAmplitudes = datas;
}

void PointSetModel::setDataGenerator(QPointer<DataGenerator> gen)
{
    // Bind the new row with data generator
    QObject::connect(gen, &DataGenerator::dataLoadFinished, this, &PointSetModel::onDataLoadedArrived);
    mGenerator = gen;
}

void PointSetModel::startLoading(size_t loadSize)
{
    if(!mLoading)
    {
        mLoading = true;
        emit loadingUpdated();
    }

    mPendingLoading++;
    mGenerator->generate(loadSize);
}

void PointSetModel::onDataLoadedArrived(const QVariant& data)
{
    mPendingLoading--;
    if(mPendingLoading == 0)
    {
        mLoading = false;
        emit loadingUpdated();
    }

    appendDatas(data.value<QList<qreal>>());
}

void PointSetModel::onDisplayingDataCountChanged(size_t count)
{
    mDisplayingRange = count;
    LoaderType type = count > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;

    if(type != mLoaderType)
    {
        mLoaderType = type;

        if(mLoaderType == Rectangle)
        {
            if(mBarSetModel == nullptr)
            {
                mBarSetModel = new BarSetModel(mAmplitudes, this);
            }
            mBarSetModel->setBaseOffset(mRangStartPos);
        }
        emit loaderTypeChanged();
    }
}

void PointSetModel::onDisplayingPositionChanged(size_t pos)
{
    mRangStartPos = pos;

    if(mLoaderType == Rectangle)
    {
        if(mBarSetModel == nullptr)
        {
            std::cout << "loader type is rectangle, but model is not exist ??" << std::endl;
            return;
        }

        if(mRangStartPos - mBarSetModel->getBaseOffset() >= MAXIMUM_RECTANGLE_DATA_COUNT - mDisplayingRange)
        {
            mBarSetModel->setBaseOffset(mRangStartPos);
        }
    }
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
    roles[(int) ChannelDataRoles::PointSetModel] = "PointSetModel";
    roles[(int) ChannelDataRoles::Color] = "Color";

    return roles;
}

QVariant ChannelDataModel::data(const QModelIndex &index, int role) const 
{
    if (index.row() < 0 || index.row() >= mRows.count())
        return QVariant();

    const ChannelDataRow& channelRow = mRows[index.row()];

    if (role == (int) ChannelDataRoles::PointSetModel)
        return QVariant::fromValue<PointSetModel*>(channelRow.pointSetModel);
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
    size_t backendSize = mRows.front().pointSetModel->getDataGenerator()->getBackEndDataSize();
    size_t residual = forward ? (backendSize - mRangStartPos - mDisplayingRange) : mRangStartPos;

    return std::min(residual, preferSize);
}

size_t ChannelDataModel::requestForZoomStride(size_t count)
{
    size_t backendSize = mRows.front().pointSetModel->getDataGenerator()->getBackEndDataSize();
    return std::min(count, backendSize - mRangStartPos);
}

ChannelDataRow ChannelDataModel::generateChannelDataRow(size_t index, QPointer<DataGenerator> generator)
{
    ChannelDataRow newRow;

    newRow.pointSetModel = new PointSetModel(this);
    newRow.pointSetModel->setInitialDatas(generator->generate(mTotalRange, true).value<QList<qreal>>());
    newRow.pointSetModel->setDataGenerator(generator);

    QObject::connect(newRow.pointSetModel, &PointSetModel::getPeresistentIndex, this, &ChannelDataModel::getPointSetModelIndex);
    QObject::connect(this, &ChannelDataModel::displayingDataCountChanged, newRow.pointSetModel, &PointSetModel::onDisplayingDataCountChanged);
    QObject::connect(this, &ChannelDataModel::displayingPositionChanged, newRow.pointSetModel, &PointSetModel::onDisplayingPositionChanged);

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
    for (auto _one : mRows)
    {
        _one.pointSetModel->startLoading(loadSize);
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

void ChannelDataModel::getPointSetModelIndex(void* item, int* res)
{
    if(item)
    {
        for (int i = 0; i< mRows.count(); i++)
        {
            if(mRows[i].pointSetModel == item)
            {
                *res = i;
            }
        }
    }
    return;
}
