#include "jardinstatistics.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QFont>
#include <QPainter>
#include <QTextEdit>

JardinStatisticsDialog::JardinStatisticsDialog(QWidget *parent)
    : QDialog(parent), chartView(nullptr), chart(nullptr)
{
    setWindowTitle(tr("Statistiques des Jardins"));
    setGeometry(100, 100, 1200, 700);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Onglets pour différents types de stats
    QTabWidget *tabWidget = new QTabWidget(this);

    // Onglet 1 : Graphique Circulaire (Pie Chart) par type de sol
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    tabWidget->addTab(chartView, tr("Répartition par type de sol (Pie)"));

    // Onglet 2 : Graphique en barres par type de choix
    QWidget *barChartWidget = new QWidget(this);
    QVBoxLayout *barLayout = new QVBoxLayout(barChartWidget);
    QChartView *barChartView = new QChartView(this);
    barChartView->setRenderHint(QPainter::Antialiasing);
    barLayout->addWidget(barChartView);
    tabWidget->addTab(barChartWidget, tr("Distribution par type de jardin"));

    // Onglet 3 : Statistiques textuelles détaillées
    QWidget *statsWidget = new QWidget(this);
    QVBoxLayout *statsLayout = new QVBoxLayout(statsWidget);

    QLabel *totalLabel = new QLabel(tr("Statistiques Globales des Jardins"), this);
    totalLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c5f2d;");
    statsLayout->addWidget(totalLabel);

    QLabel *countLabel = new QLabel(this);
    countLabel->setStyleSheet("font-size: 12px;");
    statsLayout->addWidget(countLabel);

    QLabel *idLabel = new QLabel(tr("Liste Complète des Jardins :"), this);
    idLabel->setStyleSheet("font-weight: bold; margin-top: 20px; color: #2c5f2d;");
    statsLayout->addWidget(idLabel);

    // Zone texte pour lister les jardins avec détails
    QTextEdit *jardinList = new QTextEdit(this);
    jardinList->setReadOnly(true);
    jardinList->setWordWrapMode(QTextOption::WordWrap);
    jardinList->setStyleSheet("background: #f0f8f0; color: #1a1a1a; padding: 10px; border: 2px solid #2c5f2d; border-radius: 5px;");
    statsLayout->addWidget(jardinList);

    tabWidget->addTab(statsWidget, tr("Détails & Informations"));

    mainLayout->addWidget(tabWidget);

    // Bouton fermer
    QPushButton *closeBtn = new QPushButton(tr("Fermer"), this);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "    background: #2c5f2d;"
        "    color: white;"
        "    border: 2px solid #4a7c4e;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover { background: #4a7c4e; color: white; }"
    );
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeBtn);

    setLayout(mainLayout);
}

void JardinStatisticsDialog::setJardinsData(const QList<JardinStat> &data)
{
    jardins = data;
    displayStatistics();
}

void JardinStatisticsDialog::displayStatistics()
{
    if (jardins.isEmpty()) {
        return;
    }

    createPieChart();
    createBarChart();
    createDetailedStats();
}

void JardinStatisticsDialog::createPieChart()
{
    if (chart) {
        delete chart;
    }

    chart = new QChart();
    chart->setTitle(tr("Répartition par type de sol"));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundBrush(QBrush(QColor(240, 248, 240)));
    chart->setTitleBrush(QBrush(QColor(44, 95, 45)));

    QPieSeries *series = new QPieSeries();

    // Compter les jardins par type de sol
    QMap<QString, int> soilTypes;
    for (const JardinStat &j : jardins) {
        if (!j.typeSol.isEmpty()) {
            soilTypes[j.typeSol]++;
        }
    }

    // Palette de couleurs vertes
    QList<QColor> colors;
    colors.append(QColor(44, 95, 45));      // Vert foncé
    colors.append(QColor(74, 124, 78));     // Vert moyen
    colors.append(QColor(106, 168, 79));    // Vert clair
    colors.append(QColor(148, 193, 31));    // Vert citron
    colors.append(QColor(184, 207, 69));    // Vert pâle

    int colorIndex = 0;
    for (auto it = soilTypes.constBegin(); it != soilTypes.constEnd(); ++it) {
        if (it.value() > 0) {
            QPieSlice *slice = new QPieSlice(it.key(), it.value());
            slice->setLabel(QString("%1: %2").arg(it.key()).arg(it.value()));
            slice->setLabelVisible(true);
            slice->setColor(colors[colorIndex % colors.size()]);
            slice->setLabelColor(QColor(255, 255, 255));
            series->append(slice);
            colorIndex++;
        }
    }

    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor(44, 95, 45));

    chartView->setChart(chart);
}

