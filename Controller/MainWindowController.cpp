#include "MainWindowController.h"

#include <string>
#include <QCoreApplication>
#include <QQmlEngine>

#include "PerfGraphViewController.h"
#include "../DataModel/ChannelDataModel.h"

static const std::string title = "Performance Tool";

MainWindowController::MainWindowController()
{
    
}

MainWindowController::~MainWindowController()
{
    
}

void MainWindowController::initialize()
{
    registerMetaTypes();
}

QPointer<QQuickView> MainWindowController::createQuickView(const QUrl& url, size_t width, size_t height)
{
    QQuickView* pView = new QQuickView;
    pView->setSource(url);
    pView->setWidth(width);
    pView->setHeight(height);
    pView->setX(0);
    pView->setY(0);
    pView->setTitle(QString::fromStdString(title));
    pView->setResizeMode (QQuickView::SizeRootObjectToView);
    
    QObject::connect(pView->engine(),&QQmlEngine::quit, &QCoreApplication::quit);
    return pView;
}

void MainWindowController::registerMetaTypes()
{
    qmlRegisterType<PerfGraphViewController>("com.biren.dataModel", 1, 0, "PerfGraphViewController");
    qmlRegisterUncreatableType<ChannelDataModel>("com.biren.dataModel", 1, 0, "ChannelDataModel", "Should not be created from qml file");
    // qRegisterMetaType<QList<float>>();
}
