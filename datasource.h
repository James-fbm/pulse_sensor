/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DATASOURCE_H
#define DATASOURCE_H
#define ECGBUFFERSIZE 128
#define HRBUFFERSIZE 60
#define BEATTHRESHOLD 138
#define UPPERTHRESHOLD 160
#define LOWTHRESHOLD 90
#include <QtCore/QObject>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QValueAxis>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QLineSeries>
#include <QDebug>
#include <QTimer>
#include "influxdb.h"

QT_BEGIN_NAMESPACE
class QQuickView;
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class DataSource : public QObject
{

Q_OBJECT
public:
    explicit DataSource(QQuickView *appViewer, QObject *parent = 0);

Q_SIGNALS:

public slots:
    void updateHeartRate(qint32 heartRate, qint64 ms);
    void updateECGSeries(QLineSeries *series);
    void updateHRSeries(QLineSeries *series);
    void writeHRToDatabase();
    qint64 getBPM();
private:
    QQuickView *appViewer; // A pointer to the QQuickView instance, used to interact with the QML view
    QList<QPointF> points;
    QList<QPointF> beats;
    qint64 minX_ECG, maxX_ECG, minY_ECG, maxY_ECG;
    qint64 minX_HR, maxX_HR, minY_HR, maxY_HR;
    QList<qint64> beatsWindow;    // store the latest beats timestamps to update BPM
    InfluxDB influx;
    qint64 BPM;
    QTimer timer;
};

#endif // DATASOURCE_H
