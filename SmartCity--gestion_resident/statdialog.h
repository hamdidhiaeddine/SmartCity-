#ifndef STATDIALOG_H
#define STATDIALOG_H

#include <QDialog>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QSqlQueryModel>

class StatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatDialog(QSqlQueryModel* model, QWidget *parent = nullptr);
    ~StatDialog();

private:
    QChart* createPieChart(QSqlQueryModel* model);
    QChartView* chartView;
};

#endif // STATDIALOG_H
