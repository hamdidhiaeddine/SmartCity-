#ifndef JARDINSTATISTICS_H
#define JARDINSTATISTICS_H

#include <QDialog>
#include <QChart>
#include <QChartView>
#include <QList>
#include <QString>

struct JardinStat {
    int id;
    QString emplacement;
    QString superficie;
    QString typeSol;
    QString typeChoix;
};

class JardinStatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JardinStatisticsDialog(QWidget *parent = nullptr);
    void setJardinsData(const QList<JardinStat> &data);

private:
    void displayStatistics();
    void createPieChart();
    void createBarChart();
    void createDetailedStats();

    QList<JardinStat> jardins;
    QChartView *chartView;
    QChart *chart;
};

#endif // JARDINSTATISTICS_H
