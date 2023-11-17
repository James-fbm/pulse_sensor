#ifndef HEARTRATEMONITOR_H
#define HEARTRATEMONITOR_H

#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QValueAxis>

QT_CHARTS_USE_NAMESPACE

class HeartRateMonitor : public QMainWindow
{
    Q_OBJECT

public:
    HeartRateMonitor(QWidget *parent = nullptr);
    ~HeartRateMonitor();

public slots:
    void updateHeartRate(int rate);

private:
    QLineSeries *series;
    QChart *chart;
    QChartView *chartView;
};

#endif // HEARTRATEMONITOR_H
