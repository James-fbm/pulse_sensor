#ifndef INFLUXDB_H
#define INFLUXDB_H

#endif // INFLUXDB_H

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QPair>
#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QDebug>
#include <QThread>
#include <type_traits>
#include <QObject>

// map to json configuration file
struct DBConfig {
    QString org;
    QString bucket;
    QString precision;
    QString url;
    QString token;
};

// map to single record in InfluxDB
class DBRecord {
    QString measurement;                // measurement should be a string
    QString tag;                        // key and value of a tag should be strings
    QString field;                      // key of a field should be string;
                                        // value of a field can be arithmetic types or string
    QString timestamp;                  // timestamp should be an unsigned long

public:
    void addTagPair(QPair<QString, QString>& pair);
    void addTagPair(QPair<QString, QString>&& pair);
    template<typename T>
    void addFieldPair(QPair<QString, T>& pair);
    template<typename T>
    void addFieldPair(QPair<QString, T>&& pair);
    void addFieldPair(QPair<QString, QString>& pair);
    void addFieldPair(QPair<QString, QString>&& pair);

    const QString& getMeasurement();
    const QString& getTag();
    const QString& getField();
    const QString& getTimestampString();
    quint64 getTimestamp();
    void setMeasurement(QString& measurement);
    void setMeasurement(QString&& measurement);
    void setTag(QString& tag);
    void setTag(QString&& tag);
    void setField(QString& field);
    void setField(QString&& field);
    void setTimestampString(QString& timestamp);
    void setTimestampString(QString&& timestamp);
    void setTimestamp(quint64 timestamp);
};

class InfluxDB: public QObject {
    Q_OBJECT
public:
    InfluxDB(quint32 size = 512);
    ~InfluxDB();
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
private slots:
    void handleReply(QNetworkReply* reply);

public:
    void addData(DBRecord record);
    const QString& getBuffer();
};

template<typename T>
void DBRecord::addFieldPair(QPair<QString, T>& pair) {
    static_assert(std::is_arithmetic<T>::value,
            "Field values only accept arithmetic and string types.");
    if (this->field.size() != 0) {
        this->field += ',';
    }
    this->field += pair.first;
    this->field += '=';
    this->field += QString::number(pair.second);
}


template<typename T>
void DBRecord::addFieldPair(QPair<QString, T>&& pair) {
    this->addFieldPair(pair);
}
