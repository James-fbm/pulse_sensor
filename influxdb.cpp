#include "influxdb.h"

InfluxDB::InfluxDB(quint32 size): buf_size(size), count(0) {
    getConfig();
    makeRequest();
}

void InfluxDB::getConfig() {
    QFile file(":/database/influxdb.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open influxdb.json";
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonObject = document.object();

    this->config.org = jsonObject["org"].toString().trimmed();
    this->config.bucket = jsonObject["bucket"].toString().trimmed();
    this->config.precision = jsonObject["precision"].toString().trimmed();
    this->config.url = jsonObject["url"].toString().trimmed();
    this->config.token = jsonObject["token"].toString().trimmed();
}

void InfluxDB::makeRequest() {

    QUrl url(this->config.url);
    QUrl endpoint("/api/v2/write");
    url = url.resolved(endpoint);

    QUrlQuery query;
    query.addQueryItem("org", this->config.org);
    query.addQueryItem("bucket", this->config.bucket);
    query.addQueryItem("precision", this->config.precision);

    url.setQuery(query);

    this->request.setUrl(url);

    this->request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

    QString token = QString("Token ") + this->config.token;
    this->request.setRawHeader("Authorization", token.toUtf8());

}

void InfluxDB::sendData(QString& data) {
    QByteArray bdata = data.toUtf8();
    QNetworkReply *reply = this->manager.post(this->request, bdata);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Success:" << reply->readAll();
    } else {
        qDebug() << "Error:" << reply->errorString();
    }

    qDebug() << this->buffer;
    reply->deleteLater();
}

void InfluxDB::addData(QString& measurement, QMap<QString, QString>& tag,
             QMap<QString, QString>& field, QString& timestamp) {

    QString data("");

    data += measurement;

    for (auto t = tag.cbegin(); t != tag.cend() ; ++t) {
        data += ",";
        data += t.key();
        data += "=";
        data += t.value();
    }

    data += ' ';

    auto f = field.cbegin();
    if (f == field.cend()) {

    } else {
        data += f.key();
        data += "=";
        data += f.value();
        ++f;
        for (; f != field.cend() ; ++f) {
            data += ',';
            data += f.key();
            data += "=";
            data += f.value();
        }
    }

    data += ' ';
    data += timestamp;
    data += '\n';

    this->buffer += data;
    this->count += 1;

    if (this->count == this->buf_size) {
        this->sendData(this->buffer);
        this->count = 0;
        this->buffer = "";
    }
}

void InfluxDB::addDataR(DBRecord<QString>& record) {
    // Specialized implementation for QString
    qDebug() << "QString record";
}

const QString& InfluxDB::getBuffer() {
    return this->buffer;
}
