#pragma once
#include <QAbstractListModel>
#include "../CommonDefines.h"

class RectangleViewModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int rectBaseOffset READ getBaseOffset WRITE setBaseOffset NOTIFY rectBaseOffsetChanged)

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

    void setBaseOffset(int baseOffset);
    int getBaseOffset() const {return mBaseOffset;}

signals:
    void rectBaseOffsetChanged();

private:
    const QList<qreal>& mDatas;
    int mBaseOffset{0};
};
