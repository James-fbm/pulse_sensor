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

import QtQuick 2.0
import QtCharts 2.1

//![1]
ChartView {
    id: ecgChartView
    animationOptions: ChartView.NoAnimation
    theme: ChartView.ChartThemeDark

    ValueAxis {
        id: axisY
        min: 95
        max: 150
        labelsVisible: false
        gridVisible: false
        tickCount: 2
    }


    ValueAxis {
        id: axisX
        min: 0
        max: 1024
        labelsVisible: false
        gridVisible: false
        tickCount: 2
    }

    LineSeries {
        id: lineSeries
        name: "Signal Strength"
        axisX: axisX
        axisY: axisY
        useOpenGL: true
    }


    Timer { // A QML type that triggers an action at regular intervals.
        id: refreshTimer
        interval: 1 / 60 * 1000 // 60 Hz
        running: true
        repeat: true
        onTriggered: { //The handler that is called every time the timer interval elapses.
            dataSource.update(ecgChartView.series(0));
            dataSource.update(ecgChartView.series(1));
            //updateXAxisRange(); // 更新X轴的范围
            //updateYAxisRange();
        }
    }

    function updateXAxisRange() {
        // 此处我们假设数据是连续的，并且以固定的速率生成
        // 更新X轴的范围以显示最新的数据点
        // 例如，如果每次刷新时您都添加一个新的数据点
        var lastXValue = lineSeries.at(lineSeries.count - 1).x; // 获取最后一个数据点的X值
        if (lastXValue > axisX.max) {
            axisX.min = lastXValue - 1024; // 保持显示的范围为1024个单位
            axisX.max = lastXValue;
        }
    }

    function updateYAxisRange() {
        var minY = Number.MAX_VALUE;
        var maxY = -Number.MAX_VALUE;

        for (var i = 0; i < lineSeries.count; i++) {
            var point = lineSeries.at(i); // Access each point in the LineSeries
            minY = Math.min(minY, point.y); // Find the minimum Y value
            maxY = Math.max(maxY, point.y); // Find the maximum Y value
        }

        // Add a small margin to the min and max values to ensure that all points are visible
        var margin = (maxY - minY) * 0.05; // 5% margin
        minY -= margin;
        maxY += margin;

        // Update the Y axis range to fit all data points
        axisY.min = minY;
        axisY.max = maxY;
    }


}
