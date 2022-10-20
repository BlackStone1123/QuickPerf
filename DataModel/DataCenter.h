#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include "DataGenerator.h"

class QAxObject;
class ExcelDataGenerator: public DataGenerator
{
public:
    ExcelDataGenerator(int rowCount, QObject* parent = nullptr);
    virtual ~ExcelDataGenerator();
    virtual size_t getBackEndDataSize() const override;

private:
    virtual QVariant kernelFunc(const QString& column, size_t from, size_t number) override;
    virtual void onThreadFinished() override;

private:
    QAxObject* mExcel {nullptr};
    QAxObject* mWorkSheet {nullptr};
    QAxObject* mWorkBook {nullptr};

    int mRowCount{10000};
    QList<QString> mTitleList;
};

class DataCenter: public QObject
{
    Q_OBJECT
public:
    DataCenter(const QString& fileName, QObject* parent = nullptr);
    virtual ~DataCenter();

    void initialize();

    static DataGenerator* creatDataGenerator(const QString& columnName);

private:
    QString mFileName;
    QList<QString> mTitleList;

    QAxObject* mUsedRange{nullptr};
    QAxObject* mWorkSheet{nullptr};
    QAxObject* mWorkBook{nullptr};
    QAxObject* mExcel {nullptr};
    int mRowNumber{0};
};
