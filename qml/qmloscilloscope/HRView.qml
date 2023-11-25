import QtQuick 2.0
import QtCharts 2.1


ChartView {
    id: hrChartView
    animationOptions: ChartView.NoAnimation
    theme: ChartView.ChartThemeLight

    ValueAxis {
        id: axisY
        min: 0
        max: 200
    }


    ValueAxis {
        id: axisX
        min: 0
        max: 60
    }

    LineSeries {
        id: lineSeries1
        name: "Heart Rate"
        axisX: axisX
        axisY: axisY
        useOpenGL: true
    }


//    Timer {
//        id: refreshTimer
//        interval: 1 / 60 * 1000 // 60 Hz
//        running: true
//        repeat: true
//        onTriggered: {
//            dataSource.update(hrView.series(0));
//            dataSource.update(hrView.series(1));
//            updateXAxisRange(); // 更新X轴的范围
//        }
//    }
}
