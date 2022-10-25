#include <QQmlApplicationEngine>
#include <QtDebug>
#include <QGuiApplication>
#include <QDir>

#include "DataModel/DataCenter.h"
#include "Controller/MainWindowController.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    
    MainWindowController mainController;
    mainController.initialize();

    QDir dir("spc_0_0_by_cycle_range.xlsx");
    ExcelDataCenter dataCenter(2794,  dir.absolutePath(), &app);
    auto pView = mainController.createQuickView(QStringLiteral("qrc:/Qml/main.qml"), 800, 610);
    if(pView)
    {
        pView->show();
    }

    if (app.exec() == 0)
    {
        qDebug() << "quit the application!";
    }
    delete pView;
    return 0;
}
