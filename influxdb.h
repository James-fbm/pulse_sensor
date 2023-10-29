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
template<typename T = double>
struct DBRecord {
    QString measurement;                // measurement should be a string
    QMap<QString, QString> tag;         // key and value of a tag should be strings
    QMap<QString, T> ar_field;          // key of a field should be string;
    QMap<QString, QString> str_field;   // value of a field can be arithmetic types or string
    quint64 timestamp;                  // timestamp should be an unsigned long

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
    const QString& getBuffer();
};


// This definition has to be put in a header file, or there will be a compilation error
template<typename T>
void InfluxDB::addData(DBRecord<T>& record) {

    static_assert(std::is_arithmetic<T>::value,
            "Type T should be an arithmetic type");

    // Implementation for arithemtic types
    QString data("");

    data += record.measurement;

    // Add tag keys and values
    for (auto titer = record.tag.cbegin(); titer != record.tag.cend() ; ++titer) {
        data += ",";
        data += titer.key();
        data += "=";
        data += titer.value();
    }

    // End of measurement and tag segment
    data += ' ';

    // Add field keys and values
    auto ar_fiter = record.ar_field.cbegin();
    auto str_fiter = record.str_field.cbegin();
    bool has_field = true;

    // According to the data format, field segment should begin with a valid K-V pair
    if (ar_fiter != record.ar_field.cend()) {
        data += ar_fiter.key();
        data += "=";
        data += QString::number(ar_fiter.value());
        ++ar_fiter;
    } else if (str_fiter != record.str_field.cend()) {
        data += str_fiter.key();
        data += "=\"";                  // string value needs to be surrounded by `"`
        data += str_fiter.value();
        data += '\"';
        ++str_fiter;
    } else {
        has_field = false;
    }

    for (; ar_fiter != record.ar_field.cend() ; ++ar_fiter) {
        data += ',';
        data += ar_fiter.key();
        data += "=";
        data += QString::number(ar_fiter.value());
    }

    for (; str_fiter != record.str_field.cend() ; ++str_fiter) {
        data += ',';
        data += str_fiter.key();
        data += "=\"";                  // string value needs to be surrounded by `"`
        data += str_fiter.value();
        data += '\"';
    }

    // End of field segment. If no field to be written, a new space should be ignored
    if (has_field)
        data += ' ';

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
