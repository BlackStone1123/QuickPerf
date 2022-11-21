#include "WorkerThread.h"
#include <QDebug>
#include <QMutexLocker>

WorkerThread::WorkerThread(QObject* parent)
    : QThread(parent)
{
    QObject::connect(this, &QThread::finished, this, [](){
        qDebug() << "the worker thread finished";
    });
}

WorkerThread::~WorkerThread()
{
}

QVariant WorkerThread::generate(const QString& key, const QString& columnName, size_t from, size_t number)
{
    if(!isRunning())
    {
        mGenReq.push_back({key, columnName, from, number});
        start(LowPriority);
    }
    else {
        QMutexLocker locker(&mMutex);
        mGenReq.push_back({key, columnName, from, number});
        mCondition.wakeOne();
    }

    return {};
}


void WorkerThread::run()
{
    size_t currentBatchNum = 0;
    size_t currentBatchFrom = 0;
    QString columnName;
    QString key;

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
            key = this->mGenReq.front().key;
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
            emit dataLoadFinished(key, res);
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
