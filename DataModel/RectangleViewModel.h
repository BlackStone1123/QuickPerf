#pragma once
#include <QAbstractListModel>
#include "../CommonDefines.h"

class RectangleViewModel: public QAbstractListModel
{
    Q_OBJECT

public:
    RectangleViewModel(const QList<qreal>& datas, QObject* parent = nullptr);
    virtual ~RectangleViewModel();

    enum class RectangleViewModelDataRoles : std::uint64_t
    {
        Amplitude = Qt::UserRole + 1,
    };

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    void setBaseOffset(size_t baseOffset);

private:
    const QList<qreal>& mDatas;
    size_t mBaseOffset{0};
};
