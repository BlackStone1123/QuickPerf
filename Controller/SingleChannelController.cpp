#include "SingleChannelController.h"
#include <iostream>

#include "../DataModel/RectangleViewModel.h"
#include "../DataModel/DataGenerator.h"

SingleChannelController::SingleChannelController(QObject* parent)
    : QObject(parent)
    , mBarSetModel(new RectangleViewModel(mAmplitudes, this))
{
}

SingleChannelController::~SingleChannelController()
{
    std::cout << "BarSetModel deletion" << std::endl;
}

void SingleChannelController::appendDatas(const QList<qreal>& datasToAppend)
{
    std::cout<< "begin append datas channel"
             << " data size:" << datasToAppend.count()
             << " pending loading:" << mPendingLoading <<std::endl;

    mAmplitudes.append(datasToAppend);
    emit bundleUpdated();

    if(mLoaderType == Rectangle)
    {
        mBarSetModel->setBaseOffset(mRangeStartPos);
    }
}

void SingleChannelController::loadInitialDatas()
{
    mAmplitudes = mGenerator->generate(mTotalRange ,true).value<QList<qreal>>();
    mLoaderType = mDisplayingDataCount > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;
    mBarSetModel->setBaseOffset(mRangeStartPos);
}

void SingleChannelController::setDataGenerator(DataGenerator* gen)
{
    // Bind the new row with data generator
    QObject::connect(gen, &DataGenerator::dataLoadFinished, this, &SingleChannelController::onDataLoadedArrived);
    mGenerator = gen;
}

void SingleChannelController::startLoading(size_t loadSize)
{
    if(!mLoading)
    {
        mLoading = true;
        emit loadingUpdated();
    }

    mPendingLoading++;
    mGenerator->generate(loadSize);
}

void SingleChannelController::move(size_t count, bool forward)
{
    if(count > 0)
    {
        mRangeStartPos = mRangeStartPos + (forward ? count : -count);
        emit rangeStartPosChanged();
    }

    if(mLoaderType == Rectangle)
    {
        bool rebase = forward ? (mDisplayingDataCount > MAXIMUM_RECTANGLE_DATA_COUNT - (mRangeStartPos - mRectViewBaseOffset))
                              : mRangeStartPos < mRectViewBaseOffset;

        if(rebase)
        {
            mRectViewBaseOffset = mRangeStartPos;
            emit rectBaseOffsetChanged();

            mBarSetModel->setBaseOffset(mRectViewBaseOffset);
        }
    }
    updateModel();
}

void SingleChannelController::zoomTo(size_t count)
{
    if(mDisplayingDataCount != count)
    {
        mDisplayingDataCount = count;
        emit displayingDataCountChanged();
    }

    LoaderType type = mDisplayingDataCount > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;
    if(type != mLoaderType)
    {
        mLoaderType = type;

        if(mLoaderType == Rectangle)
        {
            mRectViewBaseOffset = mRangeStartPos;
            emit rectBaseOffsetChanged();

            mBarSetModel->setBaseOffset(mRangeStartPos);
        }
        emit loaderTypeChanged();
    }

    updateModel();
}

size_t SingleChannelController::requestForMoveStride(size_t preferSize, bool forward)
{
    size_t backendSize = getDataGenerator()->getBackEndDataSize();
    size_t residual = forward ? (backendSize - mRangeStartPos - mDisplayingDataCount) : mRangeStartPos;

    return std::min(residual, preferSize);
}

size_t SingleChannelController::requestForZoomStride(size_t count)
{
    size_t backendSize = getDataGenerator()->getBackEndDataSize();
    return std::min(count, backendSize - mRangeStartPos);
}

void SingleChannelController::fetchMoreData(size_t count)
{
    size_t batchNum = std::ceil((float)count/mBatchSize);
    size_t loadSize = mBatchSize * batchNum;
    std::cout<< "Ready to load data count:" << loadSize << std::endl;

    mTotalRange += loadSize;

    startLoading(loadSize);
}

void SingleChannelController::updateModel()
{
    int diff = mDisplayingDataCount + mRangeStartPos - mTotalRange;
    if(diff>0)
    {
        std::cout << "Out of boundary, more data is required:"<< diff << std::endl;
        fetchMoreData(diff);
    }
}

void SingleChannelController::onDataLoadedArrived(const QVariant& data)
{
    mPendingLoading--;
    if(mPendingLoading == 0)
    {
        mLoading = false;
        emit loadingUpdated();
    }

    appendDatas(data.value<QList<qreal>>());
}
