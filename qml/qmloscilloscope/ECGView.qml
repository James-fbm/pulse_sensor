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
            dataSource.updateSeries(ecgChartView.series(0));
            dataSource.updateSeries(ecgChartView.series(1));
        }
    }
}
