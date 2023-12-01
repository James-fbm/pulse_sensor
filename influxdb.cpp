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
    QUrl read_endpoint("/api/v2/query");
    QUrl write_endpoint("/api/v2/write");
    QUrl read_url = url.resolved(read_endpoint);
    QUrl write_url = url.resolved(write_endpoint);

    QUrlQuery read_query, write_query;
    read_query.addQueryItem("org", this->config.org);
    write_query.addQueryItem("org", this->config.org);
    write_query.addQueryItem("bucket", this->config.bucket);
    write_query.addQueryItem("precision", this->config.precision);

    read_url.setQuery(read_query);
    write_url.setQuery(write_query);

    this->read_request.setUrl(read_url);
    this->write_request.setUrl(write_url);

    this->read_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.flux");
    this->write_request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

    QString token = QString("Token ") + this->config.token;
    this->read_request.setRawHeader("Authorization", token.toUtf8());
    this->read_request.setRawHeader("Accept", "application/csv");
    this->write_request.setRawHeader("Authorization", token.toUtf8());

}

void InfluxDB::sendData(QString& data) {
    // See the request data format at https://docs.influxdata.com/influxdb/v2/write-data/developer-tools/api/
    // The `data` variable represents the --data-binary part of the curl command.
    // Use QString for flexibility
    qDebug() << "Prepare sending data length: " << data.size() << "bytes";
//    qDebug() << data;
    QByteArray bdata = data.toUtf8();
    QNetworkReply* reply = this->manager.post(this->write_request, bdata);
    QObject::connect(reply, &QNetworkReply::finished, this,
                     [this, reply](){
        this->handleWriteReply(reply);
    });
}

void InfluxDB::handleWriteReply(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Success:" << reply->readAll();
    } else {
        qDebug() << "Error:" << reply->errorString();
    }
    reply->deleteLater();
}

void InfluxDB::queryData() {
    QString query_flux = "from(bucket:\"db_pulse_sensor\") |> range(start: -20d) "
        "|> filter(fn: (r) => r._measurement == \"hrr\")";

    QNetworkReply* reply = this->manager.post(this->read_request, query_flux.toUtf8());

    QNetworkRequest sentRequest = reply->request();

    QList<QByteArray> headerList = sentRequest.rawHeaderList();
    for (const QByteArray &header : headerList) {
        qDebug() << header << ": " << sentRequest.rawHeader(header);
    }
    qDebug() << sentRequest.url();

    QEventLoop eventLoop;
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Success:" << reply->readAll();

    } else {
        qDebug() << "Error:" << reply->errorString();
    }
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
