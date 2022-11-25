#include <QQmlApplicationEngine>
#include <QtDebug>
#include <QGuiApplication>
#include <QDir>
#include <memory>

#include "DataModel/DataCenter.h"
#include "Controller/MainWindowController.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    
    MainWindowController mainController;
    mainController.initialize();

    QDir dir("spc_0_0_by_cycle_range.xlsx");
    ExcelWorker counterWorker(2794,  dir.absolutePath(), &app);

    QDir dir1("u_spc_0_0_signals.csv");
    ExcelWorker eventWorker(279334,  dir1.absolutePath(), &app);

    ExcelDataCenter dataCenter("pfa_test_config_2.json", &app);
    dataCenter.addWorker(DataType::Count, &counterWorker);
    dataCenter.addWorker(DataType::Event, &eventWorker);

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
