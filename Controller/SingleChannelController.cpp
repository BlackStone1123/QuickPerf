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
    std::cout << "SingleChannelController deletion" << std::endl;
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
    // Bind the new row with data generator
    QObject::connect(mGenerator, &DataGenerator::dataLoadFinished, this, &SingleChannelController::onDataLoadedArrived);

    mLoaderType = mDisplayingDataCount > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;
    emit loaderTypeChanged();

    mAmplitudes = mGenerator->generate(mTotalRange ,true).value<QList<qreal>>();
    emit bundleUpdated();

    mBarSetModel->setBaseOffset(mRangeStartPos);
}

void SingleChannelController::setDataGenerator(DataGenerator* gen)
{
    if(mGenerator == nullptr)
    {
        mGenerator = gen;
    }
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

void SingleChannelController::move(int count, bool forward)
{
    if(count > 0)
    {
        mRangeStartPos = mRangeStartPos + (forward ? count : -count);
        std::cout << "range start position:"<< mRangeStartPos << " displaying data count:" << mDisplayingDataCount<< " total range:" << mTotalRange << std::endl;

        emit rangeStartPosChanged();
    }

    updateModel();
    rebase();
}

void SingleChannelController::zoomTo(size_t count)
{
    if(mDisplayingDataCount != count)
    {
        mDisplayingDataCount = count;
        //std::cout << "range start position:"<< mRangeStartPos << " displaying data count:" << mDisplayingDataCount<< " total range:" << mTotalRange << std::endl;

        emit displayingDataCountChanged();
    }

    LoaderType type = mDisplayingDataCount > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;
    if(type != mLoaderType)
    {
        mLoaderType = type;
        emit loaderTypeChanged();
    }

    updateModel();
    rebase();
}

int SingleChannelController::requestForMoveStride(size_t preferSize, bool forward)
{
    size_t backendSize = getDataGenerator()->getBackEndDataSize();
    size_t residual = forward ? (backendSize - mRangeStartPos - mDisplayingDataCount) : mRangeStartPos;

    return std::min(residual, preferSize);
}

int SingleChannelController::requestForZoomStride(size_t count)
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

void SingleChannelController::rebase()
{
    if(mLoaderType == Rectangle)
    {
        size_t baseOffset = mBarSetModel->getBaseOffset();
        bool rebase = (MAXIMUM_RECTANGLE_DATA_COUNT < (mRangeStartPos - baseOffset)) ||
                (mDisplayingDataCount > MAXIMUM_RECTANGLE_DATA_COUNT - (mRangeStartPos - baseOffset))||
                mRangeStartPos < baseOffset;

        if(rebase)
        {
            std::cout << "Rebase rectangle model offset:"<< mRangeStartPos << std::endl;
            mBarSetModel->setBaseOffset(mRangeStartPos);
        }
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
