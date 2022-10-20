#include "MainWindowController.h"

#include <string>
#include <QCoreApplication>
#include <QQmlEngine>

#include "PerfGraphViewController.h"
#include "SingleChannelController.h"
#include "PerfListViewController.h"
#include "../DataModel/RectangleViewModel.h"
#include "../DataModel/ChannelDataModel.h"
#include "../DataModel/DataCenter.h"
#include "../DataModel/TreeModel.h"

static const std::string title = "QuickPerf";

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
    qmlRegisterType<PerfListViewController>("com.biren.dataModel", 1, 0, "PerfListViewController");
    qmlRegisterType<SingleChannelController>("com.biren.dataModel", 1, 0, "SingleChannelController");

    qmlRegisterUncreatableType<RectangleViewModel>("com.biren.dataModel", 1, 0, "RectangleViewModel", "Should not be created from qml file");
    qmlRegisterUncreatableType<TreeModel>("com.biren.dataModel", 1, 0, "TreeModel", "Should not be created from qml file");
    qmlRegisterUncreatableType<ChannelDataModel>("com.biren.dataModel", 1, 0, "ChannelDataModel", "Should not be created from qml file");
    qmlRegisterUncreatableType<DataGenerator>("com.biren.dataModel", 1, 0, "DataGenerator", "Should not be created from qml file");
}
