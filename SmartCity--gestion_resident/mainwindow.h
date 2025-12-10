#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QList>

#include "resident.h"
#include "historique.h"
#include "smsmanager.h"
#include "smsreceiver.h"
#include "Employee.h"
#include "vehicule.h"
#include "maison.h"
#include "jardin.h"
#include "cabinet.h"
#include "jardinstatistics.h"
#include "cabinetdialog.h"
#include "maintenance.h"
#include "recommandationplante.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // ✅ Méthode publique pour rafraîchir après connexion DB
    void rafraichirResidents();
    
    // 📱 Test d'envoi SMS
    void testerEnvoiSms();

private slots:
    void onGestionEmployes();
    void onGestionVehicules();
    void onGestionResidents();
    void onGestionMaisons();
    void onGestionJardins();
    void onGestionCabinets();
    void onDeconnexion();
    void onAjouterResident();
    void onModifierResident();
    void onSupprimerResident();
    void onExporterResidentsPdf();
    void onResidentSelectionChanged();
    void onRechercherResident();
    void onTriResident();
    void onAfficherStatistiques();
    void afficherStatistiquesCercle();
    void onAfficherHistorique();
    void onExporterHistoriquePdf();
    void onFermerHistorique();
    void onViderHistorique();
    void onAjouterReclamation();
    void onEnvoyerSms();
    void onSmsRecu(const QString &from, const QString &to, const QString &message, const QDateTime &dateTime);
    void onAfficherSmsRecus();
    
    // Employés
    void chargerEmployes();
    void onAjouterEmploye();
    void onModifierEmploye();
    void onSupprimerEmploye();
    void onEmployeSelectionChanged();
    
    // Véhicules
    void chargerVehicules();
    void onAjouterVehicule();
    void onModifierVehicule();
    void onSupprimerVehicule();
    void onVehiculeSelectionChanged();
    
    // Maisons
    void chargerMaisons();
    void onAjouterMaison();
    void onModifierMaison();
    void onSupprimerMaison();
    void onMaisonSelectionChanged();

    // Jardins
    void chargerJardins();
    void onAjouterJardin();
    void onModifierJardin();
    void onSupprimerJardin();
    void onExporterJardinsPdf();
    void onJardinSelectionChanged();
    void onAfficherJardinStats();
    void onTrierJardinsParType();
    void onTrierJardinsParId();
    void onTrierJardinsParSuperficie();
    void onRechercherJardin();

    // Cabinets
    void chargerCabinets();
    void onAjouterCabinet();
    void onModifierCabinet();
    void onSupprimerCabinet();
    void onExporterCabinetsPdf();
    void onCabinetSelectionChanged();
    void onOuvrirCabinetAvance();

    // Maintenance prédictive et recommandations
    void chargerMaintenancePredictive();
    void chargerRecommandationsPlantes(const QString &typeSol);
    void onAfficherMaintenancePredictive();
    void onJardinSelectionChangedForMaintenance();
    
    // Dialogues Jardin/Cabinet
    void onOuvrirMaintenanceDialog();
    void onOuvrirRecommandationDialog();
    void onOuvrirVideoDialog();

private:
    Ui::MainWindow *ui;
    void connectButtons();
    void setupResidentUi();
    bool construireResidentDepuisFormulaire(Resident &resident, QStringList &erreurs) const;
    QDate parseDate(const QString &valeur) const;
    void reinitialiserFormulaireResident();
    QString residentSelectionneId() const;
    void afficherErreursValidation(const QStringList &erreurs);
    QString construireHtmlResidents() const;
    void filtrerResidents(const QString &critere, const QString &typeTri);
    void afficherResidents(const QList<Resident> &residents);
    void rafraichirHistorique();
    int calculerAge(const QDate &dateNaissance) const;
    void reinitialiserFormulaireEmploye();
    void reinitialiserFormulaireVehicule();
    void reinitialiserFormulaireMaison();
    void reinitialiserFormulaireJardin();
    void reinitialiserFormulaireCabinet();
    QString construireHtmlJardins() const;
    QString construireHtmlCabinets() const;
    
    // Membres
    QList<Resident> m_residentsComplets;
    QString m_utilisateurActuel;
    SmsReceiver *m_smsReceiver;
};

#endif // MAINWINDOW_H
