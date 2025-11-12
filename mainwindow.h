#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QMessageBox>
#include <QMap>
#include <QLineEdit>
#include "Employee.h"
#include "resident.h"
#include <QtCharts/QChartView>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGestionEmployes();
    void onGestionVehicules();
    void onGestionResidents();
    void onGestionMaisons();
    void onGestionJardins();
    void onGestionCabinets();
    void onDeconnexion();

    // Employés CRUD only
    void onAjouterEmploye();
    void onModifierEmploye();
    void onSupprimerEmploye();
    void onChargerEmployes();
    void onTableSelectionChanged();
    void onExporterPDF();
    void onRechercheChanged();
    void onTriEmailChanged();
    void onTriSalaireChanged();
    // Véhicules recherche et tri
    void onRechercheVehiculeChanged();
    void onTriTypeChanged();
    void onTriEtatChanged();
    // vehicule CRUD only (renommés pour éviter la connexion automatique de Qt)
    void onAjouterVehicule();
    void onModifierVehicule();
    void onSupprimerVehicule();
    void onExporterVehicule();
    QChartView* createVehiculePieChart();
    void on_pushButton_3_clicked();
    // Maison CRUD only (renommés pour éviter la connexion automatique de Qt)
    void onAjouterMaison();
    void onModifierMaison();
    void onSupprimerMaison();
    void onExporterMaison();
    // Resident CRUD only (renommés pour éviter la connexion automatique de Qt)
    void onAjouterResident();
    void onModifierResident();
    void onSupprimerResident();
    void onExporterResident();

private:
    void connectButtons();
    void appliquerRechercheEtTri();
    void loadVehicules();
    void appliquerRechercheEtTriVehicules();
    void loadMaisons();
    void loadResidents();

    Ui::MainWindow *ui;
    int m_lastInsertedId = -1;
    QVector<Employee> m_allEmployees; // Cache pour recherche/tri
    QLineEdit *m_rechercheLineEdit = nullptr; // Widget de recherche
    QString selectedImmat;
    int selectedMaisonId = -1; // Stocke l'ancien ID de la maison sélectionnée
    int selectedResidentId = -1; // Stocke l'ancien ID du résident sélectionné
    
    // Cache pour recherche/tri véhicules
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
};

#endif // MAINWINDOW_H
