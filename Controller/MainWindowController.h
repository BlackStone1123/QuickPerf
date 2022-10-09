#pragma once
#include <QQuickView>
#include "../CommonDefines.h"

class QQuickView;
class MainWindowController : public QObject
{
    Q_OBJECT
    
public:
    MainWindowController();
    virtual ~MainWindowController();

    void initialize();
    QPointer<QQuickView> createQuickView(const QUrl& url, size_t width, size_t height);

private:
    void registerMetaTypes();
};