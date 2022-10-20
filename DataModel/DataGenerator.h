#pragma once
#include "../CommonDefines.h"
#include <QVariant>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <random>

struct GenParam
{
    QString columnName;
    size_t from {0};
    size_t count {0};
};

class DataGenerator : public QThread
{
    Q_OBJECT

public:
    DataGenerator(QObject* parent = nullptr);
    virtual ~DataGenerator();

    virtual QVariant generate(const QString& column, size_t from, size_t number, bool immediate = false);
    virtual size_t getBackEndDataSize() const = 0;

signals:
    void dataLoadFinished(const QString& columnName, const QVariant&);

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

class RandomDataGenerator: public DataGenerator
{
public:
    RandomDataGenerator(QObject* parent);
    virtual ~RandomDataGenerator();
    virtual size_t getBackEndDataSize() const override { return 20000; }

private:
    virtual QVariant kernelFunc(const QString& column, size_t from, size_t number) override;

private:
    std::default_random_engine mRandomEng;
};