void JardinStatisticsDialog::createBarChart()
{
    QChart *barChart = new QChart();
    barChart->setTitle(tr("Distribution par type de jardin"));
    barChart->setBackgroundBrush(QBrush(QColor(240, 248, 240)));
    barChart->setTitleBrush(QBrush(QColor(44, 95, 45)));

    // Compter les jardins par type
    QMap<QString, int> typeCounts;
    for (const JardinStat &j : jardins) {
        if (!j.typeChoix.isEmpty()) {
            typeCounts[j.typeChoix]++;
        }
    }

    QBarSet *set0 = new QBarSet(tr("Nombre"));
    set0->setColor(QColor(74, 124, 78));
    QStringList categories;

    for (auto it = typeCounts.constBegin(); it != typeCounts.constEnd(); ++it) {
        *set0 << it.value();
        categories << it.key();
    }

    QBarSeries *series = new QBarSeries();
    series->append(set0);

    barChart->addSeries(series);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor(44, 95, 45));
    barChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsColor(QColor(44, 95, 45));
    barChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    barChart->legend()->setVisible(true);
    barChart->legend()->setAlignment(Qt::AlignBottom);
    barChart->legend()->setLabelColor(QColor(44, 95, 45));

    // Trouver la QChartView du deuxième onglet et lui assigner le chart
    QTabWidget *tabWidget = findChild<QTabWidget*>();
    if (tabWidget && tabWidget->count() > 1) {
        QWidget *barChartWidget = tabWidget->widget(1);
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(barChartWidget->layout());
        if (layout && layout->count() > 0) {
            QChartView *barChartView = qobject_cast<QChartView*>(layout->itemAt(0)->widget());
            if (barChartView) {
                barChartView->setChart(barChart);
                barChartView->setRenderHint(QPainter::Antialiasing);
            }
        }
    }
}

void JardinStatisticsDialog::createDetailedStats()
{
    QTabWidget *tabWidget = findChild<QTabWidget*>();
    if (!tabWidget || tabWidget->count() < 3) return;

    QWidget *statsWidget = tabWidget->widget(2);
    QVBoxLayout *statsLayout = qobject_cast<QVBoxLayout*>(statsWidget->layout());
    if (!statsLayout) return;

    // Trouver le QLabel de comptage
    QLabel *countLabel = nullptr;
    for (int i = 0; i < statsLayout->count(); ++i) {
        QLabel *label = qobject_cast<QLabel*>(statsLayout->itemAt(i)->widget());
        if (label && label->text().isEmpty()) {
            countLabel = label;
            break;
        }
    }

    if (countLabel) {
        int total = jardins.size();
        int potagers = 0;
        int ornementals = 0;
        
        for (const JardinStat &j : jardins) {
            if (j.typeChoix.toLower().contains("potager")) potagers++;
            else if (j.typeChoix.toLower().contains("ornemental")) ornementals++;
        }

        QString stats = QString(
            "Total des jardins: %1\n"
            "Jardins Potagers: %2\n"
            "Jardins Ornementaux: %3"
        ).arg(total).arg(potagers).arg(ornementals);

        countLabel->setText(stats);
    }

    // Trouver le QTextEdit pour la liste
    QTextEdit *jardinListWidget = nullptr;
    for (int i = 0; i < statsLayout->count(); ++i) {
        QTextEdit *textEdit = qobject_cast<QTextEdit*>(statsLayout->itemAt(i)->widget());
        if (textEdit) {
            jardinListWidget = textEdit;
            break;
        }
    }

    if (jardinListWidget) {
        QString listContent;
        for (const JardinStat &j : jardins) {
            listContent += QString(
                "ID: %1 | Emplacement: %2 | Superficie: %3 m² | Type Sol: %4 | Type: %5\n"
            ).arg(j.id).arg(j.emplacement).arg(j.superficie).arg(j.typeSol).arg(j.typeChoix);
        }
        jardinListWidget->setText(listContent);
    }
}
