#include "influxdb.h"

InfluxDB::InfluxDB(quint32 size): buf_size(size), count(0) {
    getConfig();
    makeRequest();
}

InfluxDB::~InfluxDB() {
    // send last buffered data before destruction
    this->sendData(this->buffer);
}

void InfluxDB::getConfig() {

    // Open database configuration file
    QFile file(":/database/influxdb.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open influxdb.json";
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonObject = document.object();

    // Read and store all the attributes
    this->config.org = jsonObject["org"].toString().trimmed();
    this->config.bucket = jsonObject["bucket"].toString().trimmed();
    this->config.precision = jsonObject["precision"].toString().trimmed();
    this->config.url = jsonObject["url"].toString().trimmed();
    this->config.token = jsonObject["token"].toString().trimmed();
}

void InfluxDB::makeRequest() {

    // Intialize an Http request for writing database
    // Use request url and header with configuration attributes stored by getConfig()
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

    // See the request data format at https://docs.influxdata.com/influxdb/v2/write-data/developer-tools/api/
    // The `data` variable represents the --data-binary part of the curl command.
    // Use QString for flexibility
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

    qDebug() << "Sent data length:" << this->buffer.size() << "bytes";
    reply->deleteLater();
}


void InfluxDB::addData(DBRecord<QString>& record) {

    // Specialized implementation for QString
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
        data += "=\"";          // string value needs to be surrounded by `"`
        data += f.value();
        data += '\"';
        ++f;
        for (; f != record.field.cend() ; ++f) {
            data += ',';
            data += f.key();
            data += "=\"";          // string value needs to be surrounded by `"`
            data += f.value();
            data += '\"';
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

const QString& InfluxDB::getBuffer() {
    return this->buffer;
}

