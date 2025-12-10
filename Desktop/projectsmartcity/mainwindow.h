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
#include "jardin.h"
#include "cabinet.h"
#include "cabinetdialog.h"
#include "maintenancedialog.h"
#include "recommandationdialog.h"
#include "alerte.h"
#include "facialrecognition.h"
#include "facecapturedialog.h"
#include "arduinorfid.h"
#include "arduinolcd.h"
#include "temperaturesensor.h"
#include "videodialog.h"

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
    void onCapturerVisage();
    
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
    void on_btnRechercherMatriculeLCD_clicked();  // Nouveau: recherche et envoi au LCD
    void on_btnOCRMatricule_clicked();  // Nouveau: OCR pour analyser image de plaque
    void on_btnDemarrerCamera_clicked();  // Nouveau: démarrer flux caméra
    void on_btnCapturerOCR_clicked();  // Nouveau: capturer et analyser
    void on_btnArreterCamera_clicked();  // Nouveau: arrêter flux caméra
    void onCameraDataReceived();  // Nouveau: réception données caméra en continu
    void analyserImageOCR(const QString &imagePath);  // Nouveau: analyse OCR d'une image
    QImage preprocessImageForOCR(const QImage &original);  // Prétraitement image pour OCR
    
    // Maisons
    void chargerMaisons();
    void onAjouterMaison();
    void onModifierMaison();
    void onSupprimerMaison();
    void onMaisonSelectionChanged();
    void onAssignerResidentMaison();
    
    // Jardins
    void chargerJardins();
    void onAjouterJardin();
    void onModifierJardin();
    void onSupprimerJardin();
    void onJardinSelectionChanged();
    void onExporterJardinsPdf();
    void onTrierJardinsParType();
    void onTrierJardinsParId();
    void onTrierJardinsParSuperficie();
    void onRechercherJardin();
    
    // Maintenance et Recommandations Jardins
    void onOuvrirMaintenanceDialog();
    void onOuvrirRecommandationDialog();
    
    // Cabinets
    void chargerCabinets();
    void onAjouterCabinet();
    void onModifierCabinet();
    void onSupprimerCabinet();
    void onExporterCabinetsPdf();
    void onCabinetSelectionChanged();
    void onOuvrirCabinetAvance();
    
    // Vidéos Jardins
    void onOuvrirVideoDialogJardin();
    
    // Arduino RFID
    void onRFIDScanned(const QString &rfidCode);
    
    // Arduino LCD + Keypad
    void onConnecterLCD();
    void onDeconnecterLCD();
    void onToucheKeypadPressee(const QString &touche);
    void onDonneesKeypadEnvoyees(const QString &donnees);
    void rechercherMatriculeKeypad(const QString &matricule);  // Recherche dédiée clavier
    
    // Capteur Température DHT11
    void onTemperatureRecue(float temperature, float humidite);
    void onAlerteArrosage(const QStringList &jardinsAArroser);
    void onConnecterCapteurTemperature();
    void onDeconnecterCapteurTemperature();
    void onLireTemperatureClicked();  // Bouton démarrer/arrêter lecture
    
    // Contrôle Servo-moteur
    void on_btnOuvrirServo_clicked();
    void on_btnFermerServo_clicked();
    void on_btnTestServo_clicked();
    
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
    void reinitialiserFormulaireJardin();
    void reinitialiserFormulaireCabinet();
    void reinitialiserFormulaireAlerte();
    void connectAlerteButtons();
    void refreshAlertes();
    QString construireHtmlJardins() const;
    QString construireHtmlCabinets() const;
    void envoyerVehiculeAuLCD(const QString &immat, const QString &marque, const QString &modele, const QString &type);
    QStringList obtenirJardinsAArroser(float temperatureAmbiante);  // Comparer avec jardins BD
    
    // Membres
    QList<Resident> m_residentsComplets;
    QString m_utilisateurActuel;
    SmsReceiver *m_smsReceiver;
    QString selectedImmatVehicule;
    QNetworkAccessManager *networkManagerVehicule;
    bool triCroissantVehicule;
    QVector<Employee> employesCache;  // Cache des employés pour accéder au password
    ArduinoRFID *arduinoRFID;  // Gestionnaire Arduino RFID
    ArduinoLCD *arduinoLCD;    // Gestionnaire Arduino LCD + Keypad
    TemperatureSensor *m_temperatureSensor;  // Capteur DHT11 température/humidité
    
    // Caméra OV7670
    QSerialPort *cameraSerial;
    QByteArray cameraImageData;
    bool isCapturingCamera;
    bool isCameraStreaming;  // Indique si le flux caméra est actif
    int expectedImageSize;
    QImage lastCapturedImage;  // Dernière image capturée pour OCR
    QLabel *labelFluxPreview;  // Label pour afficher le flux en temps réel
    QDialog *dialogFluxCamera;  // Dialogue pour le flux caméra
    
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
