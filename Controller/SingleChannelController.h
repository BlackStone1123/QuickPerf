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
    Q_PROPERTY(int rectBaseOffset MEMBER mRectViewBaseOffset NOTIFY rectBaseOffsetChanged)
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

    void move(size_t count, bool forward);
    void zoomTo(size_t count);

    size_t requestForMoveStride(size_t preferSize, bool forward);
    size_t requestForZoomStride(size_t count);
    size_t getTotalRange() const {return mTotalRange;}
    size_t getDisplayingDataCount() const {return mDisplayingDataCount;}

signals:
    void bundleUpdated();
    void loadingUpdated();
    void loaderTypeChanged();
    void barSetModelChanged();
    void dataGeneratorChanged();
    void rangeStartPosChanged();
    void displayingDataCountChanged();
    void rectBaseOffsetChanged();

private:
    void onDataLoadedArrived(const QVariant& data);
    void appendDatas(const QList<qreal>&);
    void fetchMoreData(size_t count);
    void updateModel();
    void rebase();

private:
    QList<qreal> mAmplitudes;

    DataGenerator* mGenerator {nullptr};
    RectangleViewModel* mBarSetModel {nullptr};

    bool mLoading {false};
    int mPendingLoading{0};
    LoaderType mLoaderType {Rectangle};

    size_t mRangeStartPos {0};
    size_t mRectViewBaseOffset{0};
    size_t mDisplayingDataCount {INITIAL_DISPLAYING_DATA_RANGE};

    size_t mTotalRange{ INITIAL_TOTAL_DATA_RANGE };
    size_t mBatchSize{ LOADING_BATCH_SIZE };
};
