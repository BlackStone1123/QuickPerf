#pragma once
#include "../CommonDefines.h"
#include <QVariant>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

struct GenParam
{
    QString key;
    QString columnName;
    size_t from {0};
    size_t count {0};
};

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    WorkerThread(QObject* parent = nullptr);
    virtual ~WorkerThread();

    virtual QVariant generate(const QString& key, const QString& column, size_t from, size_t number);
    virtual size_t getBackEndDataSize() const {return 0;}

signals:
    void dataLoadFinished(const QString& key, const QVariant&);

protected:
    virtual QVariant kernelFunc(const QString& column, size_t from, size_t number) = 0;
    virtual void onThreadFinished(){}
    virtual void run() override;
    void exit();

private:
    QMutex mMutex;
    QWaitCondition mCondition;
    QList<GenParam> mGenReq;

    bool mRestart{false};
    bool mAbort{false};
};
