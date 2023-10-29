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
template<typename T>
struct DBRecord {
    QString measurement;            // measurement should be a string
    QMap<QString, QString> tag;     // key and value of a tag should be strings
    QMap<QString, T> field;         // key of a field should be string;
                                    // value of a field can be arithmeti types or string, ...
    quint64 timestamp;              // timestamp should be an unsigned long

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
    void addData(QString& measurement, QMap<QString, QString>& tag,
                 QMap<QString, QString>& field, QString& timestamp);
    template<typename T>
    void addDataR(DBRecord<T>& record);
    void addDataR(DBRecord<QString>& record);
    const QString& getBuffer();
};


// This definition has to be put in a header file, or there will be a compilation error
template<typename T>
void InfluxDB::addDataR(DBRecord<T>& record) {
    // Implementation for general case
    static_assert(std::is_arithmetic<T>::value,
            "DBRecord only accepts arithmetic types or a QString");
    qDebug() << "arithmetic record";
}
