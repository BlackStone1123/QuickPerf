#include "DataGenerator.h"
#include <iostream>
#include <QMutexLocker>

static unsigned long rseed = 10;

DataGenerator::DataGenerator(QObject* parent)
    : QThread(parent)
{
}

DataGenerator::~DataGenerator()
{
}

QVariant DataGenerator::generate(size_t number, bool immediate)
{
    if(immediate)
        return kernelFunc(number);
    else
    {
        mMutex.lock();
        mCount = number;
        mMutex.unlock();

        if(!isRunning())
        {
            start(LowPriority);
        }
    }
    return {};
}

void DataGenerator::run()
{
    mMutex.lock();
    int totalCount = this->mCount;
    mMutex.unlock();

    sleep(3);
    QVariant res = kernelFunc(totalCount);
    emit dataLoadFinished(res);
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
