#pragma once
#include "../CommonDefines.h"
#include <QVariant>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <random>

class DataGenerator : public QThread
{
    Q_OBJECT

public:
    DataGenerator(QObject* parent = nullptr);
    virtual ~DataGenerator();

    virtual QVariant generate(size_t number, bool immediate = false);

signals:
    void dataLoadFinished(const QVariant&);

protected:
    virtual QVariant kernelFunc(size_t number) = 0;
    virtual void run() override;
    void exit();

private:
    QMutex mMutex;
    QWaitCondition mCondition;

    size_t mCount{0};
    QList<size_t> mGenReq;

    bool mRestart{false};
    bool mAbort{false};
};

class RandomDataGenerator: public DataGenerator
{
public:
    RandomDataGenerator(QObject* parent);
    virtual ~RandomDataGenerator();

private:
    virtual QVariant kernelFunc(size_t number) override;

private:
    std::default_random_engine mRandomEng;
};
