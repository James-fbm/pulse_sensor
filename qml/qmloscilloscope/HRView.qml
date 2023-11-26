import QtQuick 2.0
import QtCharts 2.1


ChartView {
    id: hrChartView
    animationOptions: ChartView.NoAnimation
    theme: ChartView.ChartThemeLight

    ValueAxis {
        id: axisY
        tickCount: 5
        titleText: "BPM"
    }


    DateTimeAxis {
        id: axisX
        tickCount: 2
    }

    LineSeries {
        id: lineSeries
        name: "Heart Rate"
        axisX: axisX
        axisY: axisY
        useOpenGL: true
    }


    Timer {
        id: refreshTimer
        interval: 1000 // 1 Hz
        running: true
        repeat: true
        onTriggered: {
            dataSource.updateHRSeries(lineSeries);
        }
    }
}
