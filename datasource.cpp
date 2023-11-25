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
    minY(std::numeric_limits<qreal>::max()),        // reverse initialize in order to guarantee update
    maxY(std::numeric_limits<qreal>::min())
{

    qRegisterMetaType<QAbstractSeries*>();
    qRegisterMetaType<QAbstractAxis*>();

    qint64 ms = QDateTime::currentMSecsSinceEpoch();
    minX = ms, maxX = ms;

    // initialize all the X-axis index with a valid timestamp
    for (qint32 i = 0; i < DISPLAYBUFSIZE; ++i) {
        points.append(QPointF(ms, 0));
    }
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
        valueAxisX->setRange(QDateTime::fromMSecsSinceEpoch(minX),
                             QDateTime::fromMSecsSinceEpoch(maxX));

        auto valueAxisY = static_cast<QValueAxis*>(axis[1]);
        valueAxisY->setRange(minY, maxY);
    }
}

void DataSource::updateHeartRate(int heartRate)
{
    // fetch current time as X-axis index
    qint64 ms = QDateTime::currentMSecsSinceEpoch();

    points.append(QPointF(ms, heartRate));
    // there always will be DISPLAYBUFSIZE elements
    points.pop_front();

    minX = points.front().x();
    maxX = points.back().x();
    qDebug() << minX << ' ' << ms;

    qreal newY = heartRate;
    minY = newY < minY ? newY : minY;
    maxY = newY > maxY ? newY : maxY;

}

