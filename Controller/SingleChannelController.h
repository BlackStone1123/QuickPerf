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

    void loadInitialDatas(int count = -1);
    void startLoading(size_t loadSize);

    DataGenerator* getDataGenerator() const { return mGenerator; }
    void setDataGenerator(DataGenerator* gen);

    void integralMove(int count, bool forward);
    void integralMoveTo(int pos);
    void sliderMove(int count, bool left, bool forward);
    void zoomTo(size_t count);

    int requestForMoveStride(size_t preferSize, bool forward);
    int requestForZoomStride(size_t count);

    size_t getTotalRange() const {return mTotalRange;}
    size_t getDisplayingDataCount() const {return mDisplayingDataCount;}
    size_t getRangeStartPosition() const {return mRangeStartPos;}

    Q_INVOKABLE int getTotalDataCount() const;
    Q_INVOKABLE void setKey(const QString& name){ mKey = name; }

signals:
    void bundleUpdated();
    void loadingUpdated();
    void loaderTypeChanged();
    void barSetModelChanged();
    void dataGeneratorChanged();
    void rangeStartPosChanged();
    void displayingDataCountChanged();

private:
    void onDataLoadedArrived(const QVariant& data);
    void appendDatas(const QList<qreal>&);
    void fetchMoreData(size_t count);
    void updateModel();
    void rebase();

private:
    QList<qreal> mAmplitudes;
    QString mKey;

    DataGenerator* mGenerator {nullptr};
    RectangleViewModel* mBarSetModel {nullptr};

    bool mLoading {false};
    int mPendingLoading{0};
    LoaderType mLoaderType {PointSet};

    size_t mRangeStartPos {0};
    size_t mDisplayingDataCount {INITIAL_DISPLAYING_DATA_RANGE};

    size_t mTotalRange{ INITIAL_TOTAL_DATA_RANGE };
    size_t mBatchSize{ LOADING_BATCH_SIZE };
};
