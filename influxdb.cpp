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
//    qDebug() << "Prepare sending data length: " << data.size() << "bytes";
//    qDebug() << data;
    QByteArray bdata = data.toUtf8();
    QNetworkReply* reply = this->manager.post(this->request, bdata);
    QObject::connect(reply, &QNetworkReply::finished, this,
                     [this, reply](){
        this->handleReply(reply);
    });
}

void InfluxDB::handleReply(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Success:" << reply->readAll();
    } else {
        qDebug() << "Error:" << reply->errorString();
    }
    reply->deleteLater();
}

void InfluxDB::addData(DBRecord record) {
    this->buffer += record.getMeasurement();
    if (record.getTag().size() != 0) {
        this->buffer += ',';
    }
    this->buffer += record.getTag();
    this->buffer += ' ';                        // split measurement + tag with field
    this->buffer += record.getField();
    if (record.getField().size() != 0) {
        this->buffer +=  ' ';
    }
    this->buffer += record.getTimestampString();
    this->buffer += '\n';
    ++this->count;

    if (this->count >= this->buf_size) {
        this->sendData(this->buffer);
        this->buffer = "";
        this->count = 0;
    }
}

const QString& InfluxDB::getBuffer() {
    return this->buffer;
}


void DBRecord::addFieldPair(QPair<QString, QString>& pair) {
    if (this->field.size() != 0) {
        this->field += ',';
    }
    this->field += pair.first;
    this->field += '=';
    this->field += '\"';
    this->field += pair.second;
    this->field += '\"';
}

void DBRecord::addFieldPair(QPair<QString, QString>&& pair) {
    this->addFieldPair(pair);
}

void DBRecord::addTagPair(QPair<QString, QString>& pair) {
    if (this->tag.size() != 0) {
        this->tag += ',';
    }
    this->tag += pair.first;
    this->tag += '=';
    this->tag += pair.second;
}

void DBRecord::addTagPair(QPair<QString, QString>&& pair) {
    this->addTagPair(pair);
}

const QString& DBRecord::getMeasurement() { return this->measurement; }
const QString& DBRecord::getTag() { return this->tag; }
const QString& DBRecord::getField() { return this->field; }
const QString& DBRecord::getTimestampString() { return this->timestamp; }
quint64 DBRecord::getTimestamp() { return this->timestamp.toULongLong(); }
void DBRecord::setMeasurement(QString& measurement) {
    this->measurement = measurement;
}
void DBRecord::setMeasurement(QString&& measurement) {
    this->setMeasurement(measurement);
}
void DBRecord::setTag(QString& tag){
    this->tag = tag;
}
void DBRecord::setTag(QString&& tag){
    this->setTag(tag);
}
void DBRecord::setField(QString& field) {
    this->field = field;
}
void DBRecord::setField(QString&& field) {
    this->setField(field);
}
void DBRecord::setTimestampString(QString& timestamp) {
    this->timestamp = timestamp;
}
void DBRecord::setTimestampString(QString&& timestamp) {
    this->setTimestampString(timestamp);
}
void DBRecord::setTimestamp(quint64 timestamp) {
    this->setTimestampString(QString::number(timestamp));
}
