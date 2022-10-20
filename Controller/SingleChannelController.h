#pragma once
#include <QObject>
#include "../CommonDefines.h"

class DataGenerator;
class RectangleViewModel;
class SingleChannelController: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> bundle MEMBER mAmplitudes NOTIFY bundleUpdated)
    Q_PROPERTY(bool loading MEMBER mLoading NOTIFY loadingUpdated)
    Q_PROPERTY(int displayingDataCount MEMBER mDisplayingDataCount NOTIFY displayingDataCountChanged)
    Q_PROPERTY(int rangeStartPos MEMBER mRangeStartPos NOTIFY rangeStartPosChanged)
    Q_PROPERTY(LoaderType loaderType MEMBER mLoaderType NOTIFY loaderTypeChanged)
    Q_PROPERTY(RectangleViewModel* barSetModel MEMBER mBarSetModel NOTIFY barSetModelChanged)
    Q_PROPERTY(DataGenerator* dataGenerator READ getDataGenerator WRITE setDataGenerator NOTIFY dataGeneratorChanged)

public:
    enum LoaderType
    {
        Rectangle,
        PointSet
    };
    Q_ENUM(LoaderType)

    explicit SingleChannelController(QObject* parent = nullptr);
    virtual ~SingleChannelController();

    void loadInitialDatas();
    void startLoading(size_t loadSize);

    DataGenerator* getDataGenerator() const { return mGenerator; }
    void setDataGenerator(DataGenerator* gen);

    void move(int count, bool forward);
    void moveTo(int pos);
    void zoomTo(size_t count);

    int requestForMoveStride(size_t preferSize, bool forward);
    int requestForZoomStride(size_t count);

    size_t getTotalRange() const {return mTotalRange;}
    size_t getDisplayingDataCount() const {return mDisplayingDataCount;}

    Q_INVOKABLE int getTotalDataCount() const;
    Q_INVOKABLE void setColumnName(const QString& name){ mColumnName = name; }

signals:
    void bundleUpdated();
    void loadingUpdated();
    void loaderTypeChanged();
    void barSetModelChanged();
    void dataGeneratorChanged();
    void rangeStartPosChanged();
    void displayingDataCountChanged();

private:
    void onDataLoadedArrived(const QString& columnName, const QVariant& data);
    void appendDatas(const QList<qreal>&);
    void fetchMoreData(size_t count);
    void updateModel();
    void rebase();

private:
    QList<qreal> mAmplitudes;
    QString mColumnName;

    DataGenerator* mGenerator {nullptr};
    RectangleViewModel* mBarSetModel {nullptr};

    bool mLoading {false};
    int mPendingLoading{0};
    LoaderType mLoaderType {Rectangle};

    size_t mRangeStartPos {0};
    size_t mDisplayingDataCount {INITIAL_DISPLAYING_DATA_RANGE};

    size_t mTotalRange{ INITIAL_TOTAL_DATA_RANGE };
    size_t mBatchSize{ LOADING_BATCH_SIZE };
};
