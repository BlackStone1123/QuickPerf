#pragma once
#include <QObject>
#include "../CommonDefines.h"

class DataGenerator;
class RectangleViewModel;

struct InitialStatus
{
    bool pinding{false};
    int totalCycleRange{0};
    int displayingCycleRange{0};
};

class CycleRangeConverter: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int displayingDataCount MEMBER mDisplayingDataCount NOTIFY displayingDataCountChanged)
    Q_PROPERTY(int displayingCycleCount MEMBER mCycleDisplayingCount NOTIFY displayingDataCountChanged)
    Q_PROPERTY(int rangeStartPos MEMBER mRangeStartPos NOTIFY rangeStartPosChanged)
    Q_PROPERTY(int cycleRangeStartPos MEMBER mCycleRangeStartPos NOTIFY cycleRangeStartPosChanged)
    Q_PROPERTY(int rangeStartOffset MEMBER mRangeStartOffset NOTIFY rangeStartOffsetChanged)
    Q_PROPERTY(int scale MEMBER mScale NOTIFY scaleChanged)
public:
    CycleRangeConverter(int scale = 1, int start = 0, int range = 0, QObject* parent = nullptr);
    virtual ~CycleRangeConverter() {}

    void integralMove(int count, bool forward);
    void integralMoveTo(int pos);
    void boundaryMove(int count, bool left, bool forward);
    void zoomTo(int count);

    int getScale() const { return mScale; }
    int getConvertedRangeStart() const { return mRangeStartPos;}
    int getConvertedDisplayingRange() const { return mDisplayingDataCount;}

signals:
    void rangeStartPosChanged();
    void rangeStartOffsetChanged();
    void cycleRangeStartPosChanged();
    void displayingDataCountChanged();
    void displayingCycleCountChanged();
    void scaleChanged();

private:
    void updateRangeStartPos(int count, bool forward);

    int mRangeStartPos {0};
    int mRangeStartOffset {0};
    int mDisplayingDataCount {0};
    int mScale{1};

    int mCycleRangeStartPos{0};
    int mCycleDisplayingCount{0};
};

class SingleChannelController: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> bundle MEMBER mAmplitudes NOTIFY bundleUpdated)
    Q_PROPERTY(bool loading MEMBER mLoading NOTIFY loadingUpdated)
    Q_PROPERTY(bool pinding MEMBER mPinding NOTIFY pindingUpdated)
    Q_PROPERTY(DataGenerator* dataGenerator READ getDataGenerator WRITE setDataGenerator NOTIFY dataGeneratorChanged)
    Q_PROPERTY(CycleRangeConverter* rangeConverter READ getRangeConverter WRITE setRangeConverter NOTIFY cycleRangeConverterChanged)

public:
    explicit SingleChannelController(QObject* parent = nullptr);
    virtual ~SingleChannelController();

    void loadInitialDatas(const InitialStatus& status);
    void startLoading(size_t loadSize);

    DataGenerator* getDataGenerator() const { return mGenerator; }
    void setDataGenerator(DataGenerator* gen);

    CycleRangeConverter* getRangeConverter() const { return mCycleRangeConverter; }
    void setRangeConverter(CycleRangeConverter* converter);

    void integralMove(int count, bool forward);
    void integralMoveTo(int pos);
    void sliderMove(int count, bool left, bool forward);
    void zoomTo(size_t count);
    void updatePinStatus(bool pined, bool fromUI);

    int getTotalDataCount() const;
    void setKey(const QString& name){ mKey = name; }

signals:
    void bundleUpdated();
    void loadingUpdated();
    void pindingUpdated();
    void loaderTypeChanged();
    void barSetModelChanged();
    void dataGeneratorChanged();
    void cycleRangeConverterChanged();

private:
    void onDataLoadedArrived(const QVariant& data);
    void onRangeStartPosChanged();
    void onDisplayingDataCountChanged();

    void appendDatas(const QList<qreal>&);
    void fetchMoreData(size_t count);
    void updateModel();

private:
    QList<qreal> mAmplitudes;
    QString mKey;

    DataGenerator* mGenerator {nullptr};
    CycleRangeConverter* mCycleRangeConverter {nullptr};

    bool mLoading {false};
    bool mPinding {false};
    int mPendingLoading{0};

    size_t mTotalRange{ INITIAL_TOTAL_DATA_RANGE };
    size_t mBatchSize{ LOADING_BATCH_SIZE };
};
