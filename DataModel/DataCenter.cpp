#include "DataCenter.h"
#include <QAxObject>
#include <QDebug>
#include <QDir>
#include <windows.h>

// static ExcelDataCenter* localGlobalDataCenter = nullptr;

static ExcelDataCenter* s_global_data_center= nullptr;

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

void DataGenerator::generate(size_t number)
{
    if(mWorker && !mValue.isEmpty())
    {
        qWarning() << "request data from column: " << mValue << " from: " << mFrom << "number: " << number;
        mWorker->generate(mValue, mFrom, number);
        mFrom += number;
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
    return mWorker->getBackEndDataSize();
}
/////////////////////////////////////////////////////////////////////////////////////
ExcelWorker::ExcelWorker(int rowCount, const QString& fileName, QObject* parent)
    : WorkerThread(parent)
    , mRowCount(rowCount)
    , mFileName(fileName)
{
}

ExcelWorker::~ExcelWorker()
{
    WorkerThread::exit();
}

size_t ExcelWorker::getBackEndDataSize() const
{
    return mRowCount;
}

QVariant ExcelWorker::kernelFunc(const QString& column, size_t from, size_t number)
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

void ExcelWorker::onThreadFinished()
{
    mWorkBook->dynamicCall("Close()");
    mExcel->dynamicCall("Quit()");
    delete mExcel;
}
/////////////////////////////////////////////////////////////////////////////////////
ExcelDataCenter::ExcelDataCenter(QObject* parent)
    : QObject(parent)
{
    s_global_data_center = this;
}

ExcelDataCenter::~ExcelDataCenter()
{

}

void ExcelDataCenter::addWorker(DataType type, WorkerThread* worker)
{
    if(mWorkers.find(type) == mWorkers.end())
    {
        mWorkers[type] = worker;
    }
}

DataGenerator* ExcelDataCenter::creatDataGenerator(DataType type, const QString& valueColumn)
{
    QPointer<WorkerThread> worker = s_global_data_center->mWorkers[type];
    if(worker == nullptr)
        return nullptr;

    DataGenerator* pGen = new DataGenerator(valueColumn);
    pGen->mWorker = worker;
    pGen->mType = type;
    connect(worker, &WorkerThread::dataLoadFinished, pGen, &DataGenerator::onDataLoadFinished);

    return pGen;
}
