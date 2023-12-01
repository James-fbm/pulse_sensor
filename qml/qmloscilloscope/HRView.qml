import QtQuick 2.0
import QtCharts 2.1
import QtQuick.Controls 1.0

ChartView {
    id: hrChartView
    animationOptions: ChartView.NoAnimation
    theme: ChartView.ChartThemeLight

    Rectangle {
        width: 120
        height: 40
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 15
        anchors.rightMargin: 30

        HRBeat {
            id: heartBeat
        }
    }

    ValueAxis {
        id: axisY
        tickCount: 5
        titleText: "BPM"
    }


    DateTimeAxis {
        id: axisX
        tickCount: 2
        format: "MM-dd-yyyy hh:mm:ss:zzz"
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
            var BPM = dataSource.getBPM()
            dataSource.updateHRSeries(lineSeries);
            heartBeat.bpm = BPM
        }
    }
}
