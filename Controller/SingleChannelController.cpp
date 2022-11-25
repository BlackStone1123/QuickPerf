#include "SingleChannelController.h"
#include <iostream>
#include <QtMath>
#include <QDebug>

#include "../DataModel/RectangleViewModel.h"
#include "../DataModel/DataCenter.h"
////////////////////////////////////////////////////////////////////////
CycleRangeConverter::CycleRangeConverter(int scale, int start, int range, QObject* parent)
    : QObject(parent)
    , mRangeStartPos(start)
    , mDisplayingDataCount(range)
    , mScale(scale)
{
    if(mScale == 1)
    {
        mCycleDisplayingCount = mDisplayingDataCount;
    }
}

void CycleRangeConverter::updateRangeStartPos(int count, bool forward)
{
    int pos = mCycleRangeStartPos + (forward ? count : - count);
    mCycleRangeStartPos = pos;
    emit cycleRangeStartPosChanged();

    mRangeStartPos = pos / mScale;
    mRangeStartOffset = pos % mScale;
}

void CycleRangeConverter::integralMove(int count, bool forward)
{
    if(count > 0)
    {
        updateRangeStartPos(count, forward);
        emit rangeStartPosChanged();
        emit rangeStartOffsetChanged();
    }
}

void CycleRangeConverter::integralMoveTo(int pos)
{
    integralMove(qAbs(pos - mCycleRangeStartPos) , pos > mCycleRangeStartPos);
}

void CycleRangeConverter::boundaryMove(int count, bool left, bool forward)
{
    if(count != 0)
    {
        if(left)
        {
            updateRangeStartPos(count, forward);
            zoomTo(mCycleDisplayingCount + (forward ? -count : count));

            emit rangeStartPosChanged();
            emit rangeStartOffsetChanged();
        }
        else {
            zoomTo(mCycleDisplayingCount + (forward ? count : -count));
        }
    }
}

void CycleRangeConverter::zoomTo(int count)
{
    mCycleDisplayingCount = count;
    emit displayingCycleCountChanged();

    int displayingCount = qCeil((double) count / mScale);

    if(mDisplayingDataCount != displayingCount)
    {
        mDisplayingDataCount = displayingCount;
        emit displayingDataCountChanged();
    }
}

////////////////////////////////////////////////////////////////////////
SingleChannelController::SingleChannelController(QObject* parent)
    : QObject(parent)
{
}

SingleChannelController::~SingleChannelController()
{
}

void SingleChannelController::appendDatas(const QList<qreal>& datasToAppend)
{
    qDebug() << "begin append datas channel"
             << " key: " << mKey
             << " data size:" << datasToAppend.count()
             << " pending loading:" << mPendingLoading;

    mAmplitudes.append(datasToAppend);
    emit bundleUpdated();
}

void SingleChannelController::updatePinStatus(bool pined, bool fromUI)
{
    if(pined != mPinding)
    {
        mPinding = pined;

        if(!fromUI)
        {
            emit pindingUpdated();
        }
    }
}

void SingleChannelController::loadInitialDatas(const InitialStatus& status)
{
    if(mGenerator == nullptr)
        return;

    // Bind the new row with data generator
    QObject::connect(mGenerator, &DataGenerator::dataLoaded, this, &SingleChannelController::onDataLoadedArrived);
    QObject::connect(mCycleRangeConverter, &CycleRangeConverter::rangeStartPosChanged, this, &SingleChannelController::onRangeStartPosChanged);
    QObject::connect(mCycleRangeConverter, &CycleRangeConverter::displayingDataCountChanged, this, &SingleChannelController::onDisplayingDataCountChanged);

    int scale = mCycleRangeConverter->getScale();
    int scaledTotalRange = qCeil((double)status.totalCycleRange / scale);

    mPinding = status.pinding;
    emit pindingUpdated();

    mTotalRange = scaledTotalRange;
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

void SingleChannelController::setRangeConverter(CycleRangeConverter* converter)
{
    if(mCycleRangeConverter == nullptr && converter != nullptr)
    {
        mCycleRangeConverter = converter;
        mCycleRangeConverter->setParent(this);
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
        mCycleRangeConverter->integralMove(count, forward);
    }
}

void SingleChannelController::integralMoveTo(int pos)
{
    if(pos >= 0)
    {
        mCycleRangeConverter->integralMoveTo(pos);
    }
}

void SingleChannelController::sliderMove(int count, bool left, bool forward)
{
    if(count != 0)
    {
        mCycleRangeConverter->boundaryMove(count, left, forward);
    }
}

void SingleChannelController::zoomTo(size_t count)
{
    mCycleRangeConverter->zoomTo(count);
}

void SingleChannelController::fetchMoreData(size_t count)
{
    size_t batchNum = std::ceil((float)count/mBatchSize);
    size_t loadSize = mBatchSize * batchNum;

    loadSize = std::min(loadSize, getTotalDataCount() - mTotalRange);
    qDebug()<< "Ready to load data count:" << loadSize;

    if(loadSize > 0)
    {
        mTotalRange += loadSize;

        startLoading(loadSize);
    }
}

void SingleChannelController::updateModel()
{
    int rangeStart = mCycleRangeConverter->getConvertedRangeStart();
    int displayingCount = mCycleRangeConverter->getConvertedDisplayingRange();
    int diff = displayingCount + rangeStart - mTotalRange;

    if(diff>0)
    {
        qDebug() << "Out of boundary, more data is required:"<< diff;
        fetchMoreData(diff);
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

void SingleChannelController::onRangeStartPosChanged()
{
    updateModel();
}

void SingleChannelController::onDisplayingDataCountChanged()
{
    updateModel();
}

int SingleChannelController::getTotalDataCount() const
{
    return mGenerator->getBackEndDataSize();
}
