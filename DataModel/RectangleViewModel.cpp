#include "RectangleViewModel.h"
#include <iostream>

RectangleViewModel::RectangleViewModel(const QList<qreal>& datas, QObject* parent)
    : QAbstractListModel(parent)
    , mDatas(datas)
{
}

RectangleViewModel::~RectangleViewModel()
{
    std::cout << "RectangleViewModel deletion" << std::endl;
}

QHash<int,QByteArray> RectangleViewModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[(int) RectangleViewModelDataRoles::Amplitude] = "Amplitude";

    return roles;
}

QVariant RectangleViewModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= MAXIMUM_RECTANGLE_DATA_COUNT)
        return QVariant();

    if (role == (int) RectangleViewModelDataRoles::Amplitude)
    {
        return index.row() + mBaseOffset >= (size_t)mDatas.count() ?
                    0 : mDatas[index.row() + mBaseOffset];
    }

    return QVariant();
}

int RectangleViewModel::rowCount(const QModelIndex & parent) const
{
    return MAXIMUM_RECTANGLE_DATA_COUNT;
}

void RectangleViewModel::setBaseOffset(size_t baseOffset)
{
    mBaseOffset = baseOffset;
    emit dataChanged(index(0), index(MAXIMUM_RECTANGLE_DATA_COUNT - 1));
}
