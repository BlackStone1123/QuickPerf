#include "SingleChannelController.h"
#include <iostream>
#include <QtMath>
#include <QDebug>

#include "../DataModel/RectangleViewModel.h"
#include "../DataModel/DataCenter.h"

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
    qDebug() << "begin append datas channel"
             << " column name: " << mColumnName
             << " data size:" << datasToAppend.count()
             << " pending loading:" << mPendingLoading;

    mAmplitudes.append(datasToAppend);
    emit bundleUpdated();

    if(mLoaderType == Rectangle)
    {
        mBarSetModel->setBaseOffset(mRangeStartPos);
    }
}

void SingleChannelController::loadInitialDatas()
{
    if(mGenerator == nullptr)
        return;

    // Bind the new row with data generator
    QObject::connect(mGenerator, &DataGenerator::dataLoaded, this, &SingleChannelController::onDataLoadedArrived);

    mLoaderType = mDisplayingDataCount > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;
    emit loaderTypeChanged();

    startLoading( mTotalRange );
}

void SingleChannelController::setDataGenerator(DataGenerator* gen)
{
    if(mGenerator == nullptr && gen != nullptr)
    {
        mGenerator = gen;
        mGenerator->setParent(this);
    }
}

void SingleChannelController::startLoading(size_t loadSize)
{
    if(mGenerator == nullptr)
    {
        qDebug() << "loading ended as no generator for column:" << mColumnName;
        return;
    }

    if(!mLoading)
    {
        mLoading = true;
        emit loadingUpdated();
    }

    mPendingLoading++;
    mGenerator->generate(mAmplitudes.count(), loadSize);
}

void SingleChannelController::move(int count, bool forward)
{
    if(count > 0)
    {
        mRangeStartPos = mRangeStartPos + (forward ? count : -count);
        std::cout << "range start position:"<< mRangeStartPos << " displaying data count:" << mDisplayingDataCount<< " total range:" << mTotalRange << std::endl;

        emit rangeStartPosChanged();

        updateModel();
        rebase();
    }
}

void SingleChannelController::moveTo(int pos)
{
    move(qFabs(pos - (int)mRangeStartPos) , (size_t)pos > mRangeStartPos);
}

void SingleChannelController::zoomTo(size_t count)
{
    if(mDisplayingDataCount != count)
    {
        mDisplayingDataCount = count;
        emit displayingDataCountChanged();

        LoaderType type = mDisplayingDataCount > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;
        if(type != mLoaderType)
        {
            mLoaderType = type;
            emit loaderTypeChanged();
        }

        updateModel();
        rebase();
    }
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
    if(mPendingLoading == 0)
    {
        qDebug() << "Do not have pending loading for:" << mColumnName;
        return;
    }

    mPendingLoading--;
    if(mPendingLoading == 0)
    {
        mLoading = false;
        emit loadingUpdated();
    }

    appendDatas(data.value<QList<qreal>>());
}

int SingleChannelController::getTotalDataCount() const
{
    return mGenerator->getBackEndDataSize();
}
