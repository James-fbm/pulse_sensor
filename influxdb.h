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
                                    // value of a field can be arithmetic types or string
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
    ~InfluxDB();
    template<typename T>
    void addData(DBRecord<T>& record);
    void addData(DBRecord<QString>& record);
    const QString& getBuffer();
};


// This definition has to be put in a header file, or there will be a compilation error
template<typename T>
void InfluxDB::addData(DBRecord<T>& record) {

    // QString version has been specialized
    static_assert(std::is_arithmetic<T>::value,
            "Type T should be an arithmetic type or a QString");

    // Implementation for arithemtic types
    QString data("");

    data += record.measurement;

    // add tag keys and values
    for (auto t = record.tag.cbegin(); t != record.tag.cend() ; ++t) {
        data += ",";
        data += t.key();
        data += "=";
        data += t.value();
    }

    data += ' ';

    // add field keys and values
    auto f = record.field.cbegin();
    if (f == record.field.cend()) {

    } else {
        data += f.key();
        data += "=";
        data += QString::number(f.value());
        ++f;
        for (; f != record.field.cend() ; ++f) {
            data += ',';
            data += f.key();
            data += "=";
            data += QString::number(f.value());
        }
        data += ' ';
    }

    data += QString::number(record.timestamp);
    data += '\n';

    this->buffer += data;
    this->count += 1;

    // if buffer is full, send an Http request
    if (this->count == this->buf_size) {
        this->sendData(this->buffer);
        this->count = 0;
        this->buffer = "";
    }
}
