#ifndef BLUETOOTH_H
#define BLUETOOTH_H

// #define RANDOMDATAGEN 1

#include <QObject>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QRandomGenerator>
#include <QDebug>
#include <QByteArray>

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
    void heartRateReceived(qint32 rate);

private:
    QBluetoothServer *server;
};

#endif // BLUETOOTH_H
