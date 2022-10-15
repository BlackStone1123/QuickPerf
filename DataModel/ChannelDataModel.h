#pragma once
#include <QAbstractListModel>
#include <QList>
#include <QColor>

#include "../CommonDefines.h"

class DataGenerator;
struct ChannelDataRow
{
    DataGenerator* generator;
    QColor color {Qt::red};
};

class ChannelDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    using GeneratorList = QList<QPointer<DataGenerator>>;
    using ChannelRowList = QList<ChannelDataRow>;

    enum class ChannelDataRoles : std::uint64_t
    {
        Generator = Qt::UserRole + 1,
        Color
    };

    ChannelDataModel(GeneratorList generators, QObject* parent = nullptr);
    virtual ~ChannelDataModel();

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    void addChannelDataBefore(size_t index, QPointer<DataGenerator> gen);

private:
    ChannelDataRow generateChannelDataRow(size_t index, QPointer<DataGenerator> gen);

private:
    ChannelRowList mRows;
};
