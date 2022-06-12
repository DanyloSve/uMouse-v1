#include <QApplication>
#include <QQmlApplicationEngine>

#include "appcore.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    qmlRegisterSingletonType<AppCore>("com.test.AppCore", 1, 0, "AppCore", &AppCore::qmlInstance);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    AppCore *appCore = AppCore::instance();
    appCore->setCntr(5);

    engine.load(url);

    return app.exec();
}
