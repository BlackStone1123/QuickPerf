#pragma once
#include <QAbstractListModel>
#include <QColor>
#include <QString>
#include <QList>

#include "../CommonDefines.h"

class DataGenerator;
struct ChannelDataRow
{
    QString key{};
    QString value{};
    QString type{};
};

class ChannelDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    using ChannelRowList = QList<ChannelDataRow>;

    enum class ChannelDataRoles : std::uint64_t
    {
        label = Qt::UserRole + 1,
        columnName,
        dataType
    };

    ChannelDataModel(QObject* parent = nullptr);
    virtual ~ChannelDataModel();

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    void addChannelDataBefore(size_t index, const QString& key, const QString& value, const QString& type);
    void appendChannelData(const QString& key, const QString& value, const QString& type);
    void removeChannelData(const QString& key);

private:
    ChannelDataRow generateChannelDataRow(const QString& key, const QString& value, const QString& type);

private:
    ChannelRowList mRows;
};
