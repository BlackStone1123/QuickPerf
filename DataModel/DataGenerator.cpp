#include "DataGenerator.h"
#include <iostream>
#include <QMutexLocker>

static unsigned long rseed = 10;

DataGenerator::DataGenerator(QObject* parent)
    : QThread(parent)
{
    QObject::connect(this, &QThread::finished, this, [this](){
        this->mGenReq.clear();
    });
}

DataGenerator::~DataGenerator()
{
}

QVariant DataGenerator::generate(size_t number, bool immediate)
{
    if(immediate)
        return kernelFunc(number);

    //QMutexLocker locker(&mMutex);
    //mCount = number;

    if(!isRunning())
    {
        mGenReq << number;
        start(LowPriority);
    }
    else {
        QMutexLocker locker(&mMutex);
        mGenReq << number;
    }

    return {};
}

void DataGenerator::run()
{
//    mMutex.lock();
//    int totalCount = this->mCount;
//    mMutex.unlock();

//    sleep(3);
//    QVariant res = kernelFunc(totalCount);
//    emit dataLoadFinished(res);

    size_t batchExecuted = 0;
    size_t totalbatchCount = 0;
    size_t currentBatchNum = 0;
    bool taskAbort = false;

    while(true)
    {
        mMutex.lock();
        totalbatchCount = this->mGenReq.count();
        currentBatchNum = this->mGenReq.back();
        taskAbort = this->mAbort;
        mMutex.unlock();

        if(totalbatchCount == batchExecuted || taskAbort)
        {
            break;
        }
        else
        {
            QVariant res = kernelFunc(currentBatchNum);
            emit dataLoadFinished(res);
            batchExecuted = totalbatchCount;
        }
    }
}

void DataGenerator::exit()
{
    mMutex.lock();
    mAbort = true;
    //mCondition.wakeOne();
    mMutex.unlock();

    wait();
}

////////////////////////////////////////////////////////////////////
QVariant RandomDataGenerator::kernelFunc(size_t number)
{
    sleep(3);
    QList<float> res;

    std::normal_distribution<float> distribution(40.0, 10.0);

    for (size_t j = 0; j < number; j++)
    {
        res << distribution(mRandomEng);
    }

    return QVariant::fromValue(res);
}

RandomDataGenerator::~RandomDataGenerator()
{
    DataGenerator::exit();
    std::cout << "random data gen deletion!" << std::endl;
}

RandomDataGenerator::RandomDataGenerator(QObject* parent)
    : DataGenerator(parent)
{
    mRandomEng.seed( rseed );
    rseed *= 100;
}
