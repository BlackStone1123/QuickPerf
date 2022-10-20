#include "DataCenter.h"
#include <QAxObject>
#include <QDebug>
#include <windows.h>

static ExcelDataCenter* localGlobalDataCenter = nullptr;

static QString indexToColumnLabel(int index)
{
    QString columnName = "";

    while (index > 0)
    {
        int modulo = (index - 1) % 26;
        columnName = QString('A' + modulo) + columnName;
        index = (index - modulo) / 26;
    }

    return columnName;
}

////////////////////////////////////////////////////////////////////////////
DataGenerator::DataGenerator(const QString& valueColumn, QObject* parent)
    : QObject(parent)
    , mValue(valueColumn)
{
}

void DataGenerator::generate(size_t from, size_t number)
{
    if(mDataCenter)
    {
        mDataCenter->generate(mValue, from, number);
    }
}

void DataGenerator::onDataLoadFinished(const QString& columnName, const QVariant& value)
{
    if(columnName == mValue)
    {
        emit dataLoaded(value);
    }
}

size_t DataGenerator::getBackEndDataSize() const
{
    return mDataCenter->getBackEndDataSize();
}
/////////////////////////////////////////////////////////////////////////////////////
ExcelDataCenter::ExcelDataCenter(int rowCount, const QString& fileName, QObject* parent)
    : WorkerThread(parent)
    , mRowCount(rowCount)
    , mFileName(fileName)
{
    localGlobalDataCenter = this;
}

ExcelDataCenter::~ExcelDataCenter()
{
    WorkerThread::exit();
}

size_t ExcelDataCenter::getBackEndDataSize() const
{
    return mRowCount;
}

#include <QDir>
QVariant ExcelDataCenter::kernelFunc(const QString& column, size_t from, size_t number)
{
    if(mExcel == nullptr)
    {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        mExcel = new QAxObject("Excel.Application");

        QAxObject* workBooks = mExcel->querySubObject("WorkBooks");
        mWorkBook = workBooks->querySubObject("Open(QString&)", mFileName);

        QAxObject* workSheets = mWorkBook->querySubObject("WorkSheets");
        mWorkSheet = workSheets->querySubObject("Item(int)", 1);

        QAxObject* range = mWorkSheet->querySubObject("Range(QVariant, QVariant)", "B1", "BHI1");

        QVariant var = range->dynamicCall("Value");
        QVariantList row = var.toList();

        for(auto _one : row[0].toList())
        {
            mTitleList.append(_one.toString());
        }
    }

    auto columnLabel = indexToColumnLabel(mTitleList.indexOf(column) + 2);
    QString fromStr = columnLabel + QString::number(from + 2);
    QString toStr = columnLabel + QString::number(from + number + 1);

    QAxObject* range = mWorkSheet->querySubObject("Range(QVariant, QVariant)", fromStr, toStr);

    QVariant var = range->dynamicCall("Value");
    QVariantList row = var.toList();

    QList<qreal> res{};
    for(const QVariant& _one : row)
    {
        auto innerRow = _one.toList();
        if(innerRow.isEmpty())
        {
            continue;
        }
        res.append(innerRow[0].toReal());
    }

    delete range;
    return QVariant::fromValue(res);
}

void ExcelDataCenter::onThreadFinished()
{
    mWorkBook->dynamicCall("Close()");
    mExcel->dynamicCall("Quit()");
    delete mExcel;
}

DataGenerator* ExcelDataCenter::creatDataGenerator(const QString& valueColumn)
{
    DataGenerator* pGen = new DataGenerator(valueColumn, localGlobalDataCenter);
    pGen->mDataCenter = localGlobalDataCenter;
    connect(localGlobalDataCenter, &ExcelDataCenter::dataLoadFinished, pGen, &DataGenerator::onDataLoadFinished);

    return pGen;
}
