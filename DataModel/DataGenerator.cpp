#include "DataGenerator.h"
#include <iostream>
#include <QMutexLocker>

static unsigned long rseed = 10;

WorkerThread::WorkerThread(QObject* parent)
    : QThread(parent)
{
    QObject::connect(this, &QThread::finished, this, [](){
        std::cout << "the worker thread finished" << std::endl;
    });
}

WorkerThread::~WorkerThread()
{
}

QVariant WorkerThread::generate(const QString& columnName, size_t from, size_t number)
{
    if(!isRunning())
    {
        mGenReq.push_back({columnName, from, number});
        start(LowPriority);
    }
    else {
        QMutexLocker locker(&mMutex);
        mGenReq.push_back({columnName, from, number});
        mCondition.wakeOne();
    }

    return {};
}

void WorkerThread::run()
{
    size_t currentBatchNum = 0;
    size_t currentBatchFrom = 0;
    QString columnName;

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
            currentBatchFrom = this->mGenReq.front().from;
            currentBatchNum = this->mGenReq.front().count;
            columnName = this->mGenReq.front().columnName;
            this->mGenReq.pop_front();
        }
        mMutex.unlock();

        if(taskAbort)
        {
            break;
        }
        else
        {
            QVariant res = kernelFunc(columnName, currentBatchFrom, currentBatchNum);
            //sleep(2);
            emit dataLoadFinished(columnName, res);
        }
    }
    onThreadFinished();
}

void WorkerThread::exit()
{
    mMutex.lock();
    mAbort = true;
    mCondition.wakeOne();
    mMutex.unlock();

    wait();
}

////////////////////////////////////////////////////////////////////
//QVariant RandomDataGenerator::kernelFunc(const QString& column, size_t from, size_t number)
//{
//    QList<qreal> res;

//    std::normal_distribution<float> distribution(40.0, 10.0);

//    for (size_t j = 0; j < number; j++)
//    {
//        res << distribution(mRandomEng);
//    }

//    return QVariant::fromValue(res);
//}

//RandomDataGenerator::~RandomDataGenerator()
//{
//    DataGenerator::exit();
//    std::cout << "random data gen deletion!" << std::endl;
//}

//RandomDataGenerator::RandomDataGenerator(QObject* parent)
//    : DataGenerator(parent)
//{
//    mRandomEng.seed( rseed );
//    rseed *= 100;
//}
///////////////////////////////////////////////////////////////
