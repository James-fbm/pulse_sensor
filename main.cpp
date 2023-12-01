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





#include "bluetooth.h"
#include <QApplication>
#include "datasource.h"
#include <QQuickView>
#include <QtWidgets/QApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>
#include <QtCore/QDir>


#include <QThread>

class MyThread : public QThread {
public:
    MyThread(BluetoothServer& server) : server(server) {}

protected:
    void run() override {
        server.readSocket();
    }

private:
    BluetoothServer& server;
};



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BluetoothServer server;

    QQuickView viewer; //Creates a QQuickView, which is a window that can display a Qt Quick interface (defined in QML)

    QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QWindow::close); //Connects the quit signal from the QML engine to close the QQuickView window

    viewer.setTitle(QStringLiteral("ECG Monitor"));

    DataSource dataSource(&viewer);

    viewer.rootContext()->setContextProperty("dataSource", &dataSource); // Exposes the dataSource instance to QML, allowing QML components to interact with it

    viewer.setSource(QUrl("qrc:/qml/qmloscilloscope/main.qml"));
    viewer.setResizeMode(QQuickView::SizeRootObjectToView); //Adjusts the resizing behavior of the root QML object to match the view
    viewer.setColor(QColor("#404040"));
    viewer.show();

    QObject::connect(&server, &BluetoothServer::heartRateReceived,
                     &dataSource, &DataSource::updateHeartRate/*, Qt::DirectConnection*/);

    MyThread thread(server);
    thread.start();
    return a.exec();

}

