/*
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
*/





#include "influxdb.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    InfluxDB ix(1);

    DBRecord r;
    r.setMeasurement("snvds");
    r.addTagPair({"name", "gcq"});
    r.addTagPair({"local", "Cangzhou"});
    r.addFieldPair<QString>({"value", "6"});
    r.addFieldPair<QString>({"mood", "good"});
    r.addFieldPair<double>({"gpa", 4.999});
    r.setTimestamp(1698894500008);

    ix.addData(r);

    return a.exec();
}

