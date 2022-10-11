#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QColor>
#include <QPointer>

class DataGenerator;
class BarSetModel: public QAbstractListModel
{
    Q_OBJECT
    
public:
    enum class BarSetModelDataRoles : std::uint64_t
    {
        Amplitude = Qt::UserRole + 1,
    };

    BarSetModel(QObject* parent = nullptr);
    virtual ~BarSetModel();

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    void appendDatas(const QList<float>&);
    void setInitialDatas(const QList<float>&);

    QPointer<DataGenerator> getDataGenerator() const { return mGenerator; }
    void setDataGenerator(QPointer<DataGenerator> gen);

signals:
    void getPeresistentIndex(void*, int*);

private slots:
    void onDataLoadedArrived(const QVariant& data);

private:
    QList<float> m_Amplitudes;
    QPointer<DataGenerator> mGenerator;
};

struct ChannelDataRow
{
    BarSetModel* barSetModel;
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
        BarSetModel = Qt::UserRole + 1,
        Color
    };

    ChannelDataModel(GeneratorList generators, QObject* parent = nullptr);
    virtual ~ChannelDataModel();

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    void addChannelDataBefore(size_t index, QPointer<DataGenerator> gen);
    void fetchMoreData(size_t count);

    size_t getRange() const {return mRange;}
    size_t getBatchSize() const {return mBatchSize;}

private:
    ChannelDataRow generateChannelDataRow(size_t index, QPointer<DataGenerator> gen);
    void getBarSetModelIndex(void*, int*);

private:
    ChannelRowList mRows;

    size_t mRange{1000};
    size_t mBatchSize{500};
};
