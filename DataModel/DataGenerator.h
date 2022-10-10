#pragma once
#include "../CommonDefines.h"
#include <QList>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class DataGenerator : public QThread
{
    Q_OBJECT

public:
    DataGenerator(QObject* parent = nullptr);
    virtual ~DataGenerator();

    virtual QList<float> generate(size_t number, bool immediate = false);

signals:
    void dataLoadFinished(const QList<float>&);

protected:
    virtual QList<float> kernelFunc(size_t number) = 0;
    virtual void run() override;

    QMutex mMutex;
    QWaitCondition mCondition;

    size_t mCount{0};

private:

    bool mRestart{false};
    bool mAbort{false};
};

class RandomDataGenerator: public DataGenerator
{
public:
    RandomDataGenerator(QObject* parent): DataGenerator(parent){}
    virtual ~RandomDataGenerator();

private:
    virtual QList<float> kernelFunc(size_t number) override;
};
