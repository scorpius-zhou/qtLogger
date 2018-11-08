#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "asynclog.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    AsyncLogger & logger = AsyncLogger::instance();
    logger.addLogFilter("download", 3);

    aDebug() << "hello world" << endl;
    aInfo() << "download success" <<endl;

    return app.exec();
}

