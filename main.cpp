#include <QQmlApplicationEngine>
#include "Controller/MainWindowController.h"
#include <iostream>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    
    MainWindowController mainController;
    mainController.initialize();

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
