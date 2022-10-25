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
    qDebug() << "SingleChannelController deletion";
}

void SingleChannelController::appendDatas(const QList<qreal>& datasToAppend)
{
    qDebug() << "begin append datas channel"
             << " key: " << mKey
             << " data size:" << datasToAppend.count()
             << " pending loading:" << mPendingLoading;

    mAmplitudes.append(datasToAppend);
    emit bundleUpdated();

    if(mLoaderType == Rectangle)
    {
        mBarSetModel->setBaseOffset(mRangeStartPos);
    }
}

void SingleChannelController::loadInitialDatas(int count)
{
    if(mGenerator == nullptr)
        return;

    // Bind the new row with data generator
    QObject::connect(mGenerator, &DataGenerator::dataLoaded, this, &SingleChannelController::onDataLoadedArrived);

    mLoaderType = mDisplayingDataCount > MAXIMUM_RECTANGLE_DISPLAYING_DATA_COUNT ? PointSet : Rectangle;
    emit loaderTypeChanged();

    mTotalRange = count > 0 ? count : mTotalRange;
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
        qDebug() << "loading ended as no generator for column:" << mKey;
        return;
    }

    if(!mLoading)
    {
        mLoading = true;
        emit loadingUpdated();
    }

    mPendingLoading++;
    mGenerator->generate(loadSize);
}

void SingleChannelController::integralMove(int count, bool forward)
{
    if(count > 0)
    {
        mRangeStartPos = mRangeStartPos + (forward ? count : -count);
//        qDebug()<<"key: "<< mKey
//                << "range start position:"<< mRangeStartPos
//                << " displaying data count:" << mDisplayingDataCount
//                << " total range:" << mTotalRange;

        emit rangeStartPosChanged();

        updateModel();
        rebase();
    }
}

void SingleChannelController::integralMoveTo(int pos)
{
    integralMove(qFabs(pos - (int)mRangeStartPos) , (size_t)pos > mRangeStartPos);
}

void SingleChannelController::sliderMove(int count, bool left, bool forward)
{
    if(count != 0)
    {
        if(left)
        {
            mRangeStartPos = mRangeStartPos + (forward ? count : -count);
            emit rangeStartPosChanged();

            zoomTo(mDisplayingDataCount + (forward ? -count : count));
        }
        else {
            zoomTo(mDisplayingDataCount + (forward ? count : -count));
        }
    }
}

void SingleChannelController::zoomTo(size_t count)
{
    if(mDisplayingDataCount != count)
    {
        mDisplayingDataCount = count;
        emit displayingDataCountChanged();

//        qDebug()<<"key: "<< mKey
//                << "range start position:"<< mRangeStartPos
//                << " displaying data count:" << mDisplayingDataCount
//                << " total range:" << mTotalRange;

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
    return std::min(count, backendSize);
}

void SingleChannelController::fetchMoreData(size_t count)
{
    size_t batchNum = std::ceil((float)count/mBatchSize);
    size_t loadSize = mBatchSize * batchNum;

    loadSize = std::min(loadSize, getTotalDataCount() - mTotalRange);
    qDebug()<< "Ready to load data count:" << loadSize;

    mTotalRange += loadSize;

    startLoading(loadSize);
}

void SingleChannelController::updateModel()
{
    int diff = mDisplayingDataCount + mRangeStartPos - mTotalRange;
    if(diff>0)
    {
        qDebug() << "Out of boundary, more data is required:"<< diff;
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
            qDebug() << "Rebase rectangle model offset:"<< mRangeStartPos;
            mBarSetModel->setBaseOffset(mRangeStartPos);
        }
    }
}

void SingleChannelController::onDataLoadedArrived(const QVariant& data)
{
    if(mPendingLoading == 0)
    {
        qDebug() << "Do not have pending loading for:" << mKey;
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
