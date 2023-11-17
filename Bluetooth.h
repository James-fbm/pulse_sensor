#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QBluetoothServer>
#include <QBluetoothSocket>

class BluetoothServer : public QObject {
    Q_OBJECT
public:
    BluetoothServer();
    ~BluetoothServer();

private slots:
    void clientConnected();
    void readSocket();

signals:
    void heartRateReceived(int rate);

private:
    QBluetoothServer *server;
};

#endif // BLUETOOTH_H
