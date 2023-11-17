#include "HeartRateMonitor.h"

HeartRateMonitor::HeartRateMonitor(QWidget *parent)
    : QMainWindow(parent), series(new QLineSeries()), chart(new QChart()), chartView(new QChartView(chart, this))
{
    //series->append(0, 0);
    chart->addSeries(series);
    chart->createDefaultAxes();
    // For X Axis
    foreach(QAbstractAxis *axis, chart->axes(Qt::Horizontal)) {
        axis->setRange(0, 60);
    }

    // For Y Axis
    foreach(QAbstractAxis *axis, chart->axes(Qt::Vertical)) {
        axis->setRange(95, 160);
    }


    chartView->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(chartView);
}

HeartRateMonitor::~HeartRateMonitor()
{
    delete series;
    delete chart;
    delete chartView;
}

void HeartRateMonitor::updateHeartRate(int rate)
{
    static int second = 1;
    series->append(second, rate);  // Append the new rate data

    // Adjust the X axis range to show the last 60 data points
    if (second >= 60) {
        QValueAxis *axisX = static_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
        axisX->setRange(second - 59, second);
        series->remove(0);  // Remove the oldest data point
    }

    second++;
}

