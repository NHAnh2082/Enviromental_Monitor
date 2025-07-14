#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "SerialPortManger.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // qmlRegisterType<SerialPortWrapper>("SerialPort", 1, 0, "SerialPortManager");

    SerialPortManager serialManager;
    engine.rootContext()->setContextProperty("serialManager", &serialManager);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("UART", "Main");

    return app.exec();
}
