#include "DataCenter.h"
#include <QAxObject>
#include <QDebug>
#include <windows.h>

static const DataCenter* localGlobalDataCenter = nullptr;

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

ExcelDataGenerator::ExcelDataGenerator(const QString& column, int columnCount, QObject* parent)
    : DataGenerator(parent)
    , mColumn(column)
    , mColumnCount(columnCount)
{

}

ExcelDataGenerator::~ExcelDataGenerator()
{
    DataGenerator::exit();

    mWorkBook->dynamicCall("Close()");
    mExcel->dynamicCall("Quit()");
}

size_t ExcelDataGenerator::getBackEndDataSize() const
{
    return mColumnCount;
}

QVariant ExcelDataGenerator::kernelFunc(size_t from, size_t number)
{
    QString fromStr = mColumn + QString::number(from + 2);
    QString toStr = mColumn + QString::number(from + number + 1);

    if(mExcel == nullptr)
    {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        mExcel = new QAxObject("Excel.Application");

        QAxObject* workBooks = mExcel->querySubObject("WorkBooks");
        mWorkBook = workBooks->querySubObject("Open(QString&)", "D:\\Code\\QuickTest\\spc_0_0_by_cycle_range.xlsx");

        QAxObject* workSheets = mWorkBook->querySubObject("WorkSheets");
        mWorkSheet = workSheets->querySubObject("Item(int)", 1);
    }

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

//////////////////////////////////////////////////////////////////////////////
DataCenter::DataCenter(const QString& fileName, QObject* parent)
    : QObject(parent)
    , mFileName(fileName)
{
}

DataCenter::~DataCenter()
{
    qDebug() << "Data Center deletion";
    mWorkBook->dynamicCall("Close()");
    mExcel->dynamicCall("Quit()");
    localGlobalDataCenter = nullptr;

    delete mExcel;
}

void DataCenter::initialize()
{
    if(!mFileName.isEmpty())
    {
        localGlobalDataCenter = this;

        mExcel = new QAxObject("Excel.Application");

        QAxObject* work_books = mExcel->querySubObject("WorkBooks");
        mWorkBook = work_books->querySubObject("Open(QString&)", mFileName);

        QAxObject* work_sheets = mWorkBook->querySubObject("WorkSheets");

        mWorkSheet = work_sheets->querySubObject("Item(int)", 1);
        mUsedRange = mWorkSheet->querySubObject("UsedRange");

        QAxObject * rows = mUsedRange->querySubObject("Rows");
        mRowNumber = rows->property("Count").toInt() - 1;

        QAxObject* range = mWorkSheet->querySubObject("Range(QVariant, QVariant)", "B1", "BHI1");

        QVariant var = range->dynamicCall("Value");
        QVariantList row = var.toList();

        if(row.count() <= 0)
        {
            qDebug() << "Title not exists";
            return;
        }

        for(auto _one : row[0].toList())
        {
            mTitleList.append(_one.toString());
        }
    }
}

DataGenerator* DataCenter::creatDataGenerator(const QString& columnName)
{
    if(localGlobalDataCenter)
    {
        auto allTitles = localGlobalDataCenter->mTitleList;
        auto index = allTitles.indexOf(columnName);

        if(index == -1)
        {
            qDebug() << "Invalid columnName!";
            return nullptr;
        }

        return new ExcelDataGenerator(
                    indexToColumnLabel(index + 2),
                    localGlobalDataCenter->mRowNumber);
    }
    return nullptr;
}
