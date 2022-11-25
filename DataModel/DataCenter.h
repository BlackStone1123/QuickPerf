#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include "WorkerThread.h"

class QAxObject;
class ExcelWorker;

enum DataType
{
    Count = 0,
    Event,
    Slice
};

class DataGenerator: public QObject
{
    Q_OBJECT
public:
    friend class ExcelDataCenter;

    DataGenerator(const QString& key, const QString& valueColumn, QObject* parent = nullptr);
    void generate(size_t number);
    size_t getBackEndDataSize() const;

signals:
    void dataLoaded(const QVariant&);

private slots:
    void onDataLoadFinished(const QString& key, const QVariant&);

private:
    QString mValue;
    QString mKey;

    size_t mFrom {0};
    DataType mType{DataType::Count};
    WorkerThread* mWorker{nullptr};
};

class ExcelWorker: public WorkerThread
{
    Q_OBJECT
public:
    ExcelWorker(int rowCount, const QString& fileName, QObject* parent = nullptr);
    virtual ~ExcelWorker();
    virtual size_t getBackEndDataSize() const override;

private:
    virtual QVariant kernelFunc(const QString& column, size_t from, size_t number) override;
    virtual void onThreadFinished() override;

private:
    QAxObject* mExcel {nullptr};
    QAxObject* mWorkSheet {nullptr};
    QAxObject* mWorkBook {nullptr};

    int mRowCount{10000};
    QString  mFileName;
    QList<QString> mTitleList;
};

class ExcelDataCenter: public QObject
{
    Q_OBJECT
public:
    ExcelDataCenter(const QString& config, QObject* parent = nullptr);
    virtual ~ExcelDataCenter();

    void addWorker(DataType type, WorkerThread* pWorker);
    static DataGenerator* creatDataGenerator(DataType type, const QString& key, const QString& valueColumn);
    static QString getConfigFileName();

private:
    QMap<DataType, QPointer<WorkerThread>> mWorkers;
    QString mConfig;
};
