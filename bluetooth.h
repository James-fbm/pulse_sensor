#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define RANDOMDATAGEN 1

#include <QObject>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QRandomGenerator>
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
    void heartRateReceived(int rate);

private:
    QBluetoothServer *server;
};

#endif // BLUETOOTH_H
