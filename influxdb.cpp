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

    qDebug() << "Sent data: " << this->buffer << "bytes";

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Success:" << reply->readAll();
    } else {
        qDebug() << "Error:" << reply->errorString();
    }

    qDebug() << "Sent data length:" << this->buffer.size() << "bytes";
    reply->deleteLater();
}

void InfluxDB::addData(DBRecord& record) {
    this->buffer += record.measurement;
    if (record.tag.size() != 0) {
        this->buffer += ',';
    }
    this->buffer += record.tag;
    this->buffer += ' ';                        // split measurement + tag with field
    this->buffer += record.field;
    if (record.field.size() != 0) {
        this->buffer +=  ' ';
    }
    this->buffer += record.timestamp;
}

const QString& InfluxDB::getBuffer() {
    return this->buffer;
}


void DBRecord::addTagPair(QString key, QString value) {}

QString& DBRecord::getMeasurement() { return this->measurement; }
QString& DBRecord::getTag() { return this->tag; }
QString& DBRecord::getField() { return this->field; }
QString& DBRecord::getTimestampString() { return this->timestamp; }
quint64 DBRecord::getTimestamp() { return this->timestamp.toULongLong(); }
void DBRecord::setMeasurement(QString measurement) {}
void DBRecord::setTag(QString tag){}
void DBRecord::setField(QString field) {}
void DBRecord::setTimestamp(quint64 timestamp) {}
void DBRecord::setTimestampString(QString timestamp) {}
