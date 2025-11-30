#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QMessageBox>
#include <QMap>
#include <QLineEdit>
#include <QVector>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QNetworkAccessManager>
#include <QSortFilterProxyModel>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "Employee.h"
#include "resident.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QMap<QString, int> getStatistiquesStatus();
    QMap<QString, int> getStatistiquesNiveauSecurite();

private slots:
    void onGestionEmployes();
    void onGestionVehicules();
    void onGestionResidents();
    void onGestionMaisons();
    void onGestionJardins();
    void onGestionCabinets();
    void onDeconnexion();

    void onAjouterEmploye();
    void onModifierEmploye();
    void onSupprimerEmploye();
    void onChargerEmployes();
    void onTableSelectionChanged();
    void onExporterPDF();
    void onRechercheChanged();
    void onTriEmailChanged();
    void onTriSalaireChanged();
    void onStatistiqueEmploye();

    void onRechercheVehiculeChanged();
    void onTriTypeChanged();
    void onTriEtatChanged();
    void on_ajouter_3_clicked();
    void on_modifier_3_clicked();
    void on_supprimer_3_clicked();
    void on_exporter_3_clicked();
    void on_btnChatbot_clicked();
    void on_btnTriDate_clicked();
    void on_btnBackFromChat_clicked();
    void on_btnSendChat_clicked();
    void sendMessageToAzureAI(const QString &message);
    void on_btnRecom_clicked();
    void on_btnBackFromRecom_clicked();
    QString buildMaintenancePromptFromCurrentVehicle() const;
    void sendRecommendationToAzureAI(const QString &message);
    QChartView* createVehiculePieChart();
    void on_pushButton_3_clicked();
    void onTriBoxChanged(const QString &mode);

    void onAjouterMaison();
    void onModifierMaison();
    void onSupprimerMaison();
    void onExporterMaison();
    void onTableMaisonSelectionChanged();
    void refreshTableMaison();
    void clearMaisonInputs();
    void loadMaisonToInputs(int id);
    void onExporterMaisons();
    void exporterMaisonsPDF();
    void exporterMaisonsCSV();
    void onTriMaisonsChanged(int index);
    void onRechercheMaisonsChanged(const QString &text);

    void onAjouterResident();
    void onModifierResident();
    void onSupprimerResident();
    void onExporterResident();

    void onAjouterJardin();
    void onModifierJardin();
    void onSupprimerJardin();
    void onExporterJardin();

    void on_Alertes_clicked();
    void refreshAlertes();
    void onAfficherCarte();
    void onRetourAlertes();

    void on_statistique_currentIndexChanged(int index);

private:
    void connectButtons();
    void connectMaisonButtons();
    void connectAlerteButtons();
    void connectJardinUi();
    void appliquerRechercheEtTri();
    void loadVehicules();
    void appliquerRechercheEtTriVehicules();
    void loadMaisons();
    void loadResidents();
    void refreshJardinTable();
    void loadMapForZone(const QString &zone);

    Ui::MainWindow *ui;
    int m_lastInsertedId = -1;
    QVector<Employee> m_allEmployees;
    QLineEdit *m_rechercheLineEdit = nullptr;
    QString selectedImmat;
    int selectedMaisonId = -1;
    int selectedResidentId = -1;
    int selectedJardinId = -1;
    QNetworkAccessManager *networkManager;
    QSortFilterProxyModel *proxyModel;
    bool triCroissant = true;
    QString processChatMessage(const QString &msg);

    struct VehiculeData {
        QString immat;
        QString marque;
        QString modele;
        QString type;
        QString etat;
        QString service;
        QString date_maint;
    };
    QVector<VehiculeData> m_allVehicules;
    QLineEdit *m_rechercheVehiculeLineEdit = nullptr;

    QGraphicsScene *sceneCarte;
    QGraphicsView *viewCarte;
    QNetworkAccessManager *net;
};

#endif // MAINWINDOW_H

