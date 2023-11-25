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


DataSource::DataSource(QQuickView *appViewer, QObject *parent) :
    QObject(parent),
    m_appViewer(appViewer),
    m_index(-1)
{
    qRegisterMetaType<QAbstractSeries*>();
    qRegisterMetaType<QAbstractAxis*>();

    m_data.append(QVector<QPointF>()); // Initialize with an empty QVector
}


void DataSource::update(QAbstractSeries *series)
{
    if (series) {
        QXYSeries *xySeries = static_cast<QXYSeries *>(series); //This line cast the series pointer from the type QAbstractSeries to the type QXYSeries. To manipulate the data, we need to work with a specific series type, QXYSeries in this case, which is used for XY charts
        m_index++;
        if (m_index > m_data.count() - 1)
            m_index = 0;

        QVector<QPointF> points = m_data.at(m_index);
        // Use replace instead of clear + append, it's optimized for performance
        xySeries->replace(points);



        // Find the min and max Y values
        qreal minYValue = std::numeric_limits<qreal>::max();
        qreal maxYValue = std::numeric_limits<qreal>::min();
        for (const QPointF &point : qAsConst(points)) {
            minYValue = qMin(point.y(), minYValue);
            maxYValue = qMax(point.y(), maxYValue);
        }
        qreal margin = (maxYValue - minYValue) * 0.05; // 5% margin
        minYValue -= margin;
        maxYValue += margin;

        // Update the axes range
        auto axes = xySeries->attachedAxes(); //An XY series typically has two axes, one for the X values and one for the Y values.
        //.The actual type of axes will be QList<QAbstractAxis *>
        qreal newX = (points.isEmpty() ? 0 : points.last().x() + 1);
        if (newX <= 60) {
            newX = 60;
        }
        for (QAbstractAxis *axis : axes) {
            if (axis->orientation() == Qt::Vertical) { // Y轴
                QValueAxis *valueAxisY = static_cast<QValueAxis *>(axis); //subclass of QAbstractAxis
                valueAxisY->setRange(minYValue, maxYValue);
            } else if (axis->orientation() == Qt::Horizontal) { // X轴
                QValueAxis *valueAxisX = static_cast<QValueAxis *>(axis);
                valueAxisX->setRange(newX - 60, newX);
            }
        }
    }


}


void DataSource::updateHeartRate(int heartRate)
{
    QVector<QPointF> &points = m_data.first();

    // Add the new point
    qreal newX = (points.isEmpty() ? 0 : points.last().x() + 1);
    points.append(QPointF(newX, heartRate));

    // Keep only the latest 1024 data points
    if (points.length() > 60) {
        points.pop_front();
    }

//    // Update the chart with the new data
//    if (m_appViewer->rootObject()) { //The rootObject is the top-level object in the QML document loaded into the QQuickView

//        auto ecgChartView = m_appViewer->rootObject()->findChild<QObject *>("ecgChartView");// This declares a list to hold pointers to QAbstractSeries objects

//        if (ecgChartView) {
//            auto seriesList = ecgChartView->findChildren<QAbstractSeries *>();
//            QXYSeries *xySeries = static_cast<QXYSeries *>(seriesList.first());
//            xySeries->replace(points);

//            // Find the min and max Y values
//            qreal minYValue = std::numeric_limits<qreal>::max();
//            qreal maxYValue = std::numeric_limits<qreal>::min();
//            for (const QPointF &point : qAsConst(points)) {
//                minYValue = qMin(point.y(), minYValue);
//                maxYValue = qMax(point.y(), maxYValue);
//            }
//            qreal margin = (maxYValue - minYValue) * 0.05; // 5% margin
//            minYValue -= margin;
//            maxYValue += margin;

//            // Update the axes range
//            auto axes = xySeries->attachedAxes(); //An XY series typically has two axes, one for the X values and one for the Y values.
//            //.The actual type of axes will be QList<QAbstractAxis *>

//            for (QAbstractAxis *axis : axes) {
//                if (axis->orientation() == Qt::Vertical) { // Y轴
//                    QValueAxis *valueAxisY = static_cast<QValueAxis *>(axis); //subclass of QAbstractAxis
//                    valueAxisY->setRange(minYValue, maxYValue);
//                } else if (axis->orientation() == Qt::Horizontal) { // X轴
//                    QValueAxis *valueAxisX = static_cast<QValueAxis *>(axis);
//                    valueAxisX->setRange(newX - 60, newX);
//                }
//            }
//        }
//    }
}

