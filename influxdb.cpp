#include "influxdb.h"

InfluxDB::InfluxDB() {
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

    /*
    QByteArray data = "weather,location=us-midwest temperature=82 1697894400006";
    QNetworkReply *reply = this->manager.post(this->request, data);

    qDebug() << "connect";
    // 使用事件循环等待请求完成
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // 输出响应
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Success:" << reply->readAll();
    } else {
        qDebug() << "Error:" << reply->errorString();
    }

    // 删除reply对象
    reply->deleteLater();
    qDebug() << "reply" << reply->error();
    */
}

void InfluxDB::sendData(QString data) {
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

    reply->deleteLater();
}
