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

#include "datasource.h"

QT_CHARTS_USE_NAMESPACE

Q_DECLARE_METATYPE(QAbstractSeries *)
Q_DECLARE_METATYPE(QAbstractAxis *)


DataSource::DataSource(QQuickView *_appViewer, QObject *parent) :
    QObject(parent),
    appViewer(_appViewer),
    minY_ECG(std::numeric_limits<qint64>::max()),        // reverse initialize in order to guarantee update
    maxY_ECG(std::numeric_limits<qint64>::min()),
    minY_HR(std::numeric_limits<qint64>::max()),
    maxY_HR(std::numeric_limits<qint64>::min()),
    influx(8)
{

    qRegisterMetaType<QAbstractSeries*>();
    qRegisterMetaType<QAbstractAxis*>();

    qint64 ms = QDateTime::currentMSecsSinceEpoch();
    minX_ECG = ms, maxX_ECG = ms;
    minX_HR = ms, maxX_HR = ms;

    // write heart rate to database every 1000ms
    connect(&timer, &QTimer::timeout, this, &DataSource::writeHRToDatabase);
    timer.start(1000);
}


void DataSource::updateECGSeries(QLineSeries *series)
{
    if (series) {
        series->replace(points);        // update series points

        // Update axes
        auto axis = series->attachedAxes();

        // mins and maxs are updated in updateHeartRate
        auto valueAxisX = static_cast<QDateTimeAxis *>(axis[0]);
        valueAxisX->setFormat("yyyy-MM-dd hh:mm:ss:zzz");
        valueAxisX->setRange(QDateTime::fromMSecsSinceEpoch(minX_ECG),
                             QDateTime::fromMSecsSinceEpoch(maxX_ECG));

        auto valueAxisY = static_cast<QValueAxis*>(axis[1]);
        valueAxisY->setRange(minY_ECG, maxY_ECG);
    }
}

void DataSource::updateHRSeries(QLineSeries *series) {
    if (series) {
        series->replace(beats);        // update series points

        // Update axes
        auto axis = series->attachedAxes();

        // mins and maxs are updated in updateHeartRate
        auto valueAxisX = static_cast<QDateTimeAxis *>(axis[0]);
        valueAxisX->setFormat("yyyy-MM-dd hh:mm:ss:zzz");
        valueAxisX->setRange(QDateTime::fromMSecsSinceEpoch(minX_HR),
                             QDateTime::fromMSecsSinceEpoch(maxX_HR));

        auto valueAxisY = static_cast<QValueAxis*>(axis[1]);
        valueAxisY->setRange(minY_HR, maxY_HR);
    }
}

void DataSource::updateHeartRate(qint32 heartRate, qint64 ms)
{
    // suppress large fluctuations
    if (heartRate > UPPERTHRESHOLD)
        heartRate = UPPERTHRESHOLD;
    if (heartRate < LOWTHRESHOLD)
        heartRate = LOWTHRESHOLD;

    // if the buffer is full, drop the oldest element and reset minY and maxY
    if (points.size() >= ECGBUFFERSIZE) {
        qint64 dropY_ECG = points.front().y();
        points.pop_front();
        if (dropY_ECG <= minY_ECG || dropY_ECG >= maxY_ECG) {
            // if dropY reaches boundaries, recalculate minY and maxY
            minY_ECG = std::numeric_limits<qint64>::max();
            maxY_ECG = std::numeric_limits<qint64>::min();
            for (auto& p: points) {
                if (minY_ECG >= p.y()) {
                    minY_ECG = p.y();
                }
                if (maxY_ECG <= p.y()) {
                    maxY_ECG = p.y();
                }
            }
        }
    }

    // fetch current time as X-axis index
    // qint64 ms = QDateTime::currentMSecsSinceEpoch();
    points.append(QPointF(ms, heartRate));

    minX_ECG = points.front().x();
    maxX_ECG = points.back().x();

    qint64 newY_ECG = heartRate;
    minY_ECG = newY_ECG < minY_ECG ? newY_ECG : minY_ECG;
    maxY_ECG = newY_ECG > maxY_ECG ? newY_ECG : maxY_ECG;

    // calculate new BPM
    if (heartRate > BEATTHRESHOLD) {
        if (beatsWindow.isEmpty() || ms - beatsWindow.back() > 250) {
            beatsWindow.push_back(ms);

            // window size should be no more than 8
            if (beatsWindow.size() > 8) {
                beatsWindow.pop_front();
            }

            // calculating BPM requires at least 2 beat timestamps
            if (beatsWindow.size() > 1) {
                qint64 last_beat = beatsWindow.front();
                qint64 first_beat = beatsWindow.back();

                BPM = 60000 / (first_beat - last_beat) * (beatsWindow.size() - 1);

                if (beats.size() > HRBUFFERSIZE) {
                    auto dropY_HR = beats.front().y();
                    beats.pop_front();

                    // same as ECG
                    if (dropY_HR <= minY_HR || dropY_HR >= maxY_HR) {
                        minY_HR = std::numeric_limits<qint64>::max();
                        maxY_HR = std::numeric_limits<qint64>::min();
                        for (auto& b: beats) {
                            if (minY_HR >= b.y()) {
                                minY_HR = b.y();
                            }
                            if (maxY_HR <= b.y()) {
                                maxY_HR = b.y();
                            }
                        }
                    }
                }
                beats.append(QPointF(ms, BPM));

                minX_HR = beats.front().x();
                maxX_HR = beats.back().x();
                minY_HR = BPM <= minY_HR ? BPM : minY_HR;
                maxY_HR = BPM >= maxY_HR ? BPM : maxY_HR;
            }
        }
    }

}

void DataSource::writeHRToDatabase() {
    DBRecord r;
    qint64 ms = QDateTime::currentMSecsSinceEpoch();
    r.setMeasurement("hr");
    // r.addTagPair({"name", "pl"});
    r.addFieldPair<qint64>({"value", BPM});
    r.setTimestamp(ms);

    influx.addData(r);
}
