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
#include "bluetooth.h"
#include <QApplication>
#include "HeartRateMonitor.h"
#include "datasource.h"
#include <QQuickView>
#include <QtWidgets/QApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>
#include <QtCore/QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    InfluxDB ix(2);

//    DBRecord<quint32> r;
//    r.measurement = "snvd";
//    r.tag = {{"name", "pl"}};
//    r.ar_field = {{"value", 4}};
//    r.timestamp = 1698894500008;

//    DBRecord<> rs;
//    rs.measurement = "snvs";
//    rs.tag = {{"name", "pl"}};
//    rs.ar_field = {{"mood", 5}},
//    rs.str_field = {{"value", "12"}};
//    rs.timestamp = 1698894500009;
//    ix.addData(r);
//    ix.addData(rs);

    BluetoothServer server;

//    HeartRateMonitor monitor;

//    QObject::connect(&server, &BluetoothServer::heartRateReceived,
//                     &monitor, &HeartRateMonitor::updateHeartRate);

//    monitor.show();
    // QString extraImportPath(QStringLiteral("%1/../../../%2"));

    QQuickView viewer;

    //viewer.engine()->addImportPath(extraImportPath.arg(QGuiApplication::applicationDirPath(),
    //                                  QString::fromLatin1("qml")));
    QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QWindow::close);

    viewer.setTitle(QStringLiteral("ECG Monitor"));


    DataSource dataSource(&viewer);
    viewer.rootContext()->setContextProperty("dataSource", &dataSource);

    viewer.setSource(QUrl("qrc:/qml/qmloscilloscope/main.qml"));
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    viewer.setColor(QColor("#404040"));
    viewer.show();

    QObject::connect(&server, &BluetoothServer::heartRateReceived,
                     &dataSource, &DataSource::updateHeartRate);


    return a.exec();
}

