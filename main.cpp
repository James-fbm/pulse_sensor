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
    InfluxDB ix(2);

    DBRecord<quint32> r;
    r.measurement = "snvd";
    r.tag = {{"name", "pl"}};
    r.ar_field = {{"value", 4}};
    r.timestamp = 1698894500008;

    DBRecord<> rs;
    rs.measurement = "snvs";
    rs.tag = {{"name", "pl"}};
    rs.ar_field = {{"mood", 5}},
    rs.str_field = {{"value", "12"}};
    rs.timestamp = 1698894500009;
    ix.addData(r);
    ix.addData(rs);

    return a.exec();
}

