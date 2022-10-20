#include <QQmlApplicationEngine>
#include <iostream>
#include <QGuiApplication>

#include "DataModel/DataCenter.h"
#include "Controller/MainWindowController.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    
    MainWindowController mainController;
    mainController.initialize();

    ExcelDataCenter dataCenter(2794, "D:\\Code\\QuickTest\\spc_0_0_by_cycle_range.xlsx", &app);
    auto pView = mainController.createQuickView(QStringLiteral("qrc:/Qml/main.qml"), 800, 610);
    if(pView)
    {
        pView->show();
    }

    if (app.exec() == 0)
    {
        std::cout << "quit the application!" << std::endl;
    }
    delete pView;
    return 0;
}
