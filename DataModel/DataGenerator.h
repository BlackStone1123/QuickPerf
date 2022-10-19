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
    virtual size_t getBackEndDataSize() const = 0;

signals:
    void dataLoadFinished(const QVariant&);

protected:
    virtual QVariant kernelFunc(size_t number) = 0;
    virtual void run() override;
    void exit();
    int mIndex{0};
private:
    QMutex mMutex;
    QWaitCondition mCondition;
    QList<size_t> mGenReq;

    bool mRestart{false};
    bool mAbort{false};
};

class RandomDataGenerator: public DataGenerator
{
public:
    RandomDataGenerator(QObject* parent);
    virtual ~RandomDataGenerator();
    virtual size_t getBackEndDataSize() const override { return 20000; }

private:
    virtual QVariant kernelFunc(size_t number) override;

private:
    std::default_random_engine mRandomEng;
};
