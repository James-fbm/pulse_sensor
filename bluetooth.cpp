#include "bluetooth.h"

BluetoothServer::BluetoothServer() {
    // Create a RFCOMM Bluetooth server
    this->server = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);

    // Connect the newConnection signal
    connect(server, &QBluetoothServer::newConnection, this, &BluetoothServer::clientConnected);

    // Start listening for clients
    server->listen(server->serverAddress(), 1);

    qDebug() << server->isListening() << server->serverAddress() << server->serverPort();
}

BluetoothServer::~BluetoothServer() {
    server->close();
    delete server;
}

void BluetoothServer::clientConnected() {
    QBluetoothSocket *socket = server->nextPendingConnection();

    qDebug() << "new connection";

    // Connect readyRead signal to handle incoming data
    connect(socket, &QBluetoothSocket::readyRead, this, &BluetoothServer::readSocket);
}

void BluetoothServer::readSocket() {

//#ifdef RANDOMDATAGEN    // if no bluetooth devices by hand, use this random number generator
//    int v = 0;
//    while (1) {
//        int randomNumber = QRandomGenerator::global()->bounded(80, 170);
//        for (int i = 0 ; i < 20000000 ; ++i);
//        qint64 ms = QDateTime::currentMSecsSinceEpoch();
//        emit heartRateReceived(randomNumber, ms);
//        ++v;
//    }
//#else

    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    int byte_length = data.size();

    if (byte_length > 0) {
        // Unpack the received byte into an integer
        qDebug() << "received bytes from bluetooth: " << byte_length << "bytes";
        int pos = 0;
        while (pos < byte_length) {
            int value = static_cast<unsigned char>(data[pos]);
            ++pos;
            QByteArray time_array =data.mid(pos, 8);    // int64 has 8 bytes
            qint64 ms;
            memcpy(&ms, time_array.constData(), sizeof(long long));

            pos += 8;
            emit heartRateReceived(value, ms);
        }
    }
//#endif
}
