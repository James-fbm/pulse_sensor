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

    InfluxDB ix;
    for (unsigned long i = 1698894500007 ;; i += 512) {
        QString buf_data("");
        for (unsigned j = 0 ; j < 512 ; ++j) {
            // buf_data += "w_512,location=us-midwest temperature=82 ";
            buf_data += "s_512 value=82.3 ";
            buf_data += QString::number(i+j, 10);
            buf_data += '\n';
        }
        ix.sendData(buf_data);
        // qDebug() << buf_data;
        qDebug() << buf_data.size();
        qDebug() << "i = " << i - 1698894500006;
    }
    return a.exec();
}

