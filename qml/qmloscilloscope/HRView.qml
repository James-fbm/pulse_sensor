import QtQuick 2.0
import QtCharts 2.1
import QtQuick.Controls 1.0


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

    Rectangle {
        width: parent.width / 6
        height: parent.height / 4
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 10
        anchors.rightMargin: 10
        Text {
            anchors.fill: parent
            text: "heart rate"
        }
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
