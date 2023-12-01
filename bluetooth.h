#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define RANDOMDATAGEN 0

#include <QObject>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QRandomGenerator>
#include <QDebug>
#include <QByteArray>
#include <QDateTime>

class BluetoothServer : public QObject {
    Q_OBJECT
public:
    BluetoothServer();
    ~BluetoothServer();

//private slots:
public slots:
    void clientConnected();
    void readSocket();

signals:
    void heartRateReceived(qint32 rate, qint64 ms);

private:
    QBluetoothServer *server;
};

#endif // BLUETOOTH_H
