#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QColor>
#include <QPointer>
#include "../CommonDefines.h"

class DataGenerator;
class BarSetModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int baseOffset WRITE setBaseOffset READ getBaseOffset NOTIFY baseOffsetChanged)

public:
    BarSetModel(const QList<qreal>& datas, QObject* parent = nullptr);
    virtual ~BarSetModel();

    enum class BarSetModelDataRoles : std::uint64_t
    {
        Amplitude = Qt::UserRole + 1,
    };

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    void setBaseOffset(size_t baseOffset);
    size_t getBaseOffset() const { return mBaseOffset;}

signals:
    void baseOffsetChanged();

private:
    const QList<qreal>& mDatas;
    size_t mBaseOffset{0};
};

class PointSetModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> bundle MEMBER mAmplitudes NOTIFY bundleUpdated)
    Q_PROPERTY(QPointer<BarSetModel> barSetModel MEMBER mBarSetModel NOTIFY barSetModelChanged)
    Q_PROPERTY(bool loading MEMBER mLoading NOTIFY loadingUpdated)
    Q_PROPERTY(LoaderType loaderType MEMBER mLoaderType NOTIFY loaderTypeChanged)

public:
    enum LoaderType
    {
        Rectangle,
        PointSet
    };
    Q_ENUM(LoaderType)

    PointSetModel(QObject* parent = nullptr);
    virtual ~PointSetModel();

    void appendDatas(const QList<qreal>&);
    void setInitialDatas(const QList<qreal>&);

    QPointer<DataGenerator> getDataGenerator() const { return mGenerator; }
    void setDataGenerator(QPointer<DataGenerator> gen);
    void startLoading(size_t loadSize);

signals:
    void getPeresistentIndex(void*, int*);
    void bundleUpdated();
    void loadingUpdated();
    void loaderTypeChanged();
    void barSetModelChanged();

public slots:
    void onDisplayingDataCountChanged(size_t count);
    void onDisplayingPositionChanged(size_t pos);

private slots:
    void onDataLoadedArrived(const QVariant& data);

private:
    QList<qreal> mAmplitudes;
    QPointer<DataGenerator> mGenerator;
    QPointer<BarSetModel> mBarSetModel;

    bool mLoading {false};
    LoaderType mLoaderType {Rectangle};

    int mPendingLoading{0};

    // These two parameters should be sync with controller
    size_t mRangStartPos {0};
    size_t mDisplayingRange {0};
};

struct ChannelDataRow
{
    PointSetModel* pointSetModel;
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
        PointSetModel = Qt::UserRole + 1,
        Color
    };

    ChannelDataModel(GeneratorList generators, QObject* parent = nullptr);
    virtual ~ChannelDataModel();

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    void addChannelDataBefore(size_t index, QPointer<DataGenerator> gen);
    void move(size_t count, bool forward);
    void zoomTo(size_t count);

    size_t requestForMoveStride(size_t preferSize, bool forward);
    size_t requestForZoomStride(size_t count);

    size_t getTotalRange() const {return mTotalRange;}

signals:
    void displayingDataCountChanged(size_t count);
    void displayingPositionChanged(size_t pos);

private:
    ChannelDataRow generateChannelDataRow(size_t index, QPointer<DataGenerator> gen);
    void getPointSetModelIndex(void*, int*);
    void fetchMoreData(size_t count);
    void updateModel();

private:
    ChannelRowList mRows;

    size_t mTotalRange{ INITIAL_TOTAL_DATA_RANGE };
    size_t mBatchSize{ LOADING_BATCH_SIZE };

    // These two parameters should be sync with controller
    size_t mRangStartPos {0};
    size_t mDisplayingRange {0};
};
