#include "DataGenerator.h"
#include <iostream>
#include <QMutexLocker>

static unsigned long rseed = 10;

DataGenerator::DataGenerator(QObject* parent)
    : QThread(parent)
{
    QObject::connect(this, &QThread::finished, this, [](){
        std::cout << "the worker thread finished" << std::endl;
    });
}

DataGenerator::~DataGenerator()
{
}

QVariant DataGenerator::generate(size_t number, bool immediate)
{
    if(immediate)
        return kernelFunc(number);

    if(!isRunning())
    {
        mGenReq.push_back(number);
        start(LowPriority);
    }
    else {
        QMutexLocker locker(&mMutex);
        mGenReq.push_back(number);
        mCondition.wakeOne();
    }

    return {};
}

void DataGenerator::run()
{
    size_t currentBatchNum = 0;
    bool taskAbort = false;

    while(true)
    {
        mMutex.lock();
        if(this->mGenReq.count() == 0)
        {
            mCondition.wait(&mMutex);
        }
        taskAbort = this->mAbort;

        if(!taskAbort && mGenReq.count() > 0)
        {
            currentBatchNum = this->mGenReq.front();
            this->mGenReq.pop_front();
        }
        mMutex.unlock();

        if(taskAbort)
        {
            break;
        }
        else
        {
            QVariant res = kernelFunc(currentBatchNum);
            //sleep(2);
            emit dataLoadFinished(res);
        }
    }
}

void DataGenerator::exit()
{
    mMutex.lock();
    mAbort = true;
    mCondition.wakeOne();
    mMutex.unlock();

    wait();
}

////////////////////////////////////////////////////////////////////
QVariant RandomDataGenerator::kernelFunc(size_t number)
{
    QList<qreal> res;

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
