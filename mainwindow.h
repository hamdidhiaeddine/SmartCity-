#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QList>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>

#include "resident.h"
#include "historique.h"
#include "smsmanager.h"
#include "smsreceiver.h"
#include "Employee.h"
#include "vehicule.h"
#include "maison.h"
#include "alerte.h"

class QGraphicsScene;
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
    void on_btnChatbotVehicule_clicked();
    void on_btnBackFromChatVehicule_clicked();
    void on_btnSendChatVehicule_clicked();
    void on_btnRecommandationVehicule_clicked();
    void on_btnBackFromRecommandationVehicule_clicked();
    void on_btnTriDateVehicule_clicked();
    void on_lineEditRechercheVehicule_textChanged(const QString &text);
    QChartView* createVehiculePieChart();
    void on_btnStatistiquesVehicule_clicked();
    
    // Maisons
    void chargerMaisons();
    void onAjouterMaison();
    void onModifierMaison();
    void onSupprimerMaison();
    void onMaisonSelectionChanged();
    void onAssignerResidentMaison();
    
    // Alertes
    void chargerAlertes();
    void onGestionAlertes();
    void on_Alertes_clicked();
    void on_btnRetourAlertes_clicked();
    void onAjouterAlerte();
    void onModifierAlerte();
    void onSupprimerAlerte();
    void onAlerteSelectionChanged();
    void onMarquerAlerteTraitee();
    void onRetourAlertes();
    void onAfficherCarte();


private:
    Ui::MainWindow *ui;
    void connectButtons();
    void setupResidentUi();
    void chargerToutesLesTables();
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
    void reinitialiserFormulaireAlerte();
    void connectAlerteButtons();
    void refreshAlertes();
    
    // Membres
    QList<Resident> m_residentsComplets;
    QString m_utilisateurActuel;
    SmsReceiver *m_smsReceiver;
    QString selectedImmatVehicule;
    QNetworkAccessManager *networkManagerVehicule;
    bool triCroissantVehicule;
    
    // Carte et localisation
    QGraphicsScene *sceneCarte;
    QGraphicsView *viewCarte;
    QNetworkAccessManager *net;
    void loadMapForZone(const QString &zone);
    
    // Fonctions pour chatbot et IA véhicules
    QString getVehiculesDatabaseContext();
    QString processChatMessageVehicule(const QString &msg);
    QString buildMaintenancePromptFromCurrentVehicule() const;
    void sendMessageToAzureAI(const QString &message);
    void sendRecommendationToAzureAI(const QString &message);
};

#endif // MAINWINDOW_H
