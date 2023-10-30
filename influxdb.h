#ifndef INFLUXDB_H
#define INFLUXDB_H

#endif // INFLUXDB_H

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QMap>
#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QDebug>
#include <QThread>
#include <type_traits>

// map to json configuration file
struct DBConfig {
    QString org;
    QString bucket;
    QString precision;
    QString url;
    QString token;
};

// map to single record in InfluxDB
struct DBRecord {
    QString measurement;                // measurement should be a string
    QString tag;                        // key and value of a tag should be strings
    QString field;                      // key of a field should be string;
                                        // value of a field can be arithmetic types or string
    QString timestamp;                  // timestamp should be an unsigned long

public:
    void addTagPair(QString key, QString value);
    template<typename T>
    void addFieldPair(QString key, T value);

    QString& getMeasurement();
    QString& getTag();
    QString& getField();
    QString& getTimestampString();
    quint64 getTimestamp();
    void setMeasurement(QString measureme);
    void setTag(QString tag);
    void setField(QString field);
    void setTimestampString(QString timestamp);
    void setTimestamp(quint64 timestamp);
};

class InfluxDB {
private:
    DBConfig config;

    QNetworkAccessManager manager;
    QNetworkRequest request;

    QString buffer;
    quint32 buf_size;
    quint32 count;

    void getConfig();
    void makeRequest();
    void sendData(QString& data);

public:
    InfluxDB(quint32 size = 512);
    ~InfluxDB();
    void addData(DBRecord& record);
    const QString& getBuffer();
};

template<typename T>
void DBRecord::addFieldPair(QString key, T value) {}
