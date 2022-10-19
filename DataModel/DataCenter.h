#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include "DataGenerator.h"

class QAxObject;
class ExcelDataGenerator: public DataGenerator
{
public:
    ExcelDataGenerator(const QString& column, int columnCount, QObject* parent = nullptr);
    virtual ~ExcelDataGenerator();
    virtual size_t getBackEndDataSize() const override;

private:
    virtual QVariant kernelFunc(size_t from, size_t number) override;

private:
    QAxObject* mExcel {nullptr};
    QAxObject* mWorkSheet {nullptr};
    QAxObject* mWorkBook {nullptr};

    QString mColumn;
    int mColumnCount{0};
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
