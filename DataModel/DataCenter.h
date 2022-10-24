#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include "WorkerThread.h"

class QAxObject;
class ExcelDataCenter;

class DataGenerator: public QObject
{
    Q_OBJECT
public:
    friend class ExcelDataCenter;
    DataGenerator(const QString& valueColumn, QObject* parent = nullptr);
    void generate(size_t number);
    size_t getBackEndDataSize() const;

signals:
    void dataLoaded(const QVariant&);

private slots:
    void onDataLoadFinished(const QString& columnName, const QVariant&);

private:
    QString mValue;
    size_t mFrom {0};
    ExcelDataCenter* mDataCenter{nullptr};
};

class ExcelDataCenter: public WorkerThread
{
    Q_OBJECT
public:
    ExcelDataCenter(int rowCount, const QString& fileName, QObject* parent = nullptr);
    virtual ~ExcelDataCenter();
    size_t getBackEndDataSize() const;

    static DataGenerator* creatDataGenerator(const QString& valueColumn);

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
