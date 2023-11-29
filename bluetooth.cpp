#include "bluetooth.h"

BluetoothServer::BluetoothServer() {
    // Create a RFCOMM Bluetooth server
    this->server = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);

    // Connect the newConnection signal
    connect(server, &QBluetoothServer::newConnection, this, &BluetoothServer::clientConnected);

    // Start listening for clients
    server->listen();
}

BluetoothServer::~BluetoothServer() {
    server->close();
    delete server;
}

void BluetoothServer::clientConnected() {
    QBluetoothSocket *socket = server->nextPendingConnection();

    // Connect readyRead signal to handle incoming data
    connect(socket, &QBluetoothSocket::readyRead, this, &BluetoothServer::readSocket);
}

void BluetoothServer::readSocket() {

#ifdef RANDOMDATAGEN    // if no bluetooth devices by hand, use this random number generator
    int v = 0;
    while (1) {
        int randomNumber = QRandomGenerator::global()->bounded(80, 170);
        for (int i = 0 ; i < 2000000 ; ++i);
        qint64 ms = QDateTime::currentMSecsSinceEpoch();
        emit heartRateReceived(randomNumber, ms);
        ++v;
    }
#else

    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    if (data.size() == 1) {
        // Unpack the received byte into an integer
        int value = static_cast<unsigned char>(data[0]);
        // qDebug() << "Received value:" << value;
        qint64 ms = QDateTime::currentMSecsSinceEpoch();
        emit heartRateReceived(randomNumber, ms);
    }
#endif
}
