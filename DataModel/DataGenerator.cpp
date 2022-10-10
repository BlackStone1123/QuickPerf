#include "DataGenerator.h"
#include <random>
#include <iostream>
#include <QMutexLocker>

DataGenerator::DataGenerator(QObject* parent)
    : QThread(parent)
{

}

DataGenerator::~DataGenerator()
{
//    mMutex.lock();
//    mAbort = true;
//    mCondition.wakeOne();
//    mMutex.unlock();
    std::cout << "data gen deletion!" << std::endl;
}

QList<float> DataGenerator::generate(size_t number, bool immediate)
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
    QList<float> res = kernelFunc(totalCount);
    emit dataLoadFinished(res);
}

////////////////////////////////////////////////////////////////////
QList<float> RandomDataGenerator::kernelFunc(size_t number)
{
    QList<float> res;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distribution(16.0,20.0);

    for (size_t j = 0; j < number; j++)
    {
        res << distribution(gen);
    }

    return res;
}

RandomDataGenerator::~RandomDataGenerator()
{
    wait();
    std::cout << "random data gen deletion!" << std::endl;
}
