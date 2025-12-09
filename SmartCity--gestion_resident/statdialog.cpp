#include "statdialog.h"
#include <QVBoxLayout>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QSqlQueryModel>
#include <QPainter>

StatDialog::StatDialog(QSqlQueryModel* model, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Statistiques - Spécialités"));
    setGeometry(100, 100, 800, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Create pie chart
    QChart* chart = createPieChart(model);
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(chartView);
    setLayout(layout);
}

StatDialog::~StatDialog()
{
}

QChart* StatDialog::createPieChart(QSqlQueryModel* model)
{
    QChart* chart = new QChart();
    chart->setTitle(tr("Distribution par Spécialité"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QPieSeries* series = new QPieSeries();

    if (model && model->rowCount() > 0) {
        for (int row = 0; row < model->rowCount(); ++row) {
            QString specialite = model->data(model->index(row, 0)).toString();
            int nombre = model->data(model->index(row, 1)).toInt();

            QPieSlice* slice = new QPieSlice(specialite, nombre);
            slice->setLabel(QString("%1 (%2)").arg(specialite).arg(nombre));
            slice->setLabelVisible(true);
            series->append(slice);
        }
    }

    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    return chart;
}
