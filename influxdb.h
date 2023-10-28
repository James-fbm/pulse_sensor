#ifndef INFLUXDB_H
#define INFLUXDB_H

#endif // INFLUXDB_H

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QDebug>
#include <QThread>

// map to json configuration file
struct DBConfig {
    QString org;
    QString bucket;
    QString precision;
    QString url;
    QString token;
};

class InfluxDB {
private:
    DBConfig config;

    QNetworkAccessManager manager;
    QNetworkRequest request;

    QString buffer;
    unsigned int buf_size;

    void getConfig();
    void makeRequest();

public:
    InfluxDB();
    void sendData(QString data);
};
