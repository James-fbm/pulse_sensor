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
#include <QtCharts/QXYSeries>
#include <QtCharts/QAreaSeries>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QDebug>
#include <QtCore/QRandomGenerator>
#include <QtCore/QtMath>

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

}


void DataSource::updateSeries(QAbstractSeries *series)
{
    if (series) {
        QXYSeries *xySeries = static_cast<QXYSeries *>(series); //This line cast the series pointer from the type QAbstractSeries to the type QXYSeries. To manipulate the data, we need to work with a specific series type, QXYSeries in this case, which is used for XY charts

        xySeries->replace(points);

        // Update the axes range
        auto axes = xySeries->attachedAxes(); //An XY series typically has two axes, one for the X values and one for the Y values.
        //.The actual type of axes will be QList<QAbstractAxis *>

        for (QAbstractAxis *axis : axes) {
            if (axis->orientation() == Qt::Vertical) { // Y轴
                QValueAxis *valueAxisY = static_cast<QValueAxis *>(axis); //subclass of QAbstractAxis
                valueAxisY->setRange(minY, maxY);
            } else if (axis->orientation() == Qt::Horizontal) { // X轴
                qreal newX = (points.isEmpty() ? 0 : points.last().x() + 1);
                if (newX <= 1024) {
                    newX = 1024;
                }
                QValueAxis *valueAxisX = static_cast<QValueAxis *>(axis);
                valueAxisX->setRange(newX - 1024, newX);
            }
        }
    }
}

void DataSource::updateHeartRate(int heartRate)
{
    qDebug() << heartRate;

    // Add the new point
    qreal newX = (points.isEmpty() ? 0 : points.last().x() + 1);
    points.append(QPointF(newX, heartRate));

    qreal newY = heartRate;
    minY = newY < minY ? newY : minY;
    maxY = newY > maxY ? newY : maxY;

    // Keep only the latest 1024 data points
    if (points.length() > 1024) {
        points.pop_front();
    }
}

