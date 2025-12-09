#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMap>
class QGraphicsView;
class QNetworkAccessManager;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // --- Statistiques ---
    QMap<QString, int> getStatistiquesStatus();
    QMap<QString, int> getStatistiquesNiveauSecurite();

private slots:

    void onGestionEmployes();
    void onGestionVehicules();
    void on_Alertes_clicked();
    void onRetourAlertes();

    void onAjouterMaison();
    void onModifierMaison();
    void onSupprimerMaison();
    void onTableMaisonSelectionChanged();
    void refreshTableMaison();
    void clearMaisonInputs();
    void loadMaisonToInputs(int id);

    void onRechercheMaisons();
    void onTriMaisons();
    void onExporterMaisons();
    void exporterMaisonsPDF();
    void exporterMaisonsCSV();

    void refreshAlertes();
    void onAfficherCarte();

    void on_statistique_currentIndexChanged(int index);

private:

    Ui::MainWindow *ui;
    QGraphicsScene *sceneCarte;
    QGraphicsView *viewCarte;
    QNetworkAccessManager *net;
    void loadMapForZone(const QString &zone);

    void connectButtons();
    void connectMaisonButtons();
    void connectAlerteButtons();
};

#endif // MAINWINDOW_H
