import QtQuick 2.0
import QtCharts 2.1


ChartView {
    id: chartView
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

//    function updateXAxisRange() {
//        // 此处我们假设数据是连续的，并且以固定的速率生成
//        // 更新X轴的范围以显示最新的数据点
//        // 例如，如果每次刷新时您都添加一个新的数据点
//        var lastXValue = lineSeries1.at(lineSeries1.count - 1).x; // 获取最后一个数据点的X值
//        if (lastXValue > axisX.max) {
//            axisX.min = lastXValue - 60; // 保持显示的范围为60个单位
//            axisX.max = lastXValue;
//        }
//    }

}
