#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QColor>

#include <random>

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

private:
    QList<float> m_Amplitudes;
};

struct ChannelDataRow
{
    BarSetModel* barSetModel;
    QColor color {Qt::red};
};

class DataGenerator
{
public:
    virtual ~DataGenerator(){}
    virtual QList<float> generate(size_t number)
    {
        QList<float> res;

        std::default_random_engine generator;
        std::normal_distribution<float> distribution(16.0,20.0);

        for (size_t j = 0; j < number; j++)
        {
            res << distribution(generator);
        }

        return res;
    }
};

class ChannelDataModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    enum class ChannelDataRoles : std::uint64_t
    {
        BarSetModel = Qt::UserRole + 1,
        Color
    };

    ChannelDataModel(QObject* parent = nullptr);
    virtual ~ChannelDataModel();

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    void addChannelDataBefore(size_t index);
    void fetchMoreData();

    size_t getRange() const {return mRange;}
    size_t getBatchSize() const {return mBatchSize;}

private:
    ChannelDataRow generateRandomDataRow(size_t index);

private:
    QList<ChannelDataRow> mRows;
    DataGenerator mGenerator;

    size_t mRange{1000};
    size_t mBatchSize{500};
};
