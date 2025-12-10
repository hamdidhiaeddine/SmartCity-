#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPixmap>
#include <QMessageBox>
#include <QRegularExpression>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QMarginsF>
#include <QDate>
#include <QDir>
#include <QFileInfo>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTime>
#include <QChart>
#include <QChartView>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QPieSeries>
#include <QPieSlice>
#include <QVBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QMap>
#include <QPainter>
#include <QInputDialog>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QBuffer>
#include <QImage>
#include <QProcess>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cameraSerial(nullptr)
    , isCapturingCamera(false)
    , isCameraStreaming(false)
    , expectedImageSize(320 * 240 * 2)  // QVGA YUV422 = 320x240x2 bytes
    , labelFluxPreview(nullptr)
    , dialogFluxCamera(nullptr)
{
    ui->setupUi(this);
    this->resize(1920, 1080);
    this->move(0, 0);

    QPixmap logo(":/images/Lg.png");
    ui->logo->setPixmap(logo);
    ui->logo->setScaledContents(true);

    setupResidentUi();
    connectButtons();
    connectAlerteButtons();
    m_utilisateurActuel = "Responsable";
    
    // Initialiser la carte et la localisation
    sceneCarte = new QGraphicsScene(this);
    viewCarte = nullptr; // Sera initialisé si un widget frame_2 existe dans l'UI
    if (ui->frame_2) {
        viewCarte = new QGraphicsView(ui->frame_2);
        viewCarte->setGeometry(ui->frame_2->rect());
        viewCarte->setScene(sceneCarte);
        viewCarte->hide();
    }
    net = new QNetworkAccessManager(this);
    
    // Initialiser le network manager pour les véhicules (chatbot + IA)
    networkManagerVehicule = new QNetworkAccessManager(this);
    selectedImmatVehicule = "";
    triCroissantVehicule = true;
    
    // Charger toutes les tables au démarrage
    chargerToutesLesTables();
    
    // Initialiser le récepteur SMS
    m_smsReceiver = new SmsReceiver(this);
    connect(m_smsReceiver, &SmsReceiver::smsRecu, this, &MainWindow::onSmsRecu);
    connect(m_smsReceiver, &SmsReceiver::erreur, this, [](const QString &msg) {
        qWarning() << "Erreur récepteur SMS:" << msg;
    });
    
    // Démarrer le serveur SMS sur le port 8080
    if (m_smsReceiver->demarrerServeur(8080)) {
        qDebug() << "Récepteur SMS démarré avec succès";
    } else {
        qWarning() << "Impossible de démarrer le récepteur SMS";
    }
    
    // Initialiser Arduino RFID
    arduinoRFID = new ArduinoRFID(this);
    connect(arduinoRFID, &ArduinoRFID::rfidScanned, this, &MainWindow::onRFIDScanned);
    connect(arduinoRFID, &ArduinoRFID::errorOccurred, this, [](const QString &error) {
        qWarning() << "Erreur Arduino:" << error;
    });
    
    // Initialiser Arduino LCD + Keypad
    arduinoLCD = new ArduinoLCD(this);
    connect(arduinoLCD, &ArduinoLCD::touchePressee, this, &MainWindow::onToucheKeypadPressee);
    connect(arduinoLCD, &ArduinoLCD::donneesEnvoyees, this, &MainWindow::onDonneesKeypadEnvoyees);
    connect(arduinoLCD, &ArduinoLCD::erreur, this, [](const QString &error) {
        qWarning() << "Erreur Arduino LCD:" << error;
    });
    
    // Connexion LCD IMMÉDIATE (avant RFID) pour réserver COM4
    qDebug() << "🎹 Connexion Arduino LCD en premier...";
    onConnecterLCD();
    
    // Connecter le signal de température depuis Arduino RFID
    connect(arduinoRFID, &ArduinoRFID::temperatureReceived, this, [this](float temperature, float humidite) {
        // Afficher dans l'interface
        onTemperatureRecue(temperature, humidite);
        
        // Vérifier les jardins qui nécessitent un arrosage
        QString error;
        QList<Jardin> jardins = Jardin::fetchAll(&error);
        
        if (error.isEmpty() && !jardins.isEmpty()) {
            QStringList jardinsAArroser;
            QList<int> idsJardinsAArroser;
            
            qDebug() << "🔍 Vérification jardins - Temp ambiante:" << temperature << "°C";
            
            for (const Jardin &j : jardins) {
                // Calculer la différence de température
                float difference = temperature - j.temperatureMoyenneSol();
                
                // Arroser seulement si la différence est >= 3°C
                if (difference >= 3.0) {
                    QString info = QString("Jardin #%1 (%2) - Sol: %3°C → %4°C (Diff: %5°C)")
                                  .arg(j.id())
                                  .arg(j.emplacement())
                                  .arg(j.temperatureMoyenneSol(), 0, 'f', 1)
                                  .arg(temperature, 0, 'f', 1)
                                  .arg(difference, 0, 'f', 1);
                    jardinsAArroser.append(info);
                    idsJardinsAArroser.append(j.id());
                    qDebug() << "  ✅ ARROSAGE NÉCESSAIRE:" << info;
                } else if (difference > 0) {
                    qDebug() << QString("  ⚠️ Jardin #%1: Diff %2°C < 3°C → Pas d'arrosage")
                                .arg(j.id()).arg(difference, 0, 'f', 1);
                } else {
                    qDebug() << QString("  ❌ Jardin #%1: Sol plus chaud → Pas d'arrosage").arg(j.id());
                }
            }
            
            if (!jardinsAArroser.isEmpty()) {
                // Activer la pompe via Arduino RFID
                if (arduinoRFID && arduinoRFID->isConnected()) {
                    arduinoRFID->getSerialPort()->write("1\n");
                    qDebug() << "💧 POMPE ACTIVÉE - Arrosage de" << jardinsAArroser.size() << "jardin(s)";
                    
                    // Arrêter après 3 secondes et mettre à jour les températures
                    QTimer::singleShot(3000, this, [this, idsJardinsAArroser, temperature]() {
                        // Arrêter la pompe
                        if (arduinoRFID && arduinoRFID->isConnected()) {
                            arduinoRFID->getSerialPort()->write("0\n");
                            qDebug() << "🛑 POMPE ARRÊTÉE après 3 secondes";
                        }
                        
                        // Mettre à jour la température de chaque jardin arrosé
                        QString error;
                        for (int idJardin : idsJardinsAArroser) {
                            // Récupérer le jardin actuel
                            QList<Jardin> allJardins = Jardin::fetchAll(&error);
                            for (const Jardin &j : allJardins) {
                                if (j.id() == idJardin) {
                                    // Créer un jardin modifié avec la nouvelle température
                                    Jardin jardinModifie(
                                        j.id(),
                                        j.emplacement(),
                                        j.superficie(),
                                        j.typeSol(),
                                        temperature,  // Nouvelle température = température ambiante
                                        j.typeChoix()
                                    );
                                    
                                    // Modifier dans la BD
                                    if (jardinModifie.modifier(j.id(), &error)) {
                                        qDebug() << QString("✅ Jardin #%1: Température sol mise à jour %2°C → %3°C")
                                                    .arg(idJardin)
                                                    .arg(j.temperatureMoyenneSol(), 0, 'f', 1)
                                                    .arg(temperature, 0, 'f', 1);
                                    } else {
                                        qWarning() << QString("❌ Échec mise à jour température Jardin #%1: %2")
                                                      .arg(idJardin).arg(error);
                                    }
                                    break;
                                }
                            }
                        }
                        
                        // Rafraîchir l'affichage des jardins
                        chargerJardins();
                    });
                }
                
                // Afficher l'alerte
                onAlerteArrosage(jardinsAArroser);
            }
        }
    });
    
    // Vérifier la disponibilité des ports série
    qDebug() << "========================================";
    qDebug() << "🔌 INITIALISATION ARDUINO RFID";
    qDebug() << "========================================";
    QStringList ports = ArduinoRFID::getAvailablePorts();
    if (ports.isEmpty()) {
        qWarning() << "⚠️ Aucun port série détecté sur ce système";
    } else {
        qDebug() << "✓ Ports série disponibles:" << ports.size();
        for (const QString &port : ports) {
            qDebug() << "  -" << port;
        }
        
        // Connexion automatique de l'Arduino RFID au démarrage
        qDebug() << "";
        qDebug() << "🔄 Tentative de connexion automatique Arduino RFID...";
        qDebug() << "   ⚠️ COM4 et COM10 réservés pour Arduino LCD (Keypad)";
        
        bool arduinoConnecte = false;
        
        // Prioriser les ports Arduino (sauf COM4 et COM10 réservés pour LCD)
        for (const QString &port : ports) {
            if (port == "COM4" || port == "COM10") {
                qDebug() << "   ⏭️" << port << "ignoré (réservé pour LCD)";
                continue;
            }
            if (port.contains("Arduino", Qt::CaseInsensitive)) {
                qDebug() << "   Essai prioritaire sur" << port << "...";
                if (arduinoRFID->connectArduino(port)) {
                    qDebug() << "✅ Arduino RFID connecté automatiquement sur" << port;
                    arduinoConnecte = true;
                    break;
                }
            }
        }
        
        // Si pas d'Arduino trouvé, essayer les autres ports (sauf COM4 et COM10)
        if (!arduinoConnecte) {
            for (const QString &port : ports) {
                if (port == "COM4" || port == "COM10") continue;  // Réservé pour LCD
                if (!port.contains("Bluetooth", Qt::CaseInsensitive)) {
                    qDebug() << "   Essai sur" << port << "...";
                    if (arduinoRFID->connectArduino(port)) {
                        qDebug() << "✅ Arduino RFID connecté automatiquement sur" << port;
                        arduinoConnecte = true;
                        break;
                    }
                }
            }
        }
        
        if (!arduinoConnecte) {
            qWarning() << "⚠️ Arduino RFID non connecté automatiquement";
            qWarning() << "💡 Vous pouvez réessayer manuellement depuis 'Gestion Maisons'";
        }
    }
    qDebug() << "========================================";
    
    // ⚠️ IMPORTANT: Le capteur DHT11 et l'Arduino RFID ne peuvent pas partager le même port série
    // Option 1: Utiliser 2 Arduino différents sur 2 ports différents
    // Option 2: Désactiver l'un des deux systèmes
    // Pour l'instant, on désactive le capteur température si RFID est connecté
    
    // Initialiser le capteur de température DHT11 + Pompe
    m_temperatureSensor = new TemperatureSensor(this);
    connect(m_temperatureSensor, &TemperatureSensor::temperatureRecue, this, &MainWindow::onTemperatureRecue);
    connect(m_temperatureSensor, &TemperatureSensor::alerteArrosage, this, &MainWindow::onAlerteArrosage);
    connect(m_temperatureSensor, &TemperatureSensor::erreurConnexion, this, [](const QString &error) {
        qWarning() << "Erreur capteur température:" << error;
    });
    connect(m_temperatureSensor, &TemperatureSensor::pompeStateChanged, this, [this](bool active) {
        qDebug() << (active ? "🚿 Pompe: MARCHE" : "🛑 Pompe: ARRÊT");
    });
    connect(m_temperatureSensor, &TemperatureSensor::lectureStateChanged, this, [this](bool reading) {
        if (ui->btnLireTemperature) {
            if (reading) {
                ui->btnLireTemperature->setText("⏹️ Arrêter Lecture");
                ui->btnLireTemperature->setStyleSheet(
                    "QPushButton { background-color: #dc3545; color: white; border-radius: 5px; font: bold 10pt \"Arial\"; }"
                    "QPushButton:hover { background-color: #c82333; }"
                );
            } else {
                ui->btnLireTemperature->setText("▶️ Lire Température");
                ui->btnLireTemperature->setStyleSheet(
                    "QPushButton { background-color: #28a745; color: white; border-radius: 5px; font: bold 10pt \"Arial\"; }"
                    "QPushButton:hover { background-color: #218838; }"
                );
            }
        }
    });
    
    qDebug() << "";
    qDebug() << "========================================";
    qDebug() << "🌡️ INITIALISATION CAPTEUR DHT11 + POMPE";
    qDebug() << "========================================";
    qDebug() << "ℹ️  Utilisez un Arduino séparé sur un autre port (pas COM9)";
    qDebug() << "ℹ️  Cliquez sur '🔌 Connecter Capteur' pour choisir le port";
    qDebug() << "========================================";
    
    // DÉSACTIVER la connexion automatique pour éviter les conflits avec RFID
    // Vous devez connecter manuellement via le bouton
    /*
    // Connexion automatique désactivée - décommentez si vous avez 2 Arduino
    bool capteurConnecte = false;
    
    // Prioriser un port différent du RFID (ex: COM8, COM10, etc.)
    // NE PAS UTILISER COM9 si RFID est déjà dessus
    QString portPrioritaire = "COM8";  // Changez selon votre configuration
    if (ports.contains(portPrioritaire)) {
        qDebug() << "🔄 Tentative de connexion prioritaire sur" << portPrioritaire << "...";
        if (m_temperatureSensor->connecter(portPrioritaire)) {
            qDebug() << "✅ Capteur DHT11 connecté automatiquement sur" << portPrioritaire;
            capteurConnecte = true;
            
            // Mettre à jour le bouton pour indiquer que le capteur est connecté
            if (ui->btnConnecterCapteur) {
                ui->btnConnecterCapteur->setText("✅ Capteur Connecté");
                ui->btnConnecterCapteur->setStyleSheet(
                    "QPushButton { background-color: #28a745; color: white; border-radius: 5px; font: bold 10pt \"Arial\"; }"
                    "QPushButton:hover { background-color: #218838; }"
                );
            }
            
            // Activer le bouton de lecture
            if (ui->btnLireTemperature) {
                ui->btnLireTemperature->setEnabled(true);
            }
        }
    }
    
    // Si COM8 n'a pas marché, essayer les autres ports (sauf Bluetooth et RFID)
    if (!capteurConnecte) {
        for (const QString &port : ports) {
            if (port == portPrioritaire) continue; // Déjà testé
            if (port.contains("Bluetooth", Qt::CaseInsensitive)) continue; // Éviter Bluetooth
            if (port.contains("COM9")) continue; // Éviter le port RFID (déjà utilisé)
            
            qDebug() << "🔄 Test de connexion sur" << port << "...";
            if (m_temperatureSensor->connecter(port)) {
                qDebug() << "✅ Capteur DHT11 connecté automatiquement sur" << port;
                capteurConnecte = true;
                
                // Mettre à jour le bouton pour indiquer que le capteur est connecté
                if (ui->btnConnecterCapteur) {
                    ui->btnConnecterCapteur->setText("✅ Capteur Connecté");
                    ui->btnConnecterCapteur->setStyleSheet(
                        "QPushButton { background-color: #28a745; color: white; border-radius: 5px; font: bold 10pt \"Arial\"; }"
                        "QPushButton:hover { background-color: #218838; }"
                    );
                }
                
                // Activer le bouton de lecture
                if (ui->btnLireTemperature) {
                    ui->btnLireTemperature->setEnabled(true);
                }
                break;
            }
        }
    }
    
    qDebug() << "ℹ️  Utilisez un Arduino séparé sur un autre port (pas COM9)";
    qDebug() << "ℹ️  Cliquez sur '🔌 Connecter Capteur' pour choisir le port";
    qDebug() << "========================================";
    
    // DÉSACTIVER la connexion automatique pour éviter les conflits avec RFID
    // Vous devez connecter manuellement via le bouton
    /*
    // Connexion automatique désactivée - décommentez si vous avez 2 Arduino
    bool capteurConnecte = false;
    
    // Prioriser un port différent du RFID (ex: COM8, COM10, etc.)
    // NE PAS UTILISER COM9 si RFID est déjà dessus
    QString portPrioritaire = "COM8";  // Changez selon votre configuration
    if (ports.contains(portPrioritaire)) {
        qDebug() << "🔄 Tentative de connexion prioritaire sur" << portPrioritaire << "...";
        if (m_temperatureSensor->connecter(portPrioritaire)) {
            qDebug() << "✅ Capteur DHT11 connecté automatiquement sur" << portPrioritaire;
            capteurConnecte = true;
            
            // Mettre à jour le bouton pour indiquer que le capteur est connecté
            if (ui->btnConnecterCapteur) {
                ui->btnConnecterCapteur->setText("✅ Capteur Connecté");
                ui->btnConnecterCapteur->setStyleSheet(
                    "QPushButton { background-color: #28a745; color: white; border-radius: 5px; font: bold 10pt \"Arial\"; }"
                    "QPushButton:hover { background-color: #218838; }"
                );
            }
            
            // Activer le bouton de lecture
            if (ui->btnLireTemperature) {
                ui->btnLireTemperature->setEnabled(true);
            }
        }
    }
    
    // Si COM8 n'a pas marché, essayer les autres ports (sauf Bluetooth et RFID)
    if (!capteurConnecte) {
        for (const QString &port : ports) {
            if (port == portPrioritaire) continue; // Déjà testé
            if (port.contains("Bluetooth", Qt::CaseInsensitive)) continue; // Éviter Bluetooth
            if (port.contains("COM9")) continue; // Éviter le port RFID (déjà utilisé)
            
            qDebug() << "🔄 Test de connexion sur" << port << "...";
            if (m_temperatureSensor->connecter(port)) {
                qDebug() << "✅ Capteur DHT11 connecté automatiquement sur" << port;
                capteurConnecte = true;
                
                // Mettre à jour le bouton pour indiquer que le capteur est connecté
                if (ui->btnConnecterCapteur) {
                    ui->btnConnecterCapteur->setText("✅ Capteur Connecté");
                    ui->btnConnecterCapteur->setStyleSheet(
                        "QPushButton { background-color: #28a745; color: white; border-radius: 5px; font: bold 10pt \"Arial\"; }"
                        "QPushButton:hover { background-color: #218838; }"
                    );
                }
                
                // Activer le bouton de lecture
                if (ui->btnLireTemperature) {
                    ui->btnLireTemperature->setEnabled(true);
                }
                break;
            }
        }
    }
    
    if (!capteurConnecte) {
        qWarning() << "⚠️ Capteur DHT11 non connecté - vérifiez le câblage et le port";
        qWarning() << "⚠️ Vous pouvez cliquer sur le bouton '🔌 Connecter Capteur' pour le connecter manuellement";
    } else {
        qDebug() << "ℹ️  Cliquez sur '▶️ Lire Température' pour démarrer la lecture";
    }
    qDebug() << "========================================";
    */
}

MainWindow::~MainWindow()
{
    delete sceneCarte;
    delete viewCarte;
    delete ui;
}
void MainWindow::connectButtons()
{
    connect(ui->btnEmployes,    &QPushButton::clicked, this, &MainWindow::onGestionEmployes);
    connect(ui->btnVehicules,   &QPushButton::clicked, this, &MainWindow::onGestionVehicules);
    connect(ui->btnResidents,   &QPushButton::clicked, this, &MainWindow::onGestionResidents);
    connect(ui->btnMaisons,     &QPushButton::clicked, this, &MainWindow::onGestionMaisons);
    connect(ui->btnJardins,     &QPushButton::clicked, this, &MainWindow::onGestionJardins);
    connect(ui->btnCabinets,    &QPushButton::clicked, this, &MainWindow::onGestionCabinets);
    connect(ui->btnDeconnexion, &QPushButton::clicked, this, &MainWindow::onDeconnexion);
    connect(ui->ajouter_5,      &QPushButton::clicked, this, &MainWindow::onAjouterResident);
    connect(ui->modifier_5,     &QPushButton::clicked, this, &MainWindow::onModifierResident);
    connect(ui->supprimer_5,    &QPushButton::clicked, this, &MainWindow::onSupprimerResident);
    connect(ui->exporter_5,     &QPushButton::clicked, this, &MainWindow::onExporterResidentsPdf);
    connect(ui->tableau_5,      &QTableWidget::itemSelectionChanged, this, &MainWindow::onResidentSelectionChanged);
    
    // Recherche et tri
    if (ui->recherche_4) {
        connect(ui->recherche_4, &QLineEdit::textChanged, this, &MainWindow::onRechercherResident);
    }
    if (ui->btnRechercher) {
        connect(ui->btnRechercher, &QPushButton::clicked, this, &MainWindow::onRechercherResident);
    }
    if (ui->triemail_4) {
        connect(ui->triemail_4, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTriResident);
    }
    
    // Statistiques
    if (ui->btnStatistiques) {
        connect(ui->btnStatistiques, &QPushButton::clicked, this, &MainWindow::onAfficherStatistiques);
    }
    
    // Historique
    if (ui->btnHistorique) {
        connect(ui->btnHistorique, &QPushButton::clicked, this, &MainWindow::onAfficherHistorique);
    }
    if (ui->btnFermerHistorique) {
        connect(ui->btnFermerHistorique, &QPushButton::clicked, this, &MainWindow::onFermerHistorique);
    }
    if (ui->btnPdfHistorique) {
        connect(ui->btnPdfHistorique, &QPushButton::clicked, this, &MainWindow::onExporterHistoriquePdf);
    }
    if (ui->btnViderHistorique) {
        connect(ui->btnViderHistorique, &QPushButton::clicked, this, &MainWindow::onViderHistorique);
    }
    if (ui->comboBoxAction) {
        connect(ui->comboBoxAction, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::rafraichirHistorique);
    }
    
    // SMS
    if (ui->btnEnvoyerSms) {
        connect(ui->btnEnvoyerSms, &QPushButton::clicked, this, &MainWindow::onEnvoyerSms);
    }
    
    // Employés
    if (ui->ajouter) {
        connect(ui->ajouter, &QPushButton::clicked, this, &MainWindow::onAjouterEmploye);
    }
    if (ui->modifier) {
        connect(ui->modifier, &QPushButton::clicked, this, &MainWindow::onModifierEmploye);
    }
    if (ui->supprimer) {
        connect(ui->supprimer, &QPushButton::clicked, this, &MainWindow::onSupprimerEmploye);
    }
    if (ui->tableau) {
        connect(ui->tableau, &QTableWidget::itemSelectionChanged, this, &MainWindow::onEmployeSelectionChanged);
    }
    if (ui->capturerVisageBtn) {
        connect(ui->capturerVisageBtn, &QPushButton::clicked, this, &MainWindow::onCapturerVisage);
    }
    
    // Véhicules
    if (ui->ajouter_3) {
        connect(ui->ajouter_3, &QPushButton::clicked, this, &MainWindow::onAjouterVehicule);
    }
    if (ui->modifier_3) {
        connect(ui->modifier_3, &QPushButton::clicked, this, &MainWindow::onModifierVehicule);
    }
    if (ui->supprimer_3) {
        connect(ui->supprimer_3, &QPushButton::clicked, this, &MainWindow::onSupprimerVehicule);
    }
    if (ui->tableau_3) {
        connect(ui->tableau_3, &QTableWidget::itemSelectionChanged, this, &MainWindow::onVehiculeSelectionChanged);
    }
    
    // Maisons
    if (ui->ajouter_7) {
        connect(ui->ajouter_7, &QPushButton::clicked, this, &MainWindow::onAjouterMaison);
    }
    if (ui->modifier_7) {
        connect(ui->modifier_7, &QPushButton::clicked, this, &MainWindow::onModifierMaison);
    }
    if (ui->supprimer_6) {
        connect(ui->supprimer_6, &QPushButton::clicked, this, &MainWindow::onSupprimerMaison);
    }
    if (ui->tableau_7) {
        connect(ui->tableau_7, &QTableWidget::itemSelectionChanged, this, &MainWindow::onMaisonSelectionChanged);
    }
    if (ui->btnAssignerResident) {
        connect(ui->btnAssignerResident, &QPushButton::clicked, this, &MainWindow::onAssignerResidentMaison);
    }
    
    // Jardins
    if (ui->ajouter_8) {
        connect(ui->ajouter_8, &QPushButton::clicked, this, &MainWindow::onAjouterJardin);
    }
    if (ui->modifier_8) {
        connect(ui->modifier_8, &QPushButton::clicked, this, &MainWindow::onModifierJardin);
    }
    if (ui->supprimer_7) {
        connect(ui->supprimer_7, &QPushButton::clicked, this, &MainWindow::onSupprimerJardin);
    }
    if (ui->exporter_8) {
        connect(ui->exporter_8, &QPushButton::clicked, this, &MainWindow::onExporterJardinsPdf);
    }
    if (ui->tableau_8) {
        connect(ui->tableau_8, &QTableWidget::itemSelectionChanged, this, &MainWindow::onJardinSelectionChanged);
    }
    if (ui->tripartype) {
        connect(ui->tripartype, &QPushButton::clicked, this, &MainWindow::onTrierJardinsParType);
    }
    if (ui->triparid) {
        connect(ui->triparid, &QPushButton::clicked, this, &MainWindow::onTrierJardinsParId);
    }
    if (ui->triparsuperficie) {
        connect(ui->triparsuperficie, &QPushButton::clicked, this, &MainWindow::onTrierJardinsParSuperficie);
    }
    if (ui->recherchebtn) {
        connect(ui->recherchebtn, &QPushButton::clicked, this, &MainWindow::onRechercherJardin);
    }
    
    // Maintenance et Recommandations Jardins
    if (ui->maintenance) {
        connect(ui->maintenance, &QPushButton::clicked, this, &MainWindow::onOuvrirMaintenanceDialog);
    }
    if (ui->Recommendation) {
        connect(ui->Recommendation, &QPushButton::clicked, this, &MainWindow::onOuvrirRecommandationDialog);
    }
    
    // Bouton Vidéo Jardin (créé programmatiquement car absent du .ui)
    QPushButton *btnVideoJardin = new QPushButton("📹 Vidéos Jardins", this);
    btnVideoJardin->setObjectName("btnVideoJardin");
    btnVideoJardin->setMinimumSize(150, 40);
    btnVideoJardin->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    padding: 10px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 11pt;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
    );
    connect(btnVideoJardin, &QPushButton::clicked, this, &MainWindow::onOuvrirVideoDialogJardin);
    
    // Ajouter le bouton en dessous du bouton Maintenance
    if (ui->maintenance && ui->maintenance->parentWidget()) {
        QWidget *parent = ui->maintenance->parentWidget();
        
        // Définir la position en dessous du bouton Maintenance
        QPoint pos = ui->maintenance->pos();
        btnVideoJardin->move(pos.x(), pos.y() + ui->maintenance->height() + 10);
        btnVideoJardin->setParent(parent);
        btnVideoJardin->show();
    }
    
    // Bouton connecter capteur de température
    if (ui->btnConnecterCapteur) {
        connect(ui->btnConnecterCapteur, &QPushButton::clicked, this, &MainWindow::onConnecterCapteurTemperature);
    }
    
    // Bouton démarrer/arrêter lecture température
    if (ui->btnLireTemperature) {
        connect(ui->btnLireTemperature, &QPushButton::clicked, this, &MainWindow::onLireTemperatureClicked);
    }
    
    // Cabinets
    if (ui->ajouter_9) {
        connect(ui->ajouter_9, &QPushButton::clicked, this, &MainWindow::onAjouterCabinet);
    }
    if (ui->modifier_9) {
        connect(ui->modifier_9, &QPushButton::clicked, this, &MainWindow::onModifierCabinet);
    }
    if (ui->supprimer_8) {
        connect(ui->supprimer_8, &QPushButton::clicked, this, &MainWindow::onSupprimerCabinet);
    }
    if (ui->exporter_9) {
        connect(ui->exporter_9, &QPushButton::clicked, this, &MainWindow::onExporterCabinetsPdf);
    }
    if (ui->tableau_12) {
        connect(ui->tableau_12, &QTableWidget::itemSelectionChanged, this, &MainWindow::onCabinetSelectionChanged);
    }
}
void MainWindow::onGestionEmployes()
{
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
    chargerEmployes(); // Charger les employés lors de l'affichage de la page
}

void MainWindow::onGestionVehicules()
{
    ui->stackedWidget->setCurrentWidget(ui->pageVehicules);
    chargerVehicules(); // Charger les véhicules lors de l'affichage de la page
}

void MainWindow::onGestionResidents()
{
    ui->stackedWidget->setCurrentWidget(ui->pageResidents);
}

void MainWindow::onGestionMaisons()
{
    ui->stackedWidget->setCurrentWidget(ui->pageMaisons);
    chargerMaisons(); // Charger les maisons lors de l'affichage de la page
}

void MainWindow::onGestionJardins()
{
    ui->stackedWidget->setCurrentWidget(ui->pageJardins);
    chargerJardins();
}

void MainWindow::onGestionCabinets()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCabinets);
    chargerCabinets();
}

void MainWindow::onDeconnexion()
{
    close();
}

void MainWindow::setupResidentUi()
{
    if (!ui->tableau_5) {
        return;
    }

    ui->tableau_5->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableau_5->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableau_5->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableau_5->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::chargerToutesLesTables()
{
    qDebug() << "=== 📊 Chargement de toutes les tables au démarrage ===";
    
    // Charger les résidents
    rafraichirResidents();
    
    // Charger les employés
    chargerEmployes();
    
    // Charger les véhicules
    chargerVehicules();
    
    // Charger les maisons
    chargerMaisons();
    
    // Charger les jardins
    chargerJardins();
    
    // Charger les cabinets
    chargerCabinets();
    
    // Charger les alertes
    chargerAlertes();
    
    qDebug() << "=== ✅ Toutes les tables ont été chargées ===";
}

void MainWindow::rafraichirResidents()
{
    if (!ui->tableau_5) {
        qWarning() << "✗ Tableau non initialisé !";
        return;
    }

    qDebug() << "🔄 Rafraîchissement des résidents...";
    m_residentsComplets = Resident::recupererTout();
    qDebug() << "📊 Nombre de résidents récupérés :" << m_residentsComplets.size();
    
    if (m_residentsComplets.isEmpty()) {
        qWarning() << "⚠️ Aucun résident trouvé dans la base de données !";
        qWarning() << "   Vérifiez que :";
        qWarning() << "   1. La connexion à la base de données est active";
        qWarning() << "   2. Les tables sont créées pour l'utilisateur 'soumaya'";
        qWarning() << "   3. Des résidents ont été insérés";
    }
    
    afficherResidents(m_residentsComplets);
    qDebug() << "✓ Affichage terminé - Lignes dans le tableau :" << ui->tableau_5->rowCount();
}

void MainWindow::afficherResidents(const QList<Resident> &residents)
{
    if (!ui->tableau_5) {
        qWarning() << "✗ Tableau non disponible !";
        return;
    }

    qDebug() << "📋 Affichage de" << residents.size() << "résidents dans le tableau...";
    
    ui->tableau_5->clearContents();
    ui->tableau_5->setRowCount(residents.size());

    int row = 0;
    for (const Resident &resident : residents) {
        const QString dateTexte = resident.dateNaissance().isValid()
                                      ? resident.dateNaissance().toString("dd/MM/yyyy")
                                      : QString();

        ui->tableau_5->setItem(row, 0, new QTableWidgetItem(resident.id()));
        QTableWidgetItem *nomPrenomItem = new QTableWidgetItem(resident.nomComplet());
        nomPrenomItem->setData(Qt::UserRole, resident.nom());
        nomPrenomItem->setData(Qt::UserRole + 1, resident.prenom());
        ui->tableau_5->setItem(row, 1, nomPrenomItem);
        ui->tableau_5->setItem(row, 2, new QTableWidgetItem(dateTexte));
        ui->tableau_5->setItem(row, 3, new QTableWidgetItem(resident.adresse()));
        ui->tableau_5->setItem(row, 4, new QTableWidgetItem(resident.telephone()));
        ui->tableau_5->setItem(row, 5, new QTableWidgetItem(resident.email()));
        ui->tableau_5->setItem(row, 6, new QTableWidgetItem(resident.statut()));
        ui->tableau_5->setItem(row, 7, new QTableWidgetItem(resident.situation()));
        // Afficher l'âge au lieu de la réclamation
        int age = calculerAge(resident.dateNaissance());
        ui->tableau_5->setItem(row, 8, new QTableWidgetItem(QString::number(age)));
        
        qDebug() << "  - Ligne" << row << ":" << resident.nomComplet() << "(ID:" << resident.id() << ")";
        ++row;
    }
    
    qDebug() << "✓" << row << "résidents affichés";
}

bool MainWindow::construireResidentDepuisFormulaire(Resident &resident, QStringList &erreurs) const
{
    const QString id = ui->idresidentline_2->text().trimmed();
    const QString nomComplet = ui->nomline_7->text().simplified();
    const QString dateTexte = ui->datenaissanceline_2->text().trimmed();
    const QString adresse = ui->adresseline_4->text().trimmed();
    QString telephone = ui->telephoneline_5->text().simplified();
    telephone.remove(' ');
    const QString email = ui->emailline_4->text().trimmed();
    const QString statut = ui->statusline_2 ? ui->statusline_2->currentText() : QString();
    const QString situation = ui->situationline_2 ? ui->situationline_2->currentText() : QString();

    QRegularExpression idRegex(QStringLiteral("^[0-9]{1,10}$"));
    if (id.isEmpty() || !idRegex.match(id).hasMatch()) {
        erreurs << tr("Identifiant invalide (uniquement des chiffres, 10 maximum).");
    }

    QString nom;
    QString prenom;
    const QStringList morceaux = nomComplet.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    if (morceaux.size() < 2) {
        erreurs << tr("Veuillez saisir le nom puis le prénom (ex: Dupont Sami).");
    } else {
        nom = morceaux.first();
        prenom = morceaux.mid(1).join(" ");
        if (nom.size() < 2) {
            erreurs << tr("Le nom doit contenir au moins 2 caractères.");
        }
        if (prenom.size() < 2) {
            erreurs << tr("Le prénom doit contenir au moins 2 caractères.");
        }
    }

    QDate dateNaissance = parseDate(dateTexte);
    if (!dateNaissance.isValid()) {
        erreurs << tr("Date de naissance invalide (formats acceptés: jj/mm/aaaa ou aaaa-mm-jj).");
    }

    if (adresse.isEmpty()) {
        erreurs << tr("L'adresse est obligatoire.");
    }

    QRegularExpression telRegex(QStringLiteral("^\\+?\\d{6,15}$"));
    if (telephone.isEmpty() || !telRegex.match(telephone).hasMatch()) {
        erreurs << tr("Numéro de téléphone invalide (6 à 15 chiffres, option + au début).");
    }

    QRegularExpression emailRegex(
        QStringLiteral("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"));
    if (email.isEmpty() || !emailRegex.match(email).hasMatch()) {
        erreurs << tr("Adresse e-mail invalide.");
    }

    if (statut.isEmpty()) {
        erreurs << tr("Le statut est obligatoire.");
    }

    if (situation.isEmpty()) {
        erreurs << tr("La situation familiale est obligatoire.");
    }

    if (!erreurs.isEmpty()) {
        return false;
    }

    resident = Resident(id, nom, prenom, dateNaissance, adresse, telephone, email, statut, situation);
    return true;
}

QDate MainWindow::parseDate(const QString &valeur) const
{
    const QStringList formats = {QStringLiteral("dd/MM/yyyy"),
                                 QStringLiteral("dd-MM-yyyy"),
                                 QStringLiteral("yyyy-MM-dd"),
                                 QStringLiteral("yyyy/MM/dd")};
    for (const QString &format : formats) {
        const QDate date = QDate::fromString(valeur, format);
        if (date.isValid()) {
            return date;
        }
    }
    return QDate();
}

void MainWindow::reinitialiserFormulaireResident()
{
    ui->idresidentline_2->clear();
    ui->nomline_7->clear();
    ui->datenaissanceline_2->clear();
    ui->adresseline_4->clear();
    ui->telephoneline_5->clear();
    ui->emailline_4->clear();
    ui->statusline_2->setCurrentIndex(0);
    ui->situationline_2->setCurrentIndex(0);
}

QString MainWindow::residentSelectionneId() const
{
    const int row = ui->tableau_5->currentRow();
    if (row >= 0) {
        QTableWidgetItem *item = ui->tableau_5->item(row, 0);
        if (item) {
            return item->text().trimmed();
        }
    }
    return ui->idresidentline_2->text().trimmed();
}

void MainWindow::afficherErreursValidation(const QStringList &erreurs)
{
    QMessageBox::warning(this, tr("Contrôle de saisie"), erreurs.join("\n"));
}

QString MainWindow::construireHtmlResidents() const
{
    QString html = QStringLiteral("<h2>Liste des résidents</h2>");
    html += QStringLiteral("<table border='1' cellspacing='0' cellpadding='4'>"
                           "<tr>"
                           "<th>ID</th>"
                           "<th>Nom</th>"
                           "<th>Prénom</th>"
                           "<th>Date de naissance</th>"
                           "<th>Adresse</th>"
                           "<th>Téléphone</th>"
                           "<th>Email</th>"
                           "<th>Statut</th>"
                           "<th>Situation familiale</th>"
                           "</tr>");

    for (int row = 0; row < ui->tableau_5->rowCount(); ++row) {
        html += QStringLiteral("<tr>");
        QTableWidgetItem *idItem = ui->tableau_5->item(row, 0);
        QTableWidgetItem *nomItem = ui->tableau_5->item(row, 1);
        const QString id = idItem ? idItem->text() : QString();
        const QString nom = nomItem ? nomItem->data(Qt::UserRole).toString() : QString();
        const QString prenom = nomItem ? nomItem->data(Qt::UserRole + 1).toString() : QString();
        QString nomTemp = nom;
        QString prenomTemp = prenom;
        if ((nomTemp.isEmpty() || prenomTemp.isEmpty()) && nomItem) {
            const QStringList morceaux = nomItem->text().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (!morceaux.isEmpty()) {
                if (nomTemp.isEmpty()) {
                    nomTemp = morceaux.first();
                }
                if (prenomTemp.isEmpty()) {
                    prenomTemp = morceaux.mid(1).join(" ");
                }
            }
        }

        const auto valeur = [this](int r, int c) -> QString {
            QTableWidgetItem *item = ui->tableau_5->item(r, c);
            return item ? item->text() : QString();
        };

        html += QStringLiteral("<td>%1</td>").arg(id.toHtmlEscaped());
        html += QStringLiteral("<td>%1</td>").arg(nomTemp.toHtmlEscaped());
        html += QStringLiteral("<td>%1</td>").arg(prenomTemp.toHtmlEscaped());
        html += QStringLiteral("<td>%1</td>").arg(valeur(row, 2).toHtmlEscaped());
        html += QStringLiteral("<td>%1</td>").arg(valeur(row, 3).toHtmlEscaped());
        html += QStringLiteral("<td>%1</td>").arg(valeur(row, 4).toHtmlEscaped());
        html += QStringLiteral("<td>%1</td>").arg(valeur(row, 5).toHtmlEscaped());
        html += QStringLiteral("<td>%1</td>").arg(valeur(row, 6).toHtmlEscaped());
        html += QStringLiteral("<td>%1</td>").arg(valeur(row, 7).toHtmlEscaped());
        html += QStringLiteral("</tr>");
    }
    html += QStringLiteral("</table>");
    return html;
}

void MainWindow::onAjouterResident()
{
    QStringList erreurs;
    Resident resident;
    if (!construireResidentDepuisFormulaire(resident, erreurs)) {
        afficherErreursValidation(erreurs);
        return;
    }

    if (Resident::existe(resident.id())) {
        QMessageBox::warning(this, tr("Ajout résident"),
                             tr("Un résident possède déjà cet identifiant."));
        return;
    }

    if (!Resident::ajouter(resident)) {
        QMessageBox::critical(this, tr("Ajout résident"),
                              tr("Impossible d'ajouter le résident.\n\n"
                                 "Vérifiez:\n"
                                 "• Que l'ID n'existe pas déjà\n"
                                 "• Que tous les champs obligatoires sont remplis\n"
                                 "• Que la connexion à la base de données est active\n\n"
                                 "Consultez les logs pour plus de détails."));
        return;
    }

    // L'historique est enregistré automatiquement par le trigger TRG_AUTO_HISTORIQUE_RESIDENT

    // Envoyer un SMS de notification AU RÉSIDENT
    if (!resident.telephone().isEmpty()) {
        QString messageSms = QString("Bonjour %1,\n\nVotre inscription en tant que résident a été enregistrée avec succès.\n\nCordialement,\nÉquipe SMARTCITY")
                             .arg(resident.nomComplet());
        if (SmsManager::envoyerSms(resident.telephone(), messageSms)) {
            qDebug() << "SMS de confirmation envoyé au résident" << resident.telephone();
        } else {
            qWarning() << "Échec de l'envoi du SMS au résident";
        }
    }
    
    // Envoyer un SMS de notification À L'ADMINISTRATEUR
    QString numeroAdmin = "+21628500392"; // Votre numéro
    QString messageSmsAdmin = QString("🆕 NOUVEAU RÉSIDENT AJOUTÉ\n\n"
                                      "Nom: %1\n"
                                      "ID: %2\n"
                                      "Tél: %3\n"
                                      "Email: %4\n\n"
                                      "SmartCity")
                              .arg(resident.nomComplet())
                              .arg(resident.id())
                              .arg(resident.telephone().isEmpty() ? "N/A" : resident.telephone())
                              .arg(resident.email().isEmpty() ? "N/A" : resident.email());
    
    if (SmsManager::envoyerSms(numeroAdmin, messageSmsAdmin)) {
        qDebug() << "✓ SMS de notification admin envoyé à" << numeroAdmin;
    } else {
        qWarning() << "✗ Échec de l'envoi du SMS admin";
    }

    rafraichirResidents();
    reinitialiserFormulaireResident();
    QMessageBox::information(this, tr("Ajout résident"), tr("Résident ajouté avec succès."));
}

void MainWindow::onModifierResident()
{
    QStringList erreurs;
    Resident resident;
    if (!construireResidentDepuisFormulaire(resident, erreurs)) {
        afficherErreursValidation(erreurs);
        return;
    }

    if (!Resident::existe(resident.id())) {
        QMessageBox::warning(this, tr("Modification résident"),
                             tr("Cet identifiant n'existe pas."));
        return;
    }

    if (!Resident::modifier(resident)) {
        QMessageBox::critical(this, tr("Modification résident"),
                              tr("La mise à jour a échoué.\n\n"
                                 "Vérifiez:\n"
                                 "• Que l'ID existe dans la base de données\n"
                                 "• Que tous les champs obligatoires sont remplis\n"
                                 "• Que la connexion à la base de données est active\n\n"
                                 "Consultez les logs pour plus de détails."));
        return;
    }

    // L'historique est enregistré automatiquement par le trigger TRG_AUTO_HIST_RES_UPDATE

    // Envoyer un SMS de notification
    if (!resident.telephone().isEmpty()) {
        QString messageSms = QString("Bonjour %1,\n\nVos informations de résident ont été mises à jour.\n\nCordialement,\nÉquipe SMARTCITY")
                             .arg(resident.nomComplet());
        if (SmsManager::envoyerSms(resident.telephone(), messageSms)) {
            qDebug() << "SMS de notification envoyé avec succès à" << resident.telephone();
        } else {
            qWarning() << "Échec de l'envoi du SMS de notification";
        }
    }

    rafraichirResidents();
    QMessageBox::information(this, tr("Modification résident"),
                             tr("Les informations ont été mises à jour."));
}

void MainWindow::onSupprimerResident()
{
    const QString id = residentSelectionneId();
    if (id.isEmpty()) {
        QMessageBox::warning(this, tr("Suppression résident"),
                             tr("Sélectionnez d'abord un résident."));
        return;
    }

    // Récupérer les informations du résident avant suppression pour le SMS
    Resident residentASupprimer;
    QString nomResident;
    QString telephoneResident;
    for (const Resident &r : m_residentsComplets) {
        if (r.id() == id) {
            residentASupprimer = r;
            nomResident = r.nomComplet();
            telephoneResident = r.telephone();
            break;
        }
    }

    const QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Suppression résident"),
        tr("Confirmez-vous la suppression du résident %1 ?").arg(id));
    if (reponse != QMessageBox::Yes) {
        return;
    }

    // Enregistrer dans l'historique AVANT la suppression (car le trigger DELETE cause des problèmes)
    Historique hist(QString(), id, "Suppression", QDateTime::currentDateTime());
    Historique::ajouter(hist);

    if (!Resident::supprimer(id)) {
        QMessageBox::critical(this, tr("Suppression résident"),
                              tr("La suppression a échoué.\n\n"
                                 "Causes possibles:\n"
                                 "• Le résident est référencé par d'autres tables (véhicules, employés, etc.)\n"
                                 "• L'ID n'existe pas dans la base de données\n"
                                 "• Problème de connexion à la base de données\n\n"
                                 "Consultez les logs pour plus de détails."));
        return;
    }

    // Envoyer un SMS de notification avant de supprimer les données
    if (!telephoneResident.isEmpty() && !nomResident.isEmpty()) {
        QString messageSms = QString("Bonjour %1,\n\nVotre dossier de résident a été supprimé de notre système.\n\nCordialement,\nÉquipe SMARTCITY")
                             .arg(nomResident);
        if (SmsManager::envoyerSms(telephoneResident, messageSms)) {
            qDebug() << "SMS de notification envoyé avec succès à" << telephoneResident;
        } else {
            qWarning() << "Échec de l'envoi du SMS de notification";
        }
    }

    rafraichirResidents();
    reinitialiserFormulaireResident();
    QMessageBox::information(this, tr("Suppression résident"),
                             tr("Résident supprimé."));
}

void MainWindow::onExporterResidentsPdf()
{
    if (ui->tableau_5->rowCount() == 0) {
        QMessageBox::information(this, tr("Export PDF"),
                                 tr("Aucun résident à exporter."));
        return;
    }

    const QString chemin = QFileDialog::getSaveFileName(
        this,
        tr("Exporter la liste des résidents"),
        QDir::homePath() + "/residents.pdf",
        tr("Documents PDF (*.pdf)"));

    if (chemin.isEmpty()) {
        return;
    }

    QString cheminPdf = chemin;
    if (!cheminPdf.endsWith(".pdf", Qt::CaseInsensitive)) {
        cheminPdf += ".pdf";
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminPdf);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    QTextDocument document;
    document.setHtml(construireHtmlResidents());
    document.print(&printer);

    QMessageBox::information(this, tr("Export PDF"),
                             tr("Le fichier %1 a été généré.").arg(QFileInfo(cheminPdf).fileName()));
}

void MainWindow::onResidentSelectionChanged()
{
    const int row = ui->tableau_5->currentRow();
    if (row < 0) {
        return;
    }

    ui->idresidentline_2->setText(ui->tableau_5->item(row, 0) ? ui->tableau_5->item(row, 0)->text() : QString());
    QTableWidgetItem *nomPrenomItem = ui->tableau_5->item(row, 1);
    QString nomAffiche = nomPrenomItem ? nomPrenomItem->data(Qt::UserRole).toString() : QString();
    QString prenomAffiche = nomPrenomItem ? nomPrenomItem->data(Qt::UserRole + 1).toString() : QString();
    if (nomAffiche.isEmpty() && nomPrenomItem) {
        const QStringList morceaux = nomPrenomItem->text().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (!morceaux.isEmpty()) {
            nomAffiche = morceaux.first();
            prenomAffiche = morceaux.mid(1).join(" ");
        }
    }
    ui->nomline_7->setText(QString("%1 %2").arg(nomAffiche, prenomAffiche).simplified());
    ui->datenaissanceline_2->setText(ui->tableau_5->item(row, 2) ? ui->tableau_5->item(row, 2)->text() : QString());
    ui->adresseline_4->setText(ui->tableau_5->item(row, 3) ? ui->tableau_5->item(row, 3)->text() : QString());
    ui->telephoneline_5->setText(ui->tableau_5->item(row, 4) ? ui->tableau_5->item(row, 4)->text() : QString());
    ui->emailline_4->setText(ui->tableau_5->item(row, 5) ? ui->tableau_5->item(row, 5)->text() : QString());
    // Mettre à jour les combobox pour statut et situation
    QString statut = ui->tableau_5->item(row, 6) ? ui->tableau_5->item(row, 6)->text() : QString();
    QString situation = ui->tableau_5->item(row, 7) ? ui->tableau_5->item(row, 7)->text() : QString();
    
    int indexStatut = ui->statusline_2->findText(statut);
    if (indexStatut >= 0) {
        ui->statusline_2->setCurrentIndex(indexStatut);
    }
    
    int indexSituation = ui->situationline_2->findText(situation);
    if (indexSituation >= 0) {
        ui->situationline_2->setCurrentIndex(indexSituation);
    }
}

void MainWindow::onRechercherResident()
{
    QString critere = ui->recherche_4 ? ui->recherche_4->text().trimmed() : QString();
    QString typeTri = ui->triemail_4 ? ui->triemail_4->currentText() : QString();
    
    filtrerResidents(critere, typeTri);
}

void MainWindow::onTriResident()
{
    if (!ui->triemail_4) {
        return;
    }
    
    QString critere = ui->recherche_4 ? ui->recherche_4->text().trimmed() : QString();
    QString typeTri = ui->triemail_4->currentText();
    
    filtrerResidents(critere, typeTri);
}

void MainWindow::filtrerResidents(const QString &critere, const QString &typeTri)
{
    QList<Resident> residentsFiltres = m_residentsComplets;
    
    // Recherche par plusieurs critères : ID, nom, prénom, situation familiale, statut, adresse, téléphone, email
    if (!critere.isEmpty()) {
        QList<Resident> temp;
        for (const Resident &r : residentsFiltres) {
            // Rechercher dans tous les champs pertinents
            bool correspond = 
                r.id().contains(critere, Qt::CaseInsensitive) ||
                r.nomComplet().contains(critere, Qt::CaseInsensitive) ||
                r.nom().contains(critere, Qt::CaseInsensitive) ||
                r.prenom().contains(critere, Qt::CaseInsensitive) ||
                r.situation().contains(critere, Qt::CaseInsensitive) ||
                r.statut().contains(critere, Qt::CaseInsensitive) ||
                r.adresse().contains(critere, Qt::CaseInsensitive) ||
                r.telephone().contains(critere, Qt::CaseInsensitive) ||
                r.email().contains(critere, Qt::CaseInsensitive);
            
            if (correspond) {
                temp.append(r);
            }
        }
        residentsFiltres = temp;
    }
    
    // Tri - ignorer "Trier par" qui est juste un placeholder
    if (typeTri == "Trier par" || typeTri.isEmpty()) {
        // Pas de tri, garder l'ordre original
    } else if (typeTri == "Nom (A-Z)") {
        std::sort(residentsFiltres.begin(), residentsFiltres.end(), 
                  [](const Resident &a, const Resident &b) {
                      return a.nomComplet() < b.nomComplet();
                  });
    } else if (typeTri == "Email (A-Z)") {
        std::sort(residentsFiltres.begin(), residentsFiltres.end(), 
                  [](const Resident &a, const Resident &b) {
                      return a.email() < b.email();
                  });
    } else if (typeTri == "Email (Z-A)") {
        std::sort(residentsFiltres.begin(), residentsFiltres.end(), 
                  [](const Resident &a, const Resident &b) {
                      return a.email() > b.email();
                  });
    } else if (typeTri == "Âge croissant") {
        std::sort(residentsFiltres.begin(), residentsFiltres.end(), 
                  [this](const Resident &a, const Resident &b) {
                      return calculerAge(a.dateNaissance()) < calculerAge(b.dateNaissance());
                  });
    } else if (typeTri == "Âge décroissant") {
        std::sort(residentsFiltres.begin(), residentsFiltres.end(), 
                  [this](const Resident &a, const Resident &b) {
                      return calculerAge(a.dateNaissance()) > calculerAge(b.dateNaissance());
                  });
    }
    
    afficherResidents(residentsFiltres);
}

void MainWindow::onAfficherStatistiques()
{
    afficherStatistiquesCercle();
}

void MainWindow::afficherStatistiquesCercle()
{
    // Calculer les tranches d'âge
    QMap<QString, int> tranchesAge;
    tranchesAge["0-10"] = 0;
    tranchesAge["11-20"] = 0;
    tranchesAge["21-30"] = 0;
    tranchesAge["31-40"] = 0;
    tranchesAge["41-50"] = 0;
    tranchesAge["51-60"] = 0;
    tranchesAge["61-70"] = 0;
    tranchesAge["71+"] = 0;
    
    for (const Resident &resident : m_residentsComplets) {
        if (!resident.dateNaissance().isValid()) {
            continue;
        }
        
        int age = calculerAge(resident.dateNaissance());
        if (age <= 10) {
            tranchesAge["0-10"]++;
        } else if (age <= 20) {
            tranchesAge["11-20"]++;
        } else if (age <= 30) {
            tranchesAge["21-30"]++;
        } else if (age <= 40) {
            tranchesAge["31-40"]++;
        } else if (age <= 50) {
            tranchesAge["41-50"]++;
        } else if (age <= 60) {
            tranchesAge["51-60"]++;
        } else if (age <= 70) {
            tranchesAge["61-70"]++;
        } else {
            tranchesAge["71+"]++;
        }
    }
    
    // Créer un graphique en cercle (pie chart)
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Statistiques par tranches d'âge"));
    dialog->resize(600, 500);
    
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    
    // Créer le graphique
    QPieSeries *series = new QPieSeries();
    
    QList<QColor> colors = {
        QColor("#FF6384"), QColor("#36A2EB"), QColor("#FFCE56"), QColor("#4BC0C0"),
        QColor("#9966FF"), QColor("#FF9F40"), QColor("#FF6384"), QColor("#C9CBCF")
    };
    
    int colorIndex = 0;
    for (auto it = tranchesAge.begin(); it != tranchesAge.end(); ++it) {
        if (it.value() > 0) {
            QPieSlice *slice = series->append(QString("%1 ans: %2").arg(it.key(), QString::number(it.value())), it.value());
            slice->setColor(colors[colorIndex % colors.size()]);
            slice->setLabelVisible(true);
            colorIndex++;
        }
    }
    
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Répartition des résidents par tranches d'âge"));
    chart->legend()->setAlignment(Qt::AlignRight);
    
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    layout->addWidget(chartView);
    
    // Afficher le dialogue
    dialog->exec();
    delete dialog;
}

int MainWindow::calculerAge(const QDate &dateNaissance) const
{
    if (!dateNaissance.isValid()) {
        return 0;
    }
    
    QDate aujourdhui = QDate::currentDate();
    int age = aujourdhui.year() - dateNaissance.year();
    
    if (aujourdhui.month() < dateNaissance.month() ||
        (aujourdhui.month() == dateNaissance.month() && aujourdhui.day() < dateNaissance.day())) {
        age--;
    }
    
    return age;
}

void MainWindow::onAfficherHistorique()
{
    if (!ui->stackedWidget_4) {
        return;
    }
    
    ui->stackedWidget_4->setCurrentWidget(ui->page_13);
    rafraichirHistorique();
}

void MainWindow::rafraichirHistorique()
{
    if (!ui->tableWidget) {
        return;
    }
    
    QList<Historique> historiques;
    
    // Filtrer par action si une combobox existe
    if (ui->comboBoxAction) {
        QString actionFiltre = ui->comboBoxAction->currentText();
        historiques = Historique::filtrerParAction(actionFiltre);
    } else {
        historiques = Historique::recupererTout();
    }
    
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(historiques.size());
    
    qDebug() << "Rafraîchissement de l'historique - Nombre d'éléments:" << historiques.size();
    
    int row = 0;
    for (const Historique &h : historiques) {
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(h.idHistorique()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(h.idResident()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(h.action()));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(h.dateAction().toString("dd/MM/yyyy HH:mm")));
        qDebug() << "Ligne" << row << "- ID:" << h.idHistorique() << "ID_Resident:" << h.idResident() << "Action:" << h.action();
        ++row;
    }
    
    qDebug() << "Tableau mis à jour avec" << row << "lignes";
}

void MainWindow::onExporterHistoriquePdf()
{
    if (!ui->tableWidget || ui->tableWidget->rowCount() == 0) {
        QMessageBox::information(this, tr("Export PDF"),
                                 tr("Aucun historique à exporter."));
        return;
    }
    
    const QString chemin = QFileDialog::getSaveFileName(
        this,
        tr("Exporter l'historique"),
        QDir::homePath() + "/historique_residents.pdf",
        tr("Documents PDF (*.pdf)"));
    
    if (chemin.isEmpty()) {
        return;
    }
    
    QString cheminPdf = chemin;
    if (!cheminPdf.endsWith(".pdf", Qt::CaseInsensitive)) {
        cheminPdf += ".pdf";
    }
    
    QString html = QStringLiteral("<h2>Historique des résidents</h2>");
    html += QStringLiteral("<table border='1' cellspacing='0' cellpadding='4'>"
                           "<tr>"
                           "<th>ID Historique</th>"
                           "<th>ID Resident</th>"
                           "<th>Action</th>"
                           "<th>Date</th>"
                           "</tr>");
    
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        html += QStringLiteral("<tr>");
        for (int col = 0; col < 4; ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            html += QStringLiteral("<td>%1</td>").arg(item ? item->text().toHtmlEscaped() : QString());
        }
        html += QStringLiteral("</tr>");
    }
    html += QStringLiteral("</table>");
    
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminPdf);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));
    
    QTextDocument document;
    document.setHtml(html);
    document.print(&printer);
    
    QMessageBox::information(this, tr("Export PDF"),
                             tr("Le fichier %1 a été généré.").arg(QFileInfo(cheminPdf).fileName()));
}

void MainWindow::onFermerHistorique()
{
    if (!ui->stackedWidget_4) {
        return;
    }
    
    ui->stackedWidget_4->setCurrentWidget(ui->page_12);
}

void MainWindow::onViderHistorique()
{
    const QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Vider l'historique"),
        tr("Êtes-vous sûr de vouloir vider tout l'historique ?"));
    
    if (reponse != QMessageBox::Yes) {
        return;
    }
    
    Historique::vider();
    rafraichirHistorique();
    QMessageBox::information(this, tr("Vider l'historique"),
                             tr("L'historique a été vidé."));
}

void MainWindow::onAjouterReclamation()
{
    const QString id = residentSelectionneId();
    if (id.isEmpty()) {
        QMessageBox::warning(this, tr("Réclamation"),
                             tr("Sélectionnez d'abord un résident."));
        return;
    }
    
    // Ici, vous pouvez ajouter une boîte de dialogue pour saisir la réclamation
    // Pour l'instant, on l'ajoute simplement dans la colonne réclamation du tableau
    const int row = ui->tableau_5->currentRow();
    if (row >= 0) {
        QTableWidgetItem *item = ui->tableau_5->item(row, 8);
        if (item) {
            QString reclamation = item->text();
            if (!reclamation.isEmpty()) {
                reclamation += "; ";
            }
            reclamation += QDateTime::currentDateTime().toString("dd/MM/yyyy");
            item->setText(reclamation);
        }
    }
    
    QMessageBox::information(this, tr("Réclamation"),
                             tr("Réclamation ajoutée pour le résident %1.").arg(id));
}

void MainWindow::testerEnvoiSms()
{
    QString numeroTest = "+21628500392";
    QString message = QString(
        "Bonjour !\n\n"
        "Ceci est un SMS de test depuis l'application SmartCity.\n\n"
        "Date/Heure: %1\n\n"
        "Cordialement,\nÉquipe SmartCity"
    ).arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    
    qDebug() << "📱 Test d'envoi SMS à:" << numeroTest;
    qDebug() << "Message:" << message;
    
    bool success = SmsManager::envoyerSms(numeroTest, message);
    
    if (success) {
        QMessageBox::information(this, tr("SMS Test"),
                                 tr("✓ SMS de test envoyé avec succès à %1 !\n\n"
                                    "Vérifiez votre téléphone.").arg(numeroTest));
        qDebug() << "✓ SMS de test envoyé avec succès";
    } else {
        QMessageBox::warning(this, tr("Erreur SMS Test"),
                            tr("✗ Échec de l'envoi du SMS de test.\n\n"
                               "Consultez la console pour plus de détails."));
        qWarning() << "✗ Échec de l'envoi du SMS de test";
    }
}

void MainWindow::onEnvoyerSms()
{
    // Option 1: Envoi de test direct
    QMessageBox::StandardButton choix = QMessageBox::question(
        this,
        tr("Envoyer SMS"),
        tr("Voulez-vous :\n\n"
           "• OUI : Envoyer un SMS de TEST à +21628500392\n"
           "• NON : Envoyer un SMS au résident sélectionné"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
    );
    
    if (choix == QMessageBox::Cancel) {
        return;
    }
    
    if (choix == QMessageBox::Yes) {
        // Envoi de test
        testerEnvoiSms();
        return;
    }
    
    // Option 2: Envoi au résident sélectionné
    const int row = ui->tableau_5->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Envoyer SMS"),
                             tr("Sélectionnez d'abord un résident dans le tableau."));
        return;
    }
    
    // Récupérer les informations du résident sélectionné
    QTableWidgetItem *idItem = ui->tableau_5->item(row, 0);
    QTableWidgetItem *nomItem = ui->tableau_5->item(row, 1);
    QTableWidgetItem *telephoneItem = ui->tableau_5->item(row, 4);
    
    if (!telephoneItem || telephoneItem->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Envoyer SMS"),
                             tr("Ce résident n'a pas de numéro de téléphone enregistré."));
        return;
    }
    
    QString idResident = idItem ? idItem->text().trimmed() : QString();
    QString nomResident = nomItem ? nomItem->text().trimmed() : QString();
    QString telephone = telephoneItem->text().trimmed();
    
    // Demander le message à envoyer
    bool ok;
    QString message = QInputDialog::getMultiLineText(
        this,
        tr("Envoyer SMS"),
        tr("Entrez le message à envoyer à %1 (%2):").arg(nomResident, telephone),
        QString(),
        &ok
    );
    
    if (!ok || message.trimmed().isEmpty()) {
        return;
    }
    
    // Envoyer le SMS (méthode statique)
    qDebug() << "Tentative d'envoi SMS à:" << telephone << "pour résident:" << nomResident;
    bool success = SmsManager::envoyerSms(telephone, message);
    
    if (success) {
        QMessageBox::information(this, tr("SMS envoyé"),
                                 tr("Le SMS a été envoyé avec succès à %1 (%2).")
                                 .arg(nomResident, telephone));
    } else {
        // Message d'erreur plus détaillé
        QString messageErreur = tr("Une erreur s'est produite lors de l'envoi du SMS.\n\n");
        messageErreur += tr("Numéro de téléphone: %1\n").arg(telephone);
        messageErreur += tr("Résident: %1\n\n").arg(nomResident);
        messageErreur += tr("Vérifiez que:\n");
        messageErreur += tr("• Le numéro de téléphone est au format international valide (ex: +216XXXXXXXX)\n");
        messageErreur += tr("• Votre compte Twilio est actif\n");
        messageErreur += tr("• Vous avez des crédits Twilio disponibles\n");
        messageErreur += tr("• Le numéro est vérifié dans votre compte Twilio (pour les comptes d'essai)\n\n");
        messageErreur += tr("Consultez la console pour plus de détails sur l'erreur.");
        
        QMessageBox::warning(this, tr("Erreur d'envoi"), messageErreur);
    }
}

void MainWindow::onSmsRecu(const QString &from, const QString &to, const QString &message, const QDateTime &dateTime)
{
    qDebug() << "SMS reçu dans MainWindow:" << from << "->" << to << ":" << message;
    
    // Afficher une notification
    QString notification = QString("Nouveau SMS reçu\n\n")
                          .append(tr("De: %1\n").arg(from))
                          .append(tr("Vers: %2\n").arg(to))
                          .append(tr("Message: %1\n").arg(message))
                          .append(tr("Date: %1").arg(dateTime.toString("dd/MM/yyyy HH:mm:ss")));
    
    QMessageBox::information(this, tr("Nouveau SMS"), notification);
}

void MainWindow::onAfficherSmsRecus()
{
    QMessageBox::information(this, tr("SMS reçus"),
                             tr("Les SMS reçus sont affichés dans la console de débogage.\n"
                                "Pour voir les SMS, consultez les logs de l'application."));
}

// ============================================================
// GESTION DES EMPLOYÉS
// ============================================================

void MainWindow::chargerEmployes()
{
    if (!ui->tableau) {
        qWarning() << "Tableau des employés non trouvé !";
        return;
    }
    
    QString errorText;
    employesCache = Employee::fetchAll(errorText);
    
    if (!errorText.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Impossible de charger les employés:\n" + errorText);
        return;
    }
    
    ui->tableau->clearContents();
    ui->tableau->setRowCount(employesCache.size());
    
    for (int i = 0; i < employesCache.size(); ++i) {
        const Employee &emp = employesCache[i];
        ui->tableau->setItem(i, 0, new QTableWidgetItem(QString::number(emp.idEmploye)));
        ui->tableau->setItem(i, 1, new QTableWidgetItem(emp.nom));
        ui->tableau->setItem(i, 2, new QTableWidgetItem(emp.prenom));
        ui->tableau->setItem(i, 3, new QTableWidgetItem(emp.email));
        ui->tableau->setItem(i, 4, new QTableWidgetItem(emp.poste));
        ui->tableau->setItem(i, 5, new QTableWidgetItem(QString::number(emp.salaire)));
        ui->tableau->setItem(i, 6, new QTableWidgetItem(emp.adresse));
        ui->tableau->setItem(i, 7, new QTableWidgetItem(QString::number(emp.telephone)));
    }
    
    qDebug() << "✓" << employesCache.size() << "employés chargés";
}

void MainWindow::onAjouterEmploye()
{
    if (!ui->nomline || !ui->prenomline || !ui->emailline || !ui->posteline || !ui->salaireline
        || !ui->adresseline || !ui->telephoneline) {
        QMessageBox::warning(this, "Erreur", "Les champs du formulaire employé sont introuvables !");
        return;
    }
    
    Employee emp;
    emp.nom = ui->nomline->text().trimmed();
    emp.prenom = ui->prenomline->text().trimmed();
    emp.email = ui->emailline->text().trimmed();
    emp.poste = ui->posteline->text().trimmed();
    emp.salaire = ui->salaireline->text().toDouble();
    emp.adresse = ui->adresseline->text().trimmed();
    emp.telephone = ui->telephoneline->text().toLongLong();
    emp.password = ui->passwordline ? ui->passwordline->text().trimmed() : "password123";
    
    // Validation
    if (emp.nom.isEmpty() || emp.prenom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom et le prénom sont obligatoires !");
        return;
    }
    
    int newId;
    QString errorText;
    if (emp.insert(newId, errorText)) {
        QMessageBox::information(this, "Succès", "Employé ajouté avec l'ID: " + QString::number(newId));
        reinitialiserFormulaireEmploye();
        chargerEmployes();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout:\n" + errorText);
    }
}

void MainWindow::onModifierEmploye()
{
    if (!ui->tableau) {
        return;
    }
    
    int row = ui->tableau->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un employé à modifier !");
        return;
    }
    
    int id = ui->tableau->item(row, 0)->text().toInt();
    
    Employee emp;
    emp.idEmploye = id;
    emp.nom = ui->nomline->text().trimmed();
    emp.prenom = ui->prenomline->text().trimmed();
    emp.email = ui->emailline->text().trimmed();
    emp.poste = ui->posteline->text().trimmed();
    emp.salaire = ui->salaireline->text().toDouble();
    emp.adresse = ui->adresseline->text().trimmed();
    emp.telephone = ui->telephoneline->text().toLongLong();
    emp.password = ui->passwordline ? ui->passwordline->text().trimmed() : "password123";
    
    QString errorText;
    if (emp.updateById(id, errorText)) {
        QMessageBox::information(this, "Succès", "Employé modifié avec succès !");
        reinitialiserFormulaireEmploye();
        chargerEmployes();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification:\n" + errorText);
    }
}

void MainWindow::onSupprimerEmploye()
{
    if (!ui->tableau) {
        return;
    }
    
    int row = ui->tableau->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un employé à supprimer !");
        return;
    }
    
    int id = ui->tableau->item(row, 0)->text().toInt();
    QString nom = ui->tableau->item(row, 1)->text();
    QString prenom = ui->tableau->item(row, 2)->text();
    
    auto reponse = QMessageBox::question(this, "Confirmation",
                                          "Supprimer l'employé " + nom + " " + prenom + " ?",
                                          QMessageBox::Yes | QMessageBox::No);
    
    if (reponse != QMessageBox::Yes) {
        return;
    }
    
    QString errorText;
    if (Employee::removeById(id, errorText)) {
        QMessageBox::information(this, "Succès", "Employé supprimé avec succès !");
        reinitialiserFormulaireEmploye();
        chargerEmployes();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression:\n" + errorText);
    }
}

void MainWindow::onEmployeSelectionChanged()
{
    if (!ui->tableau) {
        return;
    }
    
    int row = ui->tableau->currentRow();
    if (row < 0 || row >= employesCache.size()) {
        return;
    }
    
    // Récupérer l'employé depuis le cache
    const Employee &emp = employesCache[row];
    
    // Remplir le formulaire avec les données sélectionnées
    if (ui->nomline) ui->nomline->setText(emp.nom);
    if (ui->prenomline) ui->prenomline->setText(emp.prenom);
    if (ui->emailline) ui->emailline->setText(emp.email);
    if (ui->posteline) ui->posteline->setText(emp.poste);
    if (ui->salaireline) ui->salaireline->setText(QString::number(emp.salaire));
    if (ui->adresseline) ui->adresseline->setText(emp.adresse);
    if (ui->telephoneline) ui->telephoneline->setText(QString::number(emp.telephone));
    if (ui->passwordline) ui->passwordline->setText(emp.password);
}

void MainWindow::reinitialiserFormulaireEmploye()
{
    if (ui->nomline) ui->nomline->clear();
    if (ui->prenomline) ui->prenomline->clear();
    if (ui->emailline) ui->emailline->clear();
    if (ui->posteline) ui->posteline->clear();
    if (ui->salaireline) ui->salaireline->clear();
    if (ui->adresseline) ui->adresseline->clear();
    if (ui->telephoneline) ui->telephoneline->clear();
    if (ui->passwordline) ui->passwordline->clear();
}

void MainWindow::onCapturerVisage()
{
    // Vérifier qu'un employé est sélectionné
    if (ui->tableau && ui->tableau->currentRow() >= 0) {
        int row = ui->tableau->currentRow();
        int employeId = ui->tableau->item(row, 0)->text().toInt();
        
        // Ouvrir le dialogue de capture
        FaceCaptureDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QImage capturedImage = dialog.getCapturedImage();
            
            if (!capturedImage.isNull()) {
                QString errorText;
                if (FacialRecognition::saveFaceImage(employeId, capturedImage, errorText)) {
                    QMessageBox::information(this, "Succès", 
                        "La photo de visage a été enregistrée avec succès!");
                } else {
                    QMessageBox::warning(this, "Erreur", 
                        "Erreur lors de l'enregistrement de la photo:\n" + errorText);
                }
            }
        }
    } else {
        QMessageBox::warning(this, "Attention", 
            "Veuillez sélectionner un employé dans le tableau.");
    }
}


// ============================================================
// GESTION DES VÉHICULES
// ============================================================

void MainWindow::chargerVehicules()
{
    if (!ui->tableau_3) {
        qWarning() << "Tableau des véhicules non trouvé !";
        return;
    }
    
    Vehicule v;
    QSqlQuery query = v.afficher();
    
    ui->tableau_3->clearContents();
    ui->tableau_3->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableau_3->insertRow(row);
        ui->tableau_3->setItem(row, 0, new QTableWidgetItem(query.value("IMMATRICULATION").toString()));
        ui->tableau_3->setItem(row, 1, new QTableWidgetItem(query.value("MARQUE").toString()));
        ui->tableau_3->setItem(row, 2, new QTableWidgetItem(query.value("MODELE").toString()));
        ui->tableau_3->setItem(row, 3, new QTableWidgetItem(query.value("TYPE").toString()));
        ui->tableau_3->setItem(row, 4, new QTableWidgetItem(query.value("ETAT").toString()));
        ui->tableau_3->setItem(row, 5, new QTableWidgetItem(query.value("SERVICE").toString()));
        // Essayer DATE_MAINTENANCE, sinon DATEMAINTENCE
        QString dateMaint = query.value("DATE_MAINTENANCE").toString();
        if (dateMaint.isEmpty()) {
            dateMaint = query.value("DATEMAINTENCE").toString();
        }
        ui->tableau_3->setItem(row, 6, new QTableWidgetItem(dateMaint));
        ++row;
    }
    
    qDebug() << "✓" << row << "véhicules chargés";
}

void MainWindow::onAjouterVehicule()
{
    if (!ui->immatline_2 || !ui->marqueline_2 || !ui->modeleline_2 || !ui->triemail_2 || !ui->Etatline_2
        || !ui->serviceline_2 || !ui->datemaintline_2) {
        QMessageBox::warning(this, "Erreur", "Les champs du formulaire véhicule sont introuvables !");
        return;
    }
    
    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    QString type = ui->Typeline_2 ? ui->Typeline_2->text().trimmed() : (ui->triemail_2 ? ui->triemail_2->currentText().trimmed() : "");
    QString etat = ui->Etatline_2->currentText().trimmed();
    QString service = ui->serviceline_2->text().trimmed();
    
    // Parse date
    QString dateStr = ui->datemaintline_2->text().trimmed();
    QDate dateMaint = QDate::fromString(dateStr, "dd/MM/yyyy");
    if (!dateMaint.isValid()) {
        dateMaint = QDate::fromString(dateStr, "yyyy-MM-dd");
    }
    if (!dateMaint.isValid()) {
        dateMaint = QDate::currentDate(); // Par défaut
    }
    
    // Validation
    if (immat.isEmpty() || marque.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'immatriculation et la marque sont obligatoires !");
        return;
    }
    
    Vehicule v(immat, marque, modele, type, etat, service, dateMaint);
    
    if (v.ajouter()) {
        QMessageBox::information(this, "Succès", "Véhicule ajouté avec succès !");
        reinitialiserFormulaireVehicule();
        chargerVehicules();
    } else {
        QString errorMsg = "Échec de l'ajout du véhicule !\n\n";
        errorMsg += "Causes possibles:\n";
        errorMsg += "• L'immatriculation " + immat + " existe déjà\n";
        errorMsg += "• Problème de connexion à la base de données\n";
        errorMsg += "• Structure de table incorrecte\n\n";
        errorMsg += "Consultez la console Qt pour plus de détails techniques.";
        QMessageBox::critical(this, "Erreur", errorMsg);
    }
}

void MainWindow::onModifierVehicule()
{
    if (!ui->tableau_3) {
        return;
    }
    
    int row = ui->tableau_3->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un véhicule à modifier !");
        return;
    }
    
    QString oldImmat = ui->tableau_3->item(row, 0)->text();
    
    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    QString type = ui->Typeline_2 ? ui->Typeline_2->text().trimmed() : (ui->triemail_2 ? ui->triemail_2->currentText().trimmed() : "");
    QString etat = ui->Etatline_2->currentText().trimmed();
    QString service = ui->serviceline_2->text().trimmed();
    
    QString dateStr = ui->datemaintline_2->text().trimmed();
    QDate dateMaint = QDate::fromString(dateStr, "dd/MM/yyyy");
    if (!dateMaint.isValid()) {
        dateMaint = QDate::fromString(dateStr, "yyyy-MM-dd");
    }
    if (!dateMaint.isValid()) {
        dateMaint = QDate::currentDate();
    }
    
    Vehicule v(immat, marque, modele, type, etat, service, dateMaint);
    
    // Validation des champs obligatoires
    if (immat.isEmpty()) {
        QMessageBox::warning(this, "Validation", "L'immatriculation est obligatoire !");
        return;
    }
    if (marque.isEmpty()) {
        QMessageBox::warning(this, "Validation", "La marque est obligatoire !");
        return;
    }
    
    if (v.modifier(oldImmat)) {
        QMessageBox::information(this, "Succès", "Véhicule modifié avec succès !");
        reinitialiserFormulaireVehicule();
        chargerVehicules();
    } else {
        QString errorMsg = "Échec de la modification du véhicule !\n\n";
        errorMsg += "Causes possibles:\n";
        errorMsg += "• L'immatriculation " + oldImmat + " n'existe pas dans la base\n";
        if (immat != oldImmat) {
            errorMsg += "• La nouvelle immatriculation " + immat + " existe déjà\n";
        }
        errorMsg += "• Problème de connexion à la base de données\n";
        errorMsg += "• Contrainte de clé étrangère\n\n";
        errorMsg += "Consultez la console Qt pour plus de détails techniques.";
        QMessageBox::critical(this, "Erreur", errorMsg);
    }
}

void MainWindow::onSupprimerVehicule()
{
    if (!ui->tableau_3) {
        return;
    }
    
    int row = ui->tableau_3->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un véhicule à supprimer !");
        return;
    }
    
    QString immat = ui->tableau_3->item(row, 0)->text();
    QString marque = ui->tableau_3->item(row, 1)->text();
    QString modele = ui->tableau_3->item(row, 2)->text();
    
    auto reponse = QMessageBox::question(this, "Confirmation",
                                          "Supprimer le véhicule " + immat + " (" + marque + " " + modele + ") ?",
                                          QMessageBox::Yes | QMessageBox::No);
    
    if (reponse != QMessageBox::Yes) {
        return;
    }
    
    Vehicule v;
    if (v.supprimer(immat)) {
        QMessageBox::information(this, "Succès", "Véhicule supprimé avec succès !");
        reinitialiserFormulaireVehicule();
        chargerVehicules();
    } else {
        QString errorMsg = "Échec de la suppression du véhicule !\n\n";
        errorMsg += "Vérifiez:\n";
        errorMsg += "• Que le véhicule est bien sélectionné\n";
        errorMsg += "• Qu'il n'y a pas de contraintes de clé étrangère\n";
        errorMsg += "• La console pour plus de détails";
        QMessageBox::critical(this, "Erreur", errorMsg);
    }
}

void MainWindow::onVehiculeSelectionChanged()
{
    if (!ui->tableau_3) {
        return;
    }
    
    int row = ui->tableau_3->currentRow();
    if (row < 0) {
        selectedImmatVehicule = "";
        return;
    }
    
    // Sauvegarder l'immatriculation sélectionnée pour les recommandations
    selectedImmatVehicule = ui->tableau_3->item(row, 0)->text();
    qDebug() << "✅ Véhicule sélectionné:" << selectedImmatVehicule;
    
    // Remplir le formulaire avec les données sélectionnées
    if (ui->immatline_2) ui->immatline_2->setText(ui->tableau_3->item(row, 0)->text());
    if (ui->marqueline_2) ui->marqueline_2->setText(ui->tableau_3->item(row, 1)->text());
    if (ui->modeleline_2) ui->modeleline_2->setText(ui->tableau_3->item(row, 2)->text());
    if (ui->Typeline_2) ui->Typeline_2->setText(ui->tableau_3->item(row, 3)->text());
    if (ui->triemail_2) ui->triemail_2->setCurrentText(ui->tableau_3->item(row, 3)->text());
    if (ui->Etatline_2) ui->Etatline_2->setCurrentText(ui->tableau_3->item(row, 4)->text());
    if (ui->serviceline_2) ui->serviceline_2->setText(ui->tableau_3->item(row, 5)->text());
    if (ui->datemaintline_2) ui->datemaintline_2->setText(ui->tableau_3->item(row, 6)->text());
}

void MainWindow::reinitialiserFormulaireVehicule()
{
    if (ui->immatline_2) ui->immatline_2->clear();
    if (ui->marqueline_2) ui->marqueline_2->clear();
    if (ui->modeleline_2) ui->modeleline_2->clear();
    if (ui->Typeline_2) ui->Typeline_2->clear();
    if (ui->triemail_2) ui->triemail_2->setCurrentIndex(-1);
    if (ui->Etatline_2) ui->Etatline_2->setCurrentIndex(-1);
    if (ui->serviceline_2) ui->serviceline_2->clear();
    if (ui->datemaintline_2) ui->datemaintline_2->clear();
}

// Envoyer les informations du véhicule au LCD Arduino
void MainWindow::envoyerVehiculeAuLCD(const QString &immat, const QString &marque, const QString &modele, const QString &type)
{
    Q_UNUSED(type);
    if (!arduinoRFID || !arduinoRFID->isConnected()) {
        QMessageBox::warning(this, "Arduino", "Arduino non connecté !");
        return;
    }
    
    // Fonction pour remplacer les caractères accentués par leur équivalent sans accent
    auto removeAccents = [](QString text) -> QString {
        text.replace("é", "e").replace("è", "e").replace("ê", "e").replace("ë", "e");
        text.replace("à", "a").replace("â", "a").replace("ä", "a");
        text.replace("ù", "u").replace("û", "u").replace("ü", "u");
        text.replace("ô", "o").replace("ö", "o");
        text.replace("î", "i").replace("ï", "i");
        text.replace("ç", "c");
        text.replace("É", "E").replace("È", "E").replace("Ê", "E").replace("Ë", "E");
        text.replace("À", "A").replace("Â", "A").replace("Ä", "A");
        text.replace("Ù", "U").replace("Û", "U").replace("Ü", "U");
        text.replace("Ô", "O").replace("Ö", "O");
        text.replace("Î", "I").replace("Ï", "I");
        text.replace("Ç", "C");
        return text;
    };
    
    // Format: LCD|ligne1|ligne2
    // Ligne 1: Matricule
    // Ligne 2: Marque Modele (sans accents pour compatibilité LCD)
    QString ligne1 = QString("Mat:%1").arg(immat);
    QString ligne2 = QString("%1 %2").arg(removeAccents(marque), removeAccents(modele));
    
    // Limiter à 16 caractères par ligne pour LCD 16x2
    if (ligne1.length() > 16) ligne1 = ligne1.left(16);
    if (ligne2.length() > 16) ligne2 = ligne2.left(16);
    
    QString commande = QString("LCD|%1|%2\n").arg(ligne1, ligne2);
    
    qDebug() << "📟 Envoi au LCD:" << commande;
    arduinoRFID->getSerialPort()->write(commande.toLatin1());  // Utiliser toLatin1() au lieu de toUtf8()
    arduinoRFID->getSerialPort()->flush();
    
    QMessageBox::information(this, "LCD Arduino", 
        QString("Informations envoyées au LCD:\n\n%1\n%2").arg(ligne1, ligne2));
}

// Bouton: Rechercher matricule et envoyer au LCD
void MainWindow::on_btnRechercherMatriculeLCD_clicked()
{
    QString immatRecherche = ui->lineEditMatriculeLCD->text().trimmed().toUpper();
    
    if (immatRecherche.isEmpty()) {
        QMessageBox::warning(this, "Recherche", "Veuillez entrer une matricule !");
        return;
    }
    
    // Rechercher le véhicule dans la base de données
    QSqlQuery query;
    query.prepare("SELECT IMMATRICULATION, MARQUE, MODELE, TYPE FROM GEST_VEHICULE WHERE UPPER(IMMATRICULATION) = :immat");
    query.bindValue(":immat", immatRecherche);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur de recherche:\n" + query.lastError().text());
        return;
    }
    
    if (query.next()) {
        // Véhicule trouvé
        QString immat = query.value(0).toString();
        QString marque = query.value(1).toString();
        QString modele = query.value(2).toString();
        QString type = query.value(3).toString();
        
        // Envoyer au LCD
        envoyerVehiculeAuLCD(immat, marque, modele, type);
    } else {
        QMessageBox::warning(this, "Véhicule introuvable", 
            QString("Aucun véhicule avec la matricule: %1").arg(immatRecherche));
    }
}

// Bouton: OCR pour analyser une image de plaque d'immatriculation (Tesseract local)
void MainWindow::on_btnOCRMatricule_clicked()
{
    // Demander à l'utilisateur s'il veut utiliser un fichier ou la caméra
    QMessageBox msgBox;
    msgBox.setWindowTitle("OCR Plaque d'immatriculation");
    msgBox.setText("Choisissez la source de l'image:");
    msgBox.setIcon(QMessageBox::Question);
    
    QPushButton *btnFichier = msgBox.addButton("📁 Fichier Image", QMessageBox::ActionRole);
    msgBox.addButton("❌ Annuler", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() != btnFichier) {
        return;  // Annulé
    }
    
    // Ouvrir un dialogue pour sélectionner une image
    QString fileName = QFileDialog::getOpenFileName(this,
        "📷 Sélectionner une image de plaque", 
        QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.bmp)");
    
    if (fileName.isEmpty()) {
        return; // L'utilisateur a annulé
    }
    
    // Vérifier que l'image existe
    if (!QFile::exists(fileName)) {
        QMessageBox::warning(this, "Erreur", "Le fichier image n'existe pas !");
        return;
    }
    
    // Appeler la fonction d'analyse OCR
    analyserImageOCR(fileName);
}

// Fonction de prétraitement d'image pour améliorer l'OCR
QImage MainWindow::preprocessImageForOCR(const QImage &original)
{
    // Agrandir l'image d'abord si trop petite (meilleur pour OCR)
    QImage resized = original;
    if (resized.width() < 300 || resized.height() < 80) {
        int newWidth = qMax(resized.width() * 3, 600);
        resized = resized.scaledToWidth(newWidth, Qt::SmoothTransformation);
    } else if (resized.width() < 600) {
        resized = resized.scaledToWidth(resized.width() * 2, Qt::SmoothTransformation);
    }
    
    // Convertir en niveaux de gris
    QImage gray = resized.convertToFormat(QImage::Format_Grayscale8);
    
    // Améliorer le contraste avant le seuillage
    int minVal = 255, maxVal = 0;
    for (int y = 0; y < gray.height(); ++y) {
        const uchar *line = gray.constScanLine(y);
        for (int x = 0; x < gray.width(); ++x) {
            if (line[x] < minVal) minVal = line[x];
            if (line[x] > maxVal) maxVal = line[x];
        }
    }
    
    // Étirer le contraste si l'image est trop "plate"
    if (maxVal - minVal > 20 && maxVal - minVal < 200) {
        double scale = 255.0 / (maxVal - minVal);
        for (int y = 0; y < gray.height(); ++y) {
            uchar *line = gray.scanLine(y);
            for (int x = 0; x < gray.width(); ++x) {
                int newVal = (int)((line[x] - minVal) * scale);
                line[x] = qBound(0, newVal, 255);
            }
        }
    }
    
    // Calculer le seuil optimal avec la méthode d'Otsu simplifiée
    int histogram[256] = {0};
    for (int y = 0; y < gray.height(); ++y) {
        const uchar *line = gray.constScanLine(y);
        for (int x = 0; x < gray.width(); ++x) {
            histogram[line[x]]++;
        }
    }
    
    int totalPixels = gray.width() * gray.height();
    int sum = 0;
    for (int i = 0; i < 256; ++i) {
        sum += i * histogram[i];
    }
    
    int sumB = 0;
    int wB = 0;
    int wF = 0;
    double maxVariance = 0;
    int optimalThreshold = 128;
    
    for (int t = 0; t < 256; ++t) {
        wB += histogram[t];
        if (wB == 0) continue;
        
        wF = totalPixels - wB;
        if (wF == 0) break;
        
        sumB += t * histogram[t];
        
        double mB = (double)sumB / wB;
        double mF = (double)(sum - sumB) / wF;
        
        double variance = (double)wB * wF * (mB - mF) * (mB - mF);
        
        if (variance > maxVariance) {
            maxVariance = variance;
            optimalThreshold = t;
        }
    }
    
    // Créer une version binarisée (seuillage d'Otsu)
    QImage binary = gray.copy();
    for (int y = 0; y < binary.height(); ++y) {
        uchar *line = binary.scanLine(y);
        for (int x = 0; x < binary.width(); ++x) {
            line[x] = (line[x] > optimalThreshold) ? 255 : 0;
        }
    }
    
    // Ajouter une bordure blanche autour de l'image (aide Tesseract)
    int border = 20;
    QImage result(binary.width() + 2 * border, binary.height() + 2 * border, QImage::Format_Grayscale8);
    result.fill(255);  // Remplir en blanc
    
    // Copier l'image binarisée au centre
    for (int y = 0; y < binary.height(); ++y) {
        const uchar *srcLine = binary.constScanLine(y);
        uchar *dstLine = result.scanLine(y + border);
        for (int x = 0; x < binary.width(); ++x) {
            dstLine[x + border] = srcLine[x];
        }
    }
    
    return result;
}

void MainWindow::analyserImageOCR(const QString &imagePath)
{
    // Chemin vers Tesseract
    QString tesseractPath = "C:/Program Files/Tesseract-OCR/tesseract.exe";
    
    if (!QFile::exists(tesseractPath)) {
        QMessageBox::critical(this, "Erreur", 
            "❌ Tesseract OCR n'est pas installé !\n\n"
            "Installez-le depuis:\n"
            "https://github.com/UB-Mannheim/tesseract/wiki");
        return;
    }
    
    // Charger l'image originale
    QImage originalImage(imagePath);
    if (originalImage.isNull()) {
        QMessageBox::warning(this, "Erreur", "Impossible de charger l'image !");
        return;
    }
    
    // Utiliser le prétraitement amélioré avec seuillage d'Otsu
    QImage processedImage = preprocessImageForOCR(originalImage);
    
    // Sauvegarder l'image prétraitée
    QString tempImagePath = QDir::temp().filePath("ocr_preprocessed.png");
    processedImage.save(tempImagePath, "PNG");
    
    // Créer un fichier temporaire pour le résultat
    QString tempOutput = QDir::temp().filePath("ocr_result");
    
    // Préparer la commande Tesseract
    QProcess *process = new QProcess(this);
    
    // Arguments optimisés pour les plaques d'immatriculation
    // --psm 6 = Assumer un bloc de texte uniforme (meilleur pour plaques)
    // --psm 7 = Traiter comme une ligne unique
    // --psm 8 = Traiter comme un mot unique
    // --psm 13 = Traiter comme une ligne de texte brut
    // -c tessedit_char_whitelist = limiter aux caractères de plaque
    QStringList arguments;
    arguments << tempImagePath;      // Image prétraitée
    arguments << tempOutput;         // Fichier de sortie (sans extension)
    arguments << "-l" << "eng";      // Langue: anglais
    arguments << "--psm" << "6";     // Mode: bloc de texte uniforme
    arguments << "--oem" << "3";     // Mode OCR: LSTM + Legacy
    arguments << "-c" << "tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789- ";
    
    // Afficher un message de traitement
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("Analyse OCR");
    msgBox->setText("🔍 Analyse de l'image en cours avec Tesseract...\n\nPrétraitement: Amélioration contraste + Seuillage d'Otsu");
    msgBox->setStandardButtons(QMessageBox::NoButton);
    msgBox->setModal(false);
    msgBox->show();
    QApplication::processEvents();
    
    // Connexion pour gérer la fin du processus
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, process, tempOutput, tempImagePath, msgBox, tesseractPath](int exitCode, QProcess::ExitStatus exitStatus) {
        
        // Fermer le message de chargement
        if (msgBox) {
            msgBox->close();
            msgBox->deleteLater();
        }
        
        if (exitStatus != QProcess::NormalExit || exitCode != 0) {
            QMessageBox::critical(this, "Erreur OCR", 
                "❌ Erreur lors de l'exécution de Tesseract.\n\n" + 
                process->readAllStandardError());
            process->deleteLater();
            return;
        }
        
        // Lire le résultat (Tesseract ajoute .txt automatiquement)
        QFile resultFile(tempOutput + ".txt");
        if (!resultFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Erreur", "Impossible de lire le résultat OCR !");
            process->deleteLater();
            return;
        }
        
        QString textDetecte = QString::fromUtf8(resultFile.readAll()).trimmed().toUpper();
        resultFile.close();
        
        // Supprimer le fichier temporaire
        resultFile.remove();
        
        // Si aucun texte détecté, essayer avec PSM 7 (ligne unique) puis PSM 8 (mot unique)
        if (textDetecte.isEmpty()) {
            // Essayer avec PSM 7
            QProcess retryProcess;
            QStringList retryArgs;
            retryArgs << tempImagePath << tempOutput << "-l" << "eng" << "--psm" << "7" << "--oem" << "3";
            retryArgs << "-c" << "tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789- ";
            retryProcess.start(tesseractPath, retryArgs);
            retryProcess.waitForFinished(5000);
            
            QFile retryFile(tempOutput + ".txt");
            if (retryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                textDetecte = QString::fromUtf8(retryFile.readAll()).trimmed().toUpper();
                retryFile.close();
                retryFile.remove();
            }
        }
        
        // Si toujours rien, essayer PSM 8
        if (textDetecte.isEmpty()) {
            QProcess retryProcess2;
            QStringList retryArgs2;
            retryArgs2 << tempImagePath << tempOutput << "-l" << "eng" << "--psm" << "8" << "--oem" << "3";
            retryArgs2 << "-c" << "tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789- ";
            retryProcess2.start(tesseractPath, retryArgs2);
            retryProcess2.waitForFinished(5000);
            
            QFile retryFile2(tempOutput + ".txt");
            if (retryFile2.open(QIODevice::ReadOnly | QIODevice::Text)) {
                textDetecte = QString::fromUtf8(retryFile2.readAll()).trimmed().toUpper();
                retryFile2.close();
                retryFile2.remove();
            }
        }
        
        // Supprimer l'image prétraitée
        QFile::remove(tempImagePath);
        
        if (textDetecte.isEmpty()) {
            QMessageBox::warning(this, "OCR", 
                "❌ Aucun texte détecté dans l'image.\n\n"
                "Conseils pour améliorer la détection:\n"
                "• Recadrez l'image sur la plaque uniquement\n"
                "• Utilisez une image nette et bien éclairée\n"
                "• Assurez-vous que la plaque est horizontale\n"
                "• Évitez les reflets, ombres et angles\n"
                "• Résolution minimale recommandée: 300x100 pixels\n\n"
                "💡 Astuce: Prenez une photo rapprochée de la plaque seule.");
        } else {
            // Nettoyer le texte pour ne garder que les caractères alphanumériques et les tirets
            QString matricule;
            for (QChar c : textDetecte) {
                if (c.isLetterOrNumber() || c == '-' || c == ' ') {
                    matricule += c;
                }
            }
            // Supprimer les espaces multiples
            matricule = matricule.simplified();
            
            // Mettre à jour le champ
            ui->lineEditMatriculeLCD->setText(matricule);
            
            QMessageBox::information(this, "✅ OCR Réussi", 
                QString("Texte détecté:\n%1\n\nMatricule extraite:\n%2\n\n"
                        "Vous pouvez maintenant l'afficher sur le LCD.")
                .arg(textDetecte, matricule));
        }
        
        process->deleteLater();
    });
    
    // Lancer le processus Tesseract
    process->start(tesseractPath, arguments);
    
    // Timeout de 10 secondes
    if (!process->waitForStarted(10000)) {
        msgBox->close();
        msgBox->deleteLater();
        QMessageBox::critical(this, "Erreur", "Impossible de démarrer Tesseract !");
        process->deleteLater();
    }
}

// ============================================================
// GESTION DES MAISONS
// ============================================================

void MainWindow::chargerMaisons()
{
    if (!ui->tableau_7) {
        qWarning() << "Tableau des maisons non trouvé !";
        return;
    }
    
    Maison::afficher(ui->tableau_7);
    qDebug() << "✓ Maisons chargées";
}

void MainWindow::onAjouterMaison()
{
    if (!ui->idmaisonline || !ui->adresseline_2 || !ui->nivsecline) {
        QMessageBox::warning(this, "Erreur", "Les champs du formulaire maison sont introuvables !");
        return;
    }
    
    // Récupérer les données
    QString idStr = ui->idmaisonline->text().trimmed();
    QString adresse = ui->adresseline_2->text().trimmed();
    QString securiteStr = ui->nivsecline->text().trimmed();
    
    // Validation basique
    bool ok;
    int id = idStr.toInt(&ok);
    if (!ok || id <= 0) {
        QMessageBox::warning(this, "Erreur", "L'ID doit être un nombre positif !");
        return;
    }
    
    int securite = securiteStr.toInt(&ok);
    if (!ok || securite < 0 || securite > 10) {
        QMessageBox::warning(this, "Erreur", "Le niveau de sécurité doit être entre 0 et 10 !");
        return;
    }
    
    if (adresse.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'adresse est obligatoire !");
        return;
    }
    
    // Vérifier si l'ID existe déjà
    if (Maison::idExists(id)) {
        QMessageBox::warning(this, "Erreur", "L'ID " + QString::number(id) + " existe déjà !");
        return;
    }
    
    // Créer et ajouter la maison
    Maison m;
    m.setId(id);
    m.setAdresse(adresse);
    m.setSecurite(securite);
    m.setStatut("Libre"); // Statut par défaut
    m.setType("Maison"); // Type par défaut
    m.setNbrPieces(3); // Nombre de pièces par défaut
    
    QString errorText;
    if (m.ajouter(id, &errorText)) {
        QMessageBox::information(this, "Succès", "Maison ajoutée avec succès !");
        
        // Créer une alerte automatiquement si le niveau de sécurité est faible (< 3)
        if (securite < 3) {
            Alerte alerte(id, adresse, securite, "En attente");
            if (alerte.ajouter()) {
                qDebug() << "Alerte créée automatiquement pour la maison" << id << "avec niveau de sécurité" << securite;
            }
        }
        
        reinitialiserFormulaireMaison();
        chargerMaisons();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout:\n" + errorText);
    }
}

void MainWindow::onModifierMaison()
{
    if (!ui->tableau_7) {
        return;
    }
    
    int row = ui->tableau_7->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une maison à modifier !");
        return;
    }
    
    // Récupérer l'ancien ID
    int oldId = ui->tableau_7->item(row, 0)->text().toInt();
    
    // Récupérer les nouvelles données
    QString idStr = ui->idmaisonline->text().trimmed();
    QString adresse = ui->adresseline_2->text().trimmed();
    QString securiteStr = ui->nivsecline->text().trimmed();
    
    bool ok;
    int newId = idStr.toInt(&ok);
    if (!ok || newId <= 0) {
        QMessageBox::warning(this, "Erreur", "L'ID doit être un nombre positif !");
        return;
    }
    
    int securite = securiteStr.toInt(&ok);
    if (!ok || securite < 0 || securite > 10) {
        QMessageBox::warning(this, "Erreur", "Le niveau de sécurité doit être entre 0 et 10 !");
        return;
    }
    
    if (adresse.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'adresse est obligatoire !");
        return;
    }
    
    // Créer l'objet maison avec les nouvelles valeurs
    Maison m;
    m.setId(newId);
    m.setAdresse(adresse);
    m.setSecurite(securite);
    m.setStatut("Occupée"); // Statut par défaut pour modification
    m.setType("Maison");
    m.setNbrPieces(3);
    
    QString errorText;
    if (m.modifier(oldId, newId, &errorText)) {
        QMessageBox::information(this, "Succès", "Maison modifiée avec succès !");
        
        // Créer une alerte automatiquement si le niveau de sécurité est faible (< 3)
        // et qu'il n'existe pas déjà d'alerte active pour cette maison
        if (securite < 3 && !Alerte::existeAlertePourMaison(newId)) {
            Alerte alerte(newId, adresse, securite, "En attente");
            if (alerte.ajouter()) {
                qDebug() << "Alerte créée automatiquement pour la maison" << newId << "avec niveau de sécurité" << securite;
            }
        }
        
        reinitialiserFormulaireMaison();
        chargerMaisons();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification:\n" + errorText);
    }
}

void MainWindow::onSupprimerMaison()
{
    if (!ui->tableau_7) {
        return;
    }
    
    int row = ui->tableau_7->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une maison à supprimer !");
        return;
    }
    
    int id = ui->tableau_7->item(row, 0)->text().toInt();
    QString adresse = ui->tableau_7->item(row, 1)->text();
    
    auto reponse = QMessageBox::question(this, "Confirmation",
                                          "Supprimer la maison " + QString::number(id) + " (" + adresse + ") ?",
                                          QMessageBox::Yes | QMessageBox::No);
    
    if (reponse != QMessageBox::Yes) {
        return;
    }
    
    Maison m;
    QString errorText;
    if (m.supprimer(id, &errorText)) {
        QMessageBox::information(this, "Succès", "Maison supprimée avec succès !");
        reinitialiserFormulaireMaison();
        chargerMaisons();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression:\n" + errorText);
    }
}

void MainWindow::onMaisonSelectionChanged()
{
    if (!ui->tableau_7) {
        return;
    }
    
    int row = ui->tableau_7->currentRow();
    if (row < 0) {
        return;
    }
    
    // Récupérer l'ID de la maison sélectionnée
    int idMaison = ui->tableau_7->item(row, 0)->text().toInt();
    
    // Remplir le formulaire avec les données sélectionnées
    if (ui->idmaisonline) ui->idmaisonline->setText(ui->tableau_7->item(row, 0)->text());
    if (ui->adresseline_2) ui->adresseline_2->setText(ui->tableau_7->item(row, 1)->text());
    if (ui->nivsecline) ui->nivsecline->setText(ui->tableau_7->item(row, 2)->text());
    
    // Remplir le ComboBox avec tous les résidents disponibles
    if (ui->comboResidentsMaison) {
        ui->comboResidentsMaison->clear();
        ui->comboResidentsMaison->addItem("-- Sélectionner un résident à assigner --", "");
        
        // Récupérer tous les résidents
        QList<Resident> tousResidents = Resident::recupererTout();
        for (const Resident &resident : tousResidents) {
            QString display = QString("%1 %2 - %3")
                .arg(resident.nom(), resident.prenom(), resident.telephone());
            ui->comboResidentsMaison->addItem(display, resident.id());
        }
    }
    
    // Afficher les résidents actuels de cette maison
    if (ui->labelResidentsMaison) {
        QStringList residents = Maison::getResidentsParMaison(idMaison);
        if (residents.isEmpty()) {
            ui->labelResidentsMaison->setText("Résidents actuels : Aucun");
        } else {
            QString texte = QString("Résidents actuels (%1):\n").arg(residents.count());
            texte += residents.join("\n");
            ui->labelResidentsMaison->setText(texte);
        }
    }
}

void MainWindow::reinitialiserFormulaireMaison()
{
    if (ui->idmaisonline) ui->idmaisonline->clear();
    if (ui->adresseline_2) ui->adresseline_2->clear();
    if (ui->nivsecline) ui->nivsecline->clear();
    if (ui->comboResidentsMaison) ui->comboResidentsMaison->clear();
    if (ui->labelResidentsMaison) ui->labelResidentsMaison->setText("Résidents actuels : -");
}

// ============================================================
// GESTION DES JARDINS
// ============================================================

void MainWindow::chargerJardins()
{
    if (!ui->tableau_8) {
        return;
    }

    QString error;
    const QList<Jardin> jardins = Jardin::fetchAll(&error);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Jardins"), tr("Impossible de charger les jardins :\n%1").arg(error));
        return;
    }

    ui->tableau_8->clearContents();
    ui->tableau_8->setRowCount(jardins.size());

    const QLocale locale;
    int row = 0;
    for (const Jardin &j : jardins) {
        ui->tableau_8->setItem(row, 0, new QTableWidgetItem(QString::number(j.id())));
        ui->tableau_8->setItem(row, 1, new QTableWidgetItem(j.emplacement()));
        ui->tableau_8->setItem(row, 2, new QTableWidgetItem(locale.toString(j.superficie(), 'f', 2)));
        ui->tableau_8->setItem(row, 3, new QTableWidgetItem(j.typeSol()));
        ui->tableau_8->setItem(row, 4, new QTableWidgetItem(locale.toString(j.temperatureMoyenneSol(), 'f', 1) + " °C"));
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 5, typeItem);
        ++row;
    }
}

void MainWindow::onAjouterJardin()
{
    if (!ui->idjardinline || !ui->emplacementline || !ui->superficieline || !ui->superficieline_2) {
        QMessageBox::warning(this, tr("Jardins"), tr("Formulaire incomplet"));
        return;
    }

    bool ok = false;
    const int id = ui->idjardinline->text().trimmed().toInt(&ok);
    if (!ok || id <= 0) {
        QMessageBox::warning(this, tr("Jardins"), tr("L'identifiant doit être un entier positif."));
        return;
    }

    if (Jardin::idExists(id)) {
        QMessageBox::warning(this, tr("Jardins"), tr("L'identifiant %1 existe déjà.").arg(id));
        return;
    }

    const QString emplacement = ui->emplacementline->text().trimmed();
    if (emplacement.isEmpty()) {
        QMessageBox::warning(this, tr("Jardins"), tr("L'emplacement est obligatoire."));
        return;
    }

    const double superficie = ui->superficieline->text().trimmed().toDouble(&ok);
    if (!ok || superficie <= 0) {
        QMessageBox::warning(this, tr("Jardins"), tr("La superficie doit être un nombre positif."));
        return;
    }

    const QString typeSol = ui->superficieline_2->text().trimmed();
    if (typeSol.isEmpty()) {
        QMessageBox::warning(this, tr("Jardins"), tr("Le type de sol est obligatoire."));
        return;
    }

    double temperatureMoyenneSol = 0.0;
    if (ui->tempsolline) {
        temperatureMoyenneSol = ui->tempsolline->text().trimmed().toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, tr("Jardins"), tr("La température doit être un nombre valide."));
            return;
        }
    }

    const QString typeChoix = ui->typechoix ? ui->typechoix->currentText().trimmed() : QString();
    if (typeChoix.isEmpty()) {
        QMessageBox::warning(this, tr("Jardins"), tr("Veuillez sélectionner le type de jardin."));
        return;
    }

    Jardin jardin(id, emplacement, superficie, typeSol, temperatureMoyenneSol, typeChoix);
    QString error;
    if (jardin.ajouter(&error)) {
        chargerJardins();
        reinitialiserFormulaireJardin();
        QMessageBox::information(this, tr("Jardins"), tr("Jardin ajouté avec succès."));
    } else {
        QMessageBox::critical(this, tr("Jardins"), tr("Échec de l'ajout :\n%1").arg(error));
    }
}

void MainWindow::onModifierJardin()
{
    if (!ui->tableau_8 || ui->tableau_8->currentRow() < 0) {
        QMessageBox::warning(this, tr("Jardins"), tr("Veuillez sélectionner un jardin."));
        return;
    }

    const int oldId = ui->tableau_8->item(ui->tableau_8->currentRow(), 0)->text().toInt();

    bool ok = false;
    const int newId = ui->idjardinline->text().trimmed().toInt(&ok);
    if (!ok || newId <= 0) {
        QMessageBox::warning(this, tr("Jardins"), tr("L'identifiant doit être un entier positif."));
        return;
    }

    const QString emplacement = ui->emplacementline->text().trimmed();
    if (emplacement.isEmpty()) {
        QMessageBox::warning(this, tr("Jardins"), tr("L'emplacement est obligatoire."));
        return;
    }

    const double superficie = ui->superficieline->text().trimmed().toDouble(&ok);
    if (!ok || superficie <= 0) {
        QMessageBox::warning(this, tr("Jardins"), tr("La superficie doit être un nombre positif."));
        return;
    }

    const QString typeSol = ui->superficieline_2->text().trimmed();
    const QString typeChoix = ui->typechoix ? ui->typechoix->currentText().trimmed() : QString();

    double temperatureMoyenneSol = 0.0;
    if (ui->tempsolline) {
        bool tempOk = false;
        temperatureMoyenneSol = ui->tempsolline->text().trimmed().toDouble(&tempOk);
        if (!tempOk) {
            QMessageBox::warning(this, tr("Jardins"), tr("La température doit être un nombre valide."));
            return;
        }
    }

    Jardin jardin(newId, emplacement, superficie, typeSol, temperatureMoyenneSol, typeChoix);
    QString error;
    if (jardin.modifier(oldId, &error)) {
        chargerJardins();
        reinitialiserFormulaireJardin();
        QMessageBox::information(this, tr("Jardins"), tr("Jardin modifié avec succès."));
    } else {
        QMessageBox::critical(this, tr("Jardins"), tr("Échec de la modification :\n%1").arg(error));
    }
}

void MainWindow::onSupprimerJardin()
{
    if (!ui->tableau_8 || ui->tableau_8->currentRow() < 0) {
        QMessageBox::warning(this, tr("Jardins"), tr("Veuillez sélectionner un jardin."));
        return;
    }

    const int id = ui->tableau_8->item(ui->tableau_8->currentRow(), 0)->text().toInt();
    const QString emplacement = ui->tableau_8->item(ui->tableau_8->currentRow(), 1)->text();

    const auto reponse = QMessageBox::question(this,
                                               tr("Jardins"),
                                               tr("Supprimer le jardin %1 (%2) ?").arg(id).arg(emplacement));
    if (reponse != QMessageBox::Yes) {
        return;
    }

    QString error;
    if (Jardin::supprimer(id, &error)) {
        chargerJardins();
        reinitialiserFormulaireJardin();
        QMessageBox::information(this, tr("Jardins"), tr("Jardin supprimé."));
    } else {
        QMessageBox::critical(this, tr("Jardins"), tr("Échec de la suppression :\n%1").arg(error));
    }
}

void MainWindow::onExporterJardinsPdf()
{
    if (!ui->tableau_8 || ui->tableau_8->rowCount() == 0) {
        QMessageBox::information(this, tr("Jardins"), tr("Aucun jardin à exporter."));
        return;
    }

    QString chemin = QFileDialog::getSaveFileName(this,
                                                  tr("Exporter les jardins"),
                                                  QDir::homePath() + "/jardins.pdf",
                                                  tr("Documents PDF (*.pdf)"));
    if (chemin.isEmpty()) {
        return;
    }
    if (!chemin.endsWith(".pdf", Qt::CaseInsensitive)) {
        chemin += ".pdf";
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(chemin);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    QTextDocument document;
    document.setHtml(construireHtmlJardins());
    document.print(&printer);

    QMessageBox::information(this, tr("Jardins"), tr("Le fichier %1 a été généré.").arg(QFileInfo(chemin).fileName()));
}

void MainWindow::onJardinSelectionChanged()
{
    if (!ui->tableau_8) {
        return;
    }

    const int row = ui->tableau_8->currentRow();
    if (row < 0) {
        return;
    }

    if (ui->idjardinline) ui->idjardinline->setText(ui->tableau_8->item(row, 0)->text());
    if (ui->emplacementline) ui->emplacementline->setText(ui->tableau_8->item(row, 1)->text());
    if (ui->superficieline) ui->superficieline->setText(ui->tableau_8->item(row, 2)->text());
    if (ui->superficieline_2) ui->superficieline_2->setText(ui->tableau_8->item(row, 3)->text());
    if (ui->tempsolline) {
        QString tempText = ui->tableau_8->item(row, 4)->text();
        tempText.remove(" °C"); // Enlever l'unité pour l'édition
        ui->tempsolline->setText(tempText);
    }
    if (ui->typechoix) {
        const QString type = ui->tableau_8->item(row, 5)->text();
        const int index = ui->typechoix->findText(type);
        if (index >= 0) {
            ui->typechoix->setCurrentIndex(index);
        }
    }
}

void MainWindow::onTrierJardinsParType()
{
    if (!ui->tableau_8) {
        return;
    }

    QString error;
    const QList<Jardin> jardins = Jardin::fetchAllSortedByType(&error);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Jardins"), tr("Impossible de trier les jardins :\n%1").arg(error));
        return;
    }

    ui->tableau_8->clearContents();
    ui->tableau_8->setRowCount(jardins.size());

    const QLocale locale;
    int row = 0;
    for (const Jardin &j : jardins) {
        ui->tableau_8->setItem(row, 0, new QTableWidgetItem(QString::number(j.id())));
        ui->tableau_8->setItem(row, 1, new QTableWidgetItem(j.emplacement()));
        ui->tableau_8->setItem(row, 2, new QTableWidgetItem(locale.toString(j.superficie(), 'f', 2)));
        ui->tableau_8->setItem(row, 3, new QTableWidgetItem(j.typeSol()));
        ui->tableau_8->setItem(row, 4, new QTableWidgetItem(locale.toString(j.temperatureMoyenneSol(), 'f', 1) + " °C"));
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 5, typeItem);
        ++row;
    }

    QMessageBox::information(this, tr("Tri"), tr("Jardins triés par type avec succès !"));
}

void MainWindow::onTrierJardinsParId()
{
    if (!ui->tableau_8) {
        return;
    }

    QString error;
    const QList<Jardin> jardins = Jardin::fetchAllSortedById(&error);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Jardins"), tr("Impossible de trier les jardins :\n%1").arg(error));
        return;
    }

    ui->tableau_8->clearContents();
    ui->tableau_8->setRowCount(jardins.size());

    const QLocale locale;
    int row = 0;
    for (const Jardin &j : jardins) {
        ui->tableau_8->setItem(row, 0, new QTableWidgetItem(QString::number(j.id())));
        ui->tableau_8->setItem(row, 1, new QTableWidgetItem(j.emplacement()));
        ui->tableau_8->setItem(row, 2, new QTableWidgetItem(locale.toString(j.superficie(), 'f', 2)));
        ui->tableau_8->setItem(row, 3, new QTableWidgetItem(j.typeSol()));
        ui->tableau_8->setItem(row, 4, new QTableWidgetItem(locale.toString(j.temperatureMoyenneSol(), 'f', 1) + " °C"));
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 5, typeItem);
        ++row;
    }

    QMessageBox::information(this, tr("Tri"), tr("Jardins triés par ID avec succès !"));
}

void MainWindow::onTrierJardinsParSuperficie()
{
    if (!ui->tableau_8) {
        return;
    }

    QString error;
    const QList<Jardin> jardins = Jardin::fetchAllSortedBySuperficie(&error);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Jardins"), tr("Impossible de trier les jardins :\n%1").arg(error));
        return;
    }

    ui->tableau_8->clearContents();
    ui->tableau_8->setRowCount(jardins.size());

    const QLocale locale;
    int row = 0;
    for (const Jardin &j : jardins) {
        ui->tableau_8->setItem(row, 0, new QTableWidgetItem(QString::number(j.id())));
        ui->tableau_8->setItem(row, 1, new QTableWidgetItem(j.emplacement()));
        ui->tableau_8->setItem(row, 2, new QTableWidgetItem(locale.toString(j.superficie(), 'f', 2)));
        ui->tableau_8->setItem(row, 3, new QTableWidgetItem(j.typeSol()));
        ui->tableau_8->setItem(row, 4, new QTableWidgetItem(locale.toString(j.temperatureMoyenneSol(), 'f', 1) + " °C"));
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 5, typeItem);
        ++row;
    }

    QMessageBox::information(this, tr("Tri"), tr("Jardins triés par superficie avec succès !"));
}

void MainWindow::onRechercherJardin()
{
    if (!ui->tableau_8 || !ui->rechechelabel) {
        return;
    }

    QString recherche = ui->rechechelabel->text().trimmed();
    
    if (recherche.isEmpty()) {
        // Si la recherche est vide, afficher tous les jardins
        chargerJardins();
        return;
    }

    QString error;
    const QList<Jardin> tousJardins = Jardin::fetchAll(&error);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Jardins"), tr("Impossible de charger les jardins :\n%1").arg(error));
        return;
    }

    // Filtrer les jardins selon le critère de recherche
    QList<Jardin> jardinsFiltres;
    for (const Jardin &j : tousJardins) {
        if (QString::number(j.id()).contains(recherche, Qt::CaseInsensitive) ||
            j.emplacement().contains(recherche, Qt::CaseInsensitive) ||
            j.typeSol().contains(recherche, Qt::CaseInsensitive) ||
            j.typeChoix().contains(recherche, Qt::CaseInsensitive) ||
            QString::number(j.superficie()).contains(recherche)) {
            jardinsFiltres.append(j);
        }
    }

    // Afficher les résultats
    ui->tableau_8->clearContents();
    ui->tableau_8->setRowCount(jardinsFiltres.size());

    const QLocale locale;
    int row = 0;
    for (const Jardin &j : jardinsFiltres) {
        ui->tableau_8->setItem(row, 0, new QTableWidgetItem(QString::number(j.id())));
        ui->tableau_8->setItem(row, 1, new QTableWidgetItem(j.emplacement()));
        ui->tableau_8->setItem(row, 2, new QTableWidgetItem(locale.toString(j.superficie(), 'f', 2)));
        ui->tableau_8->setItem(row, 3, new QTableWidgetItem(j.typeSol()));
        ui->tableau_8->setItem(row, 4, new QTableWidgetItem(locale.toString(j.temperatureMoyenneSol(), 'f', 1) + " °C"));
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 5, typeItem);
        ++row;
    }

    if (jardinsFiltres.isEmpty()) {
        QMessageBox::information(this, tr("Recherche"), tr("Aucun jardin trouvé pour \"%1\"").arg(recherche));
    }
}

void MainWindow::reinitialiserFormulaireJardin()
{
    if (ui->idjardinline) ui->idjardinline->clear();
    if (ui->emplacementline) ui->emplacementline->clear();
    if (ui->superficieline) ui->superficieline->clear();
    if (ui->superficieline_2) ui->superficieline_2->clear();
    if (ui->tempsolline) ui->tempsolline->clear();
    if (ui->typechoix) ui->typechoix->setCurrentIndex(0);
}

QString MainWindow::construireHtmlJardins() const
{
    QString error;
    const QList<Jardin> jardins = Jardin::fetchAll(&error);
    QString html = QStringLiteral("<h2>Liste des jardins</h2>");

    if (!error.isEmpty()) {
        html += QStringLiteral("<p>%1</p>").arg(error.toHtmlEscaped());
        return html;
    }

    html += QStringLiteral("<table border='1' cellspacing='0' cellpadding='4'>");
    html += QStringLiteral("<tr><th>ID</th><th>Emplacement</th><th>Superficie</th><th>Type de sol</th><th>Température moy. sol</th><th>Type</th></tr>");
    const QLocale locale;
    for (const Jardin &j : jardins) {
        html += QStringLiteral("<tr><td>%1</td><td>%2</td><td>%3 m²</td><td>%4</td><td>%5 °C</td><td>%6</td></tr>")
                    .arg(j.id())
                    .arg(j.emplacement().toHtmlEscaped())
                    .arg(locale.toString(j.superficie(), 'f', 2))
                    .arg(j.typeSol().toHtmlEscaped())
                    .arg(locale.toString(j.temperatureMoyenneSol(), 'f', 1))
                    .arg(j.typeChoix().toHtmlEscaped());
    }
    html += QStringLiteral("</table>");
    return html;
}

void MainWindow::onAssignerResidentMaison()
{
    if (!ui->idmaisonline) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une maison d'abord !");
        return;
    }
    
    // Récupérer l'ID de la maison
    int idMaison = ui->idmaisonline->text().toInt();
    if (idMaison <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une maison valide !");
        return;
    }
    
    // Récupérer l'ID du résident depuis le ComboBox
    QString idResident;
    if (ui->comboResidentsMaison) {
        idResident = ui->comboResidentsMaison->currentData().toString();
    }
    
    if (idResident.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un résident dans la liste !");
        return;
    }
    
    // Assigner le résident à la maison
    if (Maison::assignerResidentAMaison(idResident, idMaison)) {
        QMessageBox::information(this, "Succès", "Résident assigné à la maison avec succès !");
        
        // Rafraîchir l'affichage
        chargerMaisons();
        
        // Mettre à jour la liste des résidents actuels
        if (ui->labelResidentsMaison) {
            QStringList residents = Maison::getResidentsParMaison(idMaison);
            if (residents.isEmpty()) {
                ui->labelResidentsMaison->setText("Résidents actuels : Aucun");
            } else {
                QString texte = QString("Résidents actuels (%1):\n").arg(residents.count());
                texte += residents.join("\n");
                ui->labelResidentsMaison->setText(texte);
            }
        }
        
        // Réinitialiser le ComboBox
        if (ui->comboResidentsMaison) {
            ui->comboResidentsMaison->setCurrentIndex(0);
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'assignation du résident !");
    }
}

/* ============================================================
 *                   GESTION DES ALERTES
 * ============================================================ */

void MainWindow::onGestionAlertes()
{
    // Pour l'instant, afficher les alertes dans un message box ou une boîte de dialogue
    chargerAlertes();
    QMessageBox::information(this, "Alertes", "Les alertes sont affichées dans le tableau.");
}

void MainWindow::on_Alertes_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->stackedWidget_5->setCurrentIndex(1);
    refreshAlertes();
}

void MainWindow::chargerAlertes()
{
    if (!ui->tableau_10) {
        return;
    }
    
    QSqlQuery query("SELECT ID, ID_MAISON, ZONE, NIVEAU, STATUT, DATE_ALERTE FROM GEST_ALERTES ORDER BY DATE_ALERTE DESC",
                    QSqlDatabase::database("qt_oracle"));
    
    ui->tableau_10->clearContents();
    ui->tableau_10->setRowCount(0);
    
    // Configurer 7 colonnes (6 données + 1 bouton localisation)
    ui->tableau_10->setColumnCount(7);
    
    // Définir les en-têtes
    QStringList headers;
    headers << "ID" << "ID Maison" << "Zone" << "Niveau" << "Statut" << "Date" << "Localisation";
    ui->tableau_10->setHorizontalHeaderLabels(headers);
    
    int row = 0;
    while (query.next()) {
        ui->tableau_10->insertRow(row);
        
        QString zone = query.value("ZONE").toString();
        
        ui->tableau_10->setItem(row, 0, new QTableWidgetItem(query.value("ID").toString()));
        ui->tableau_10->setItem(row, 1, new QTableWidgetItem(query.value("ID_MAISON").toString()));
        ui->tableau_10->setItem(row, 2, new QTableWidgetItem(zone));
        ui->tableau_10->setItem(row, 3, new QTableWidgetItem(query.value("NIVEAU").toString()));
        ui->tableau_10->setItem(row, 4, new QTableWidgetItem(query.value("STATUT").toString()));
        ui->tableau_10->setItem(row, 5, new QTableWidgetItem(query.value("DATE_ALERTE").toDate().toString("dd/MM/yyyy")));
        
        // Créer un bouton "Localiser" pour chaque alerte
        QPushButton *btnLocaliser = new QPushButton("🗺️ Localiser");
        btnLocaliser->setStyleSheet("background-color: #007bff; color: white; border-radius: 6px; padding: 4px 8px; font-weight: bold;");
        btnLocaliser->setCursor(Qt::PointingHandCursor);
        
        // Connecter le bouton à la fonction de localisation avec la zone capturée
        connect(btnLocaliser, &QPushButton::clicked, this, [this, zone]() {
            loadMapForZone(zone);
        });
        
        ui->tableau_10->setCellWidget(row, 6, btnLocaliser);
        
        row++;
    }
    
    if (ui->tableau_10->horizontalHeader()) {
        ui->tableau_10->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
}

void MainWindow::onAjouterAlerte()
{
    // Utiliser des boîtes de dialogue pour saisir les informations
    bool ok;
    int idMaison = QInputDialog::getInt(this, "Ajouter Alerte", "ID de la maison:", 1, 1, 10000, 1, &ok);
    if (!ok) return;
    
    QString zone = QInputDialog::getText(this, "Ajouter Alerte", "Zone/Adresse:", QLineEdit::Normal, "", &ok);
    if (!ok || zone.isEmpty()) return;
    
    int niveau = QInputDialog::getInt(this, "Ajouter Alerte", "Niveau de sécurité (1-5):", 1, 1, 5, 1, &ok);
    if (!ok) return;
    
    QStringList statuts = {"En attente", "En cours", "Traitée"};
    QString statut = QInputDialog::getItem(this, "Ajouter Alerte", "Statut:", statuts, 0, false, &ok);
    if (!ok) return;
    
    Alerte alerte(idMaison, zone, niveau, statut);
    
    if (alerte.ajouter()) {
        QMessageBox::information(this, "Succès", "Alerte ajoutée avec succès !");
        chargerAlertes();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout de l'alerte !");
    }
}

void MainWindow::onModifierAlerte()
{
    if (!ui->tableau_10) {
        return;
    }
    
    int row = ui->tableau_10->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une alerte à modifier !");
        return;
    }
    
    int id = ui->tableau_10->item(row, 0)->text().toInt();
    int currentIdMaison = ui->tableau_10->item(row, 1)->text().toInt();
    QString currentZone = ui->tableau_10->item(row, 2)->text();
    int currentNiveau = ui->tableau_10->item(row, 3)->text().toInt();
    QString currentStatut = ui->tableau_10->item(row, 4)->text();
    
    bool ok;
    int idMaison = QInputDialog::getInt(this, "Modifier Alerte", "ID de la maison:", currentIdMaison, 1, 10000, 1, &ok);
    if (!ok) return;
    
    QString zone = QInputDialog::getText(this, "Modifier Alerte", "Zone/Adresse:", QLineEdit::Normal, currentZone, &ok);
    if (!ok) return;
    
    int niveau = QInputDialog::getInt(this, "Modifier Alerte", "Niveau:", currentNiveau, 1, 5, 1, &ok);
    if (!ok) return;
    
    QStringList statuts = {"En attente", "En cours", "Traitée"};
    QString statut = QInputDialog::getItem(this, "Modifier Alerte", "Statut:", statuts, statuts.indexOf(currentStatut), false, &ok);
    if (!ok) return;
    
    Alerte alerte(idMaison, zone, niveau, statut);
    
    if (alerte.modifier(id)) {
        QMessageBox::information(this, "Succès", "Alerte modifiée avec succès !");
        chargerAlertes();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification de l'alerte !");
    }
}

void MainWindow::onSupprimerAlerte()
{
    if (!ui->tableau_10) {
        return;
    }
    
    int row = ui->tableau_10->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une alerte à supprimer !");
        return;
    }
    
    int id = ui->tableau_10->item(row, 0)->text().toInt();
    QString zone = ui->tableau_10->item(row, 2)->text();
    
    auto reponse = QMessageBox::question(this, "Confirmation",
                                          "Supprimer l'alerte " + QString::number(id) + " pour " + zone + " ?",
                                          QMessageBox::Yes | QMessageBox::No);
    
    if (reponse != QMessageBox::Yes) {
        return;
    }
    
    Alerte alerte;
    if (alerte.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Alerte supprimée avec succès !");
        reinitialiserFormulaireAlerte();
        chargerAlertes();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression de l'alerte !");
    }
}

void MainWindow::onAlerteSelectionChanged()
{
    if (!ui->tableau_10) {
        return;
    }
    
    int row = ui->tableau_10->currentRow();
    if (row < 0) {
        return;
    }
    
    // Afficher les informations de l'alerte sélectionnée (optionnel)
    // Les données sont déjà visibles dans le tableau
}

void MainWindow::onMarquerAlerteTraitee()
{
    if (!ui->tableau_10) {
        return;
    }
    
    int row = ui->tableau_10->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une alerte !");
        return;
    }
    
    int id = ui->tableau_10->item(row, 0)->text().toInt();
    
    Alerte alerte;
    if (alerte.marquerCommeTraitee(id)) {
        QMessageBox::information(this, "Succès", "Alerte marquée comme traitée !");
        chargerAlertes();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la mise à jour !");
    }
}

void MainWindow::onRetourAlertes()
{
    // Retour à la page des maisons
    if (ui->stackedWidget && ui->pageMaisons) {
        ui->stackedWidget->setCurrentWidget(ui->pageMaisons);
    }
}

void MainWindow::on_btnRetourAlertes_clicked()
{
    ui->stackedWidget_5->setCurrentIndex(0);
}

void MainWindow::reinitialiserFormulaireAlerte()
{
    // Pas de formulaire à réinitialiser car on utilise des dialogues
}

void MainWindow::connectAlerteButtons()
{
    // Connecter les boutons de gestion des alertes si disponibles dans l'UI
    // Les boutons spécifiques aux alertes peuvent être ajoutés plus tard dans l'UI Designer
    
    if (ui->btnRetourAlertes) {
        connect(ui->btnRetourAlertes, &QPushButton::clicked, this, &MainWindow::onRetourAlertes);
    }
    if (ui->btnAfficherCarte) {
        connect(ui->btnAfficherCarte, &QPushButton::clicked, this, &MainWindow::onAfficherCarte);
    }
    if (ui->tableau_10) {
        connect(ui->tableau_10, &QTableWidget::itemSelectionChanged, this, &MainWindow::onAlerteSelectionChanged);
    }
}

void MainWindow::refreshAlertes()
{
    chargerAlertes();
}

/* ============================================================
 *          CARTE ET LOCALISATION DES ALERTES
 * ============================================================ */

void MainWindow::onAfficherCarte()
{
    if (!ui->tableau_10) {
        return;
    }
    
    int row = ui->tableau_10->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Carte", "Veuillez sélectionner une alerte.");
        return;
    }
    
    // Récupérer la zone de l'alerte sélectionnée (colonne 2)
    QString zone = ui->tableau_10->item(row, 2)->text();
    loadMapForZone(zone);
}

void MainWindow::loadMapForZone(const QString &zone)
{
    if (!net) {
        QMessageBox::warning(this, "Erreur", "Gestionnaire réseau non disponible.");
        return;
    }
    
    // Géocodage de l'adresse via Nominatim (OpenStreetMap)
    QUrl geocode("https://nominatim.openstreetmap.org/search");
    QUrlQuery query;
    query.addQueryItem("q", zone);
    query.addQueryItem("format", "json");
    query.addQueryItem("limit", "1");
    geocode.setQuery(query);
    
    QNetworkRequest req(geocode);
    req.setRawHeader("User-Agent", QByteArray("QtSmartCity/1.0"));
    
    QNetworkReply *r = net->get(req);
    connect(r, &QNetworkReply::finished, this, [this, r]() {
        if (r->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "Carte", "Géocodage indisponible.");
            r->deleteLater();
            return;
        }
        
        QByteArray data = r->readAll();
        r->deleteLater();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray() || doc.array().isEmpty()) {
            QMessageBox::information(this, "Carte", "Adresse introuvable.");
            return;
        }
        
        QJsonObject obj = doc.array().first().toObject();
        QString lat = obj.value("lat").toString();
        QString lon = obj.value("lon").toString();
        
        // Essayer de charger une carte statique
        QUrl imgUrl(QString("http://staticmap.openstreetmap.de/staticmap.php?center=%1,%2&zoom=14&size=370x230&markers=%1,%2,red-pushpin")
                        .arg(lat, lon));
        QNetworkRequest imgReq(imgUrl);
        imgReq.setRawHeader("User-Agent", QByteArray("QtSmartCity/1.0"));
        
        QNetworkReply *ri = net->get(imgReq);
        connect(ri, &QNetworkReply::finished, this, [this, ri, lat, lon]() {
            if (ri->error() == QNetworkReply::NoError) {
                QByteArray bytes = ri->readAll();
                ri->deleteLater();
                QPixmap pix;
                if (pix.loadFromData(bytes)) {
                    sceneCarte->clear();
                    sceneCarte->addPixmap(pix);
                    if (viewCarte) viewCarte->show();
                    return;
                }
            }
            ri->deleteLater();
            
            // Fallback : charger une tuile OpenStreetMap
            bool okLat = false, okLon = false;
            double dLat = lat.toDouble(&okLat);
            double dLon = lon.toDouble(&okLon);
            
            if (!okLat || !okLon) {
                QMessageBox::warning(this, "Carte", "Localisation introuvable.");
                return;
            }
            
            // Calculer la tuile OSM
            int zoom = 14;
            double latRad = qDegreesToRadians(dLat);
            double n = (1 << zoom);
            int xTile = qFloor((dLon + 180.0) / 360.0 * n);
            int yTile = qFloor((1.0 - log(tan(latRad) + 1.0 / cos(latRad)) / M_PI) / 2.0 * n);
            
            QUrl tileUrl(QString("http://tile.openstreetmap.org/%1/%2/%3.png")
                            .arg(QString::number(zoom), QString::number(xTile), QString::number(yTile)));
            QNetworkRequest tileReq(tileUrl);
            tileReq.setRawHeader("User-Agent", QByteArray("QtSmartCity/1.0"));
            
            QNetworkReply *rt = net->get(tileReq);
            connect(rt, &QNetworkReply::finished, this, [this, rt, dLat, dLon, zoom, xTile, yTile]() {
                if (rt->error() != QNetworkReply::NoError) {
                    QMessageBox::warning(this, "Carte", "Impossible de charger la localisation.");
                    rt->deleteLater();
                    return;
                }
                
                QByteArray tileBytes = rt->readAll();
                rt->deleteLater();
                
                QPixmap tilePix;
                if (!tilePix.loadFromData(tileBytes)) {
                    QMessageBox::information(this, "Carte", "Image de tuile invalide.");
                    return;
                }
                
                // Calculer la position du marqueur sur la tuile
                double latRad2 = qDegreesToRadians(dLat);
                double n2 = (1 << zoom);
                double pixelX = ((dLon + 180.0) / 360.0) * 256.0 * n2;
                double pixelY = ((1.0 - log(tan(latRad2) + 1.0 / cos(latRad2)) / M_PI) / 2.0) * 256.0 * n2;
                double localX = pixelX - 256.0 * xTile;
                double localY = pixelY - 256.0 * yTile;
                
                // Dessiner le marqueur rouge
                QPixmap composed = tilePix.copy();
                QPainter p(&composed);
                p.setRenderHint(QPainter::Antialiasing, true);
                p.setPen(QPen(Qt::red, 2));
                p.setBrush(QBrush(Qt::red));
                p.drawEllipse(QPointF(localX, localY), 6, 6);
                p.end();
                
                // Afficher la carte
                sceneCarte->clear();
                sceneCarte->addPixmap(composed);
                if (viewCarte) viewCarte->show();
            });
        });
    });
}


// ===========================================================================================
// FONCTIONS POUR CHATBOT ET RECOMMANDATIONS IA - VÉHICULES
// À ajouter à la fin de mainwindow.cpp
// ===========================================================================================

// ========== CHATBOT VÉHICULE ==========
void MainWindow::on_btnChatbotVehicule_clicked()
{
    if (ui->stackedWidget_3) {
        ui->stackedWidget_3->setCurrentWidget(ui->page_10);
    }
}

void MainWindow::on_btnBackFromChatVehicule_clicked()
{
    if (ui->stackedWidget_3) {
        ui->stackedWidget_3->setCurrentWidget(ui->page_9);
    }
}

void MainWindow::on_btnSendChatVehicule_clicked()
{
    if (!ui->textEditChatInputVehicule || !ui->textEditChatOutputVehicule) {
        return;
    }
    
    QString userMsg = ui->textEditChatInputVehicule->toPlainText().trimmed();
    if (userMsg.isEmpty()) {
        return;
    }
    
    // Afficher le message de l'utilisateur
    QString current = ui->textEditChatOutputVehicule->toPlainText();
    current += "\n🧑 Vous: " + userMsg + "\n";
    ui->textEditChatOutputVehicule->setPlainText(current);
    ui->textEditChatInputVehicule->clear();
    
    // Ajouter message de chargement
    current += "🤖 SmartHelp IA: Analyse en cours...\n";
    ui->textEditChatOutputVehicule->setPlainText(current);
    
    // Récupérer le contexte de la base de données
    QString contextDB = getVehiculesDatabaseContext();
    
    // Construire le prompt complet avec contexte
    QString fullPrompt = QString("Contexte de la base de données véhicules:\n%1\n\nQuestion de l'utilisateur: %2")
        .arg(contextDB)
        .arg(userMsg);
    
    // Envoyer à Azure OpenAI pour réponse dynamique
    sendMessageToAzureAI(fullPrompt);
    
    // Auto-scroll vers le bas
    QTextCursor cursor = ui->textEditChatOutputVehicule->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEditChatOutputVehicule->setTextCursor(cursor);
}

QString MainWindow::getVehiculesDatabaseContext()
{
    QString context = "";
    
    // Statistiques globales
    QSqlQuery countQuery;
    countQuery.prepare("SELECT COUNT(*) FROM VEHICULE");
    if (countQuery.exec() && countQuery.next()) {
        int total = countQuery.value(0).toInt();
        context += QString("Nombre total de véhicules: %1\n").arg(total);
    }
    
    // Répartition par état
    QSqlQuery etatQuery;
    etatQuery.prepare("SELECT ETAT, COUNT(*) FROM VEHICULE GROUP BY ETAT");
    if (etatQuery.exec()) {
        context += "Répartition par état:\n";
        while (etatQuery.next()) {
            context += QString("  - %1: %2 véhicule(s)\n")
                .arg(etatQuery.value(0).toString())
                .arg(etatQuery.value(1).toInt());
        }
    }
    
    // Liste des véhicules (limité à 20 pour ne pas surcharger)
    QSqlQuery listQuery;
    listQuery.prepare("SELECT IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, TO_CHAR(DATE_MAINT, 'DD/MM/YYYY') FROM VEHICULE ORDER BY IMMAT FETCH FIRST 20 ROWS ONLY");
    if (listQuery.exec()) {
        context += "\nListe des véhicules:\n";
        while (listQuery.next()) {
            context += QString("  • %1 - %2 %3 | Type: %4 | État: %5 | Service: %6 | Maintenance: %7\n")
                .arg(listQuery.value(0).toString())
                .arg(listQuery.value(1).toString())
                .arg(listQuery.value(2).toString())
                .arg(listQuery.value(3).toString())
                .arg(listQuery.value(4).toString())
                .arg(listQuery.value(5).toString())
                .arg(listQuery.value(6).toString());
        }
    }
    
    return context;
}

QString MainWindow::processChatMessageVehicule(const QString &msg)
{
    QString m = msg.toLower();

    // --- Questions sur les véhicules disponibles ---
    if (m.contains("combien") && (m.contains("vehicule") || m.contains("voiture"))) {
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM VEHICULE");
        if (query.exec() && query.next()) {
            int count = query.value(0).toInt();
            return QString("Il y a actuellement %1 véhicule(s) dans la base de données.").arg(count);
        }
        return "Impossible de récupérer le nombre de véhicules.";
    }

    // --- Questions sur l'état des véhicules ---
    if ((m.contains("combien") || m.contains("nombre")) && (m.contains("neuf") || m.contains("use") || m.contains("panne"))) {
        QString etat;
        if (m.contains("neuf")) etat = "Neuf";
        else if (m.contains("use")) etat = "Use";
        else if (m.contains("panne")) etat = "En panne";
        
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM VEHICULE WHERE UPPER(ETAT) = :etat");
        query.bindValue(":etat", etat.toUpper());
        if (query.exec() && query.next()) {
            int count = query.value(0).toInt();
            return QString("Il y a %1 véhicule(s) en état '%2'.").arg(count).arg(etat);
        }
    }

    // --- Liste des véhicules ---
    if (m.contains("liste") || m.contains("affiche") || (m.contains("tous") && m.contains("vehicule"))) {
        QSqlQuery query;
        query.prepare("SELECT IMMAT, MARQUE, MODELE FROM VEHICULE ORDER BY IMMAT");
        if (query.exec()) {
            QString result = "📋 Liste des véhicules :\n\n";
            int count = 0;
            while (query.next() && count < 10) {
                result += QString("• %1 - %2 %3\n")
                    .arg(query.value(0).toString())
                    .arg(query.value(1).toString())
                    .arg(query.value(2).toString());
                count++;
            }
            if (count == 0) result = "Aucun véhicule trouvé.";
            else if (count == 10) result += "\n(Affichage limité aux 10 premiers résultats)";
            return result;
        }
    }

    // --- Recherche par marque ---
    if (m.contains("marque")) {
        QStringList words = m.split(QRegularExpression("\\s+"));
        QString marque = "";
        for (int i = 0; i < words.size(); i++) {
            if (words[i] == "marque" && i + 1 < words.size()) {
                marque = words[i + 1];
                break;
            }
        }
        
        if (!marque.isEmpty()) {
            QSqlQuery query;
            query.prepare("SELECT IMMAT, MODELE, ETAT FROM VEHICULE WHERE UPPER(MARQUE) LIKE :marque");
            query.bindValue(":marque", "%" + marque.toUpper() + "%");
            if (query.exec()) {
                QString result = QString("🔍 Véhicules de marque '%1' :\n\n").arg(marque);
                int count = 0;
                while (query.next()) {
                    result += QString("• %1 - %2 (État: %3)\n")
                        .arg(query.value(0).toString())
                        .arg(query.value(1).toString())
                        .arg(query.value(2).toString());
                    count++;
                }
                if (count == 0) result = QString("Aucun véhicule de marque '%1' trouvé.").arg(marque);
                return result;
            }
        }
    }

    // --- Recherche par immatriculation ---
    if ((m.contains("immat") || m.contains("plaque")) && !m.contains("recherche")) {
        QStringList words = m.split(QRegularExpression("\\s+"));
        QString immat = "";
        for (int i = 0; i < words.size(); i++) {
            if ((words[i] == "immat" || words[i] == "immatriculation" || words[i] == "plaque") && i + 1 < words.size()) {
                immat = words[i + 1];
                break;
            }
        }
        
        if (!immat.isEmpty()) {
            QSqlQuery query;
            query.prepare("SELECT MARQUE, MODELE, TYPE, ETAT, SERVICE FROM VEHICULE WHERE UPPER(IMMAT) LIKE :immat");
            query.bindValue(":immat", "%" + immat.toUpper() + "%");
            if (query.exec() && query.next()) {
                return QString("🚗 Véhicule %1 :\n\n• Marque: %2\n• Modèle: %3\n• Type: %4\n• État: %5\n• Service: %6")
                    .arg(immat)
                    .arg(query.value(0).toString())
                    .arg(query.value(1).toString())
                    .arg(query.value(2).toString())
                    .arg(query.value(3).toString())
                    .arg(query.value(4).toString());
            } else {
                return QString("Véhicule '%1' non trouvé.").arg(immat);
            }
        }
    }

    // --- Véhicules en panne ---
    if (m.contains("panne") && !m.contains("combien")) {
        QSqlQuery query;
        query.prepare("SELECT IMMAT, MARQUE, MODELE FROM VEHICULE WHERE UPPER(ETAT) = 'EN PANNE'");
        if (query.exec()) {
            QString result = "⚠️ Véhicules en panne :\n\n";
            int count = 0;
            while (query.next()) {
                result += QString("• %1 - %2 %3\n")
                    .arg(query.value(0).toString())
                    .arg(query.value(1).toString())
                    .arg(query.value(2).toString());
                count++;
            }
            if (count == 0) result = "Aucun véhicule en panne ! 👍";
            return result;
        }
    }

    // --- Dernière maintenance ---
    if (m.contains("dernier") && m.contains("maintenance")) {
        QSqlQuery query;
        query.prepare("SELECT IMMAT, MARQUE, MODELE, TO_CHAR(DATE_MAINT, 'DD/MM/YYYY') FROM VEHICULE ORDER BY DATE_MAINT DESC FETCH FIRST 5 ROWS ONLY");
        if (query.exec()) {
            QString result = "🔧 Dernières maintenances :\n\n";
            while (query.next()) {
                result += QString("• %1 - %2 %3 (le %4)\n")
                    .arg(query.value(0).toString())
                    .arg(query.value(1).toString())
                    .arg(query.value(2).toString())
                    .arg(query.value(3).toString());
            }
            return result;
        }
    }

    // --- Aide sur l'ajout ---
    if (m.contains("ajouter") && (m.contains("vehicule") || m.contains("voiture")))
        return "Pour ajouter un véhicule : remplissez les champs (Immatriculation, Marque, Modèle, Type, État, Service, Date maintenance) puis cliquez sur 'Ajouter'.";

    // --- Aide sur la modification ---
    if (m.contains("modifier") || m.contains("update"))
        return "Pour modifier un véhicule : sélectionnez-le dans le tableau, modifiez les champs souhaités, puis cliquez sur 'Modifier'.";

    // --- Aide sur la suppression ---
    if (m.contains("supprimer") || m.contains("delete"))
        return "Pour supprimer un véhicule : sélectionnez-le dans le tableau puis cliquez sur 'Supprimer'. Une confirmation vous sera demandée.";

    // --- Aide sur la recherche ---
    if (m.contains("recherche") || m.contains("chercher") || m.contains("filtrer"))
        return "Tapez une immatriculation dans la barre de recherche '🔍 Rechercher par immatriculation...' pour filtrer les véhicules en temps réel.";

    // --- Aide sur l'état ---
    if (m.contains("etat"))
        return "Les états disponibles sont :\n• Neuf : véhicule en excellent état\n• Use : véhicule usagé mais fonctionnel\n• En panne : véhicule nécessitant une réparation";

    // --- Aide sur la date de maintenance ---
    if (m.contains("date") && m.contains("maintenance"))
        return "La date de maintenance indique la dernière intervention effectuée sur le véhicule. Format attendu : JJ/MM/AAAA. Vous pouvez trier les véhicules par date via le bouton '📅 Tri par date'.";

    // --- Aide sur le tri ---
    if (m.contains("tri") || m.contains("trier"))
        return "Le bouton '📅 Tri par date' permet de trier les véhicules par date de maintenance croissante ou décroissante. Cliquez plusieurs fois pour inverser l'ordre.";

    // --- Aide sur les statistiques ---
    if (m.contains("statistique") || m.contains("graphique"))
        return "Le bouton '📊 Statistiques' affiche un graphique en secteurs montrant la répartition des véhicules par état (Neuf, Use, En panne).";

    // --- Aide sur les recommandations IA ---
    if (m.contains("recommandation") || m.contains("ia") || m.contains("ai"))
        return "Sélectionnez un véhicule puis cliquez sur '🔧 Recommandation IA' pour obtenir des conseils de maintenance personnalisés basés sur l'intelligence artificielle Azure OpenAI.";

    // --- Aide générale ---
    if (m.contains("aide") || m.contains("help") || m.contains("?"))
        return "🤖 Je peux vous aider avec :\n\n• Combien de véhicules ?\n• Liste tous les véhicules\n• Véhicules en panne\n• Marque [nom]\n• Immat [code]\n• Dernière maintenance\n• Comment ajouter/modifier/supprimer ?\n• Statistiques\n• Recommandation IA";

    // --- Message par défaut ---
    return "Je n'ai pas compris 😅. Tapez 'aide' pour voir ce que je peux faire, ou essayez :\n• Combien de véhicules ?\n• Liste tous les véhicules\n• Véhicules en panne\n• Marque Renault";
}

// ========== RECOMMANDATIONS IA VÉHICULE ==========
void MainWindow::on_btnRecommandationVehicule_clicked()
{
    qDebug() << "🔎 [DEBUG] on_btnRecommandationVehicule_clicked() triggered";
    qDebug() << "🔎 [DEBUG] selectedImmatVehicule current value:" << selectedImmatVehicule;

    if (selectedImmatVehicule.isEmpty()) {
        qDebug() << "⚠️ [DEBUG] No vehicle selected before recommendation!";
        QMessageBox::warning(this,
                             "Erreur",
                             "Veuillez d'abord sélectionner un véhicule avant de demander une recommandation.");
        return;
    }

    // Construire le message pour l'IA à partir du véhicule courant
    QString prompt = buildMaintenancePromptFromCurrentVehicule();
    qDebug() << "📨 [DEBUG] Maintenance prompt envoyé à Azure :" << prompt;

    // Nettoyer la zone avant la réponse
    if (ui->textEditRecommandationVehicule) {
        ui->textEditRecommandationVehicule->clear();
        ui->textEditRecommandationVehicule->setPlainText("Génération de la recommandation en cours...");
    }

    // Lancer l'appel Azure
    sendRecommendationToAzureAI(prompt);

    // Afficher la page de recommandation
    if (ui->stackedWidget_3) {
        ui->stackedWidget_3->setCurrentWidget(ui->page_11);
    }
}

void MainWindow::on_btnBackFromRecommandationVehicule_clicked()
{
    if (ui->stackedWidget_3) {
        ui->stackedWidget_3->setCurrentWidget(ui->page_9);
    }
}

QString MainWindow::buildMaintenancePromptFromCurrentVehicule() const
{
    QString immat   = ui->immatline_2 ? ui->immatline_2->text() : "";
    QString marque  = ui->marqueline_2 ? ui->marqueline_2->text() : "";
    QString modele  = ui->modeleline_2 ? ui->modeleline_2->text() : "";
    QString type    = ui->triemail_2 ? ui->triemail_2->currentText() : "";
    QString etat    = ui->Etatline_2 ? ui->Etatline_2->currentText() : "";
    QString service = ui->serviceline_2 ? ui->serviceline_2->text() : "";
    
    QDate date_maint;
    if (ui->datemaintline_2) {
        date_maint = QDate::fromString(ui->datemaintline_2->text(), "dd/MM/yyyy");
    }

    // 🔥 Get today's REAL date from system
    QString today = QDate::currentDate().toString("dd/MM/yyyy");

    QString vehiculeInfo = QString(
                               "Nous sommes le %1.\n"
                               "Données véhicule :\n"
                               "Immatriculation : %2\n"
                               "Marque : %3\n"
                               "Modèle : %4\n"
                               "Type : %5\n"
                               "État : %6\n"
                               "Service : %7\n"
                               "Date de dernière maintenance : %8\n\n"
                               "En te basant uniquement sur ces données, propose une recommandation d'entretien "
                               "pratique et cohérente avec la date du jour. Réponds en français, texte simple, "
                               "sans markdown, en maximum 10 lignes."
                               ).arg(
                                   today,
                                   immat,
                                   marque,
                                   modele,
                                   type,
                                   etat,
                                   service,
                                   date_maint.isValid() ? date_maint.toString("dd/MM/yyyy") : "inconnue"
                                   );

    return vehiculeInfo;
}

void MainWindow::sendMessageToAzureAI(const QString &message)
{
    qDebug() << "🚀 sendMessageToAzureAI() démarre (Chatbot)";

    QString endpoint   = "https://ai-kassem.cognitiveservices.azure.com";
    QString apiKey     = "445NLYUwthBdj5EbFvxbCxV2XSdJWKYartumAOvqEFMtKEofmdpuJQQJ99BGACfhMk5XJ3w3AAAAACOGRpAA";
    QString apiVersion = "2024-12-01-preview";
    QString model      = "gpt-4o";

    QString url = endpoint
                  + "/openai/deployments/"
                  + model
                  + "/chat/completions?api-version="
                  + apiVersion;

    QNetworkRequest request{ QUrl(url) };
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("api-key", apiKey.toUtf8());

    QString systemPrompt = R"(Tu es SmartHelp, un assistant intelligent pour la gestion des véhicules SmartCity.
Tu as accès aux données complètes de la flotte de véhicules.

Instructions:
- Réponds toujours en français, de manière claire et concise
- Utilise les données fournies dans le contexte pour répondre avec précision
- Si on te demande des statistiques, analyse les données et donne des chiffres précis
- Si on te demande des conseils, base-toi sur l'état et l'historique des véhicules
- Formate tes réponses avec des emojis pertinents (🚗, 📊, ⚠️, ✅, etc.)
- Maximum 10 lignes par réponse
- Sois professionnel mais amical)";

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = systemPrompt;

    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = message;

    QJsonArray arr;
    arr.append(systemMsg);
    arr.append(userMsg);

    QJsonObject payload;
    payload["messages"]    = arr;
    payload["temperature"] = 0.7;

    QByteArray jsonData = QJsonDocument(payload).toJson();
    QNetworkReply *reply = networkManagerVehicule->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "❌ ERROR Chatbot:" << reply->errorString();
            if (ui->textEditChatOutputVehicule) {
                QString current = ui->textEditChatOutputVehicule->toPlainText();
                current = current.replace("🤖 SmartHelp IA: Analyse en cours...\n", "");
                current += "❌ Erreur de connexion à l'IA. Veuillez réessayer.\n";
                ui->textEditChatOutputVehicule->setPlainText(current);
            }
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(data);
        QString bot = json["choices"][0]["message"]["content"].toString();

        if (ui->textEditChatOutputVehicule) {
            QString current = ui->textEditChatOutputVehicule->toPlainText();
            // Supprimer le message temporaire "Analyse en cours..."
            current = current.replace("🤖 SmartHelp IA: Analyse en cours...\n", "");
            current += "🤖 SmartHelp IA: " + bot.trimmed() + "\n";
            ui->textEditChatOutputVehicule->setPlainText(current);
            
            // Auto-scroll vers le bas
            QTextCursor cursor = ui->textEditChatOutputVehicule->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->textEditChatOutputVehicule->setTextCursor(cursor);
        }

        reply->deleteLater();
    });
}

void MainWindow::sendRecommendationToAzureAI(const QString &message)
{
    qDebug() << "🚀 sendRecommendationToAzureAI() démarre";

    QString endpoint   = "https://ai-kassem.cognitiveservices.azure.com";
    QString apiKey     = "445NLYUwthBdj5EbFvxbCxV2XSdJWKYartumAOvqEFMtKEofmdpuJQQJ99BGACfhMk5XJ3w3AAAAACOGRpAA";
    QString apiVersion = "2024-12-01-preview";
    QString model      = "gpt-4o";

    QString url = endpoint
                  + "/openai/deployments/"
                  + model
                  + "/chat/completions?api-version="
                  + apiVersion;

    QNetworkRequest request{ QUrl(url) };
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("api-key", apiKey.toUtf8());

    QString systemPrompt = R"(Tu es SmartHelp, assistant SmartCity.
Tu donnes des recommandations de maintenance pour les véhicules.
Réponds toujours en français, en texte simple, sans markdown, en maximum 10 lignes.)";

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = systemPrompt;

    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = message;

    QJsonArray arr;
    arr.append(systemMsg);
    arr.append(userMsg);

    QJsonObject payload;
    payload["messages"]    = arr;
    payload["temperature"] = 0.4;

    QByteArray jsonData = QJsonDocument(payload).toJson();
    QNetworkReply *reply = networkManagerVehicule->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "❌ ERROR Recommandation:" << reply->errorString();
            if (ui->textEditRecommandationVehicule) {
                ui->textEditRecommandationVehicule->setPlainText("Erreur Azure : " + reply->errorString());
            }
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(data);
        QString bot = json["choices"][0]["message"]["content"].toString();

        if (ui->textEditRecommandationVehicule) {
            ui->textEditRecommandationVehicule->setPlainText(bot.trimmed());
        }

        reply->deleteLater();
    });
}

// ========== STATISTIQUES VÉHICULE ==========
QChartView* MainWindow::createVehiculePieChart()
{
    QSqlQuery query;
    QMap<QString, int> etatCount;

    if (!query.exec("SELECT ETAT, COUNT(*) FROM VEHICULE GROUP BY ETAT")) {
        qDebug() << "❌ Erreur statistiques:" << query.lastError().text();
        return nullptr;
    }

    while (query.next()) {
        QString etat = query.value(0).toString();
        int count = query.value(1).toInt();
        etatCount[etat] = count;
    }

    QPieSeries *series = new QPieSeries();
    for (auto it = etatCount.begin(); it != etatCount.end(); ++it) {
        series->append(it.key() + " (" + QString::number(it.value()) + ")", it.value());
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des véhicules par état");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

void MainWindow::on_btnStatistiquesVehicule_clicked()
{
    QChartView *chartView = createVehiculePieChart();
    if (!chartView) {
        QMessageBox::warning(this, "Erreur", "Impossible de générer les statistiques.");
        return;
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Statistiques Véhicules");
    dialog->resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    dialog->setLayout(layout);

    dialog->exec();
}

// ========== TRI PAR DATE ==========
void MainWindow::on_btnTriDateVehicule_clicked()
{
    if (!ui->tableau_3) {
        return;
    }

    triCroissantVehicule = !triCroissantVehicule;

    QSqlQuery query;
    QString order = triCroissantVehicule ? "ASC" : "DESC";
    query.prepare("SELECT IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT "
                  "FROM VEHICULE ORDER BY DATE_MAINT " + order);

    if (!query.exec()) {
        qDebug() << "❌ Erreur tri:" << query.lastError().text();
        return;
    }

    ui->tableau_3->setRowCount(0);
    int row = 0;
    while (query.next()) {
        ui->tableau_3->insertRow(row);
        for (int col = 0; col < 7; ++col) {
            QString value = query.value(col).toString();
            if (col == 6 && !value.isEmpty()) { // Date
                QDate date = query.value(col).toDate();
                value = date.toString("dd/MM/yyyy");
            }
            ui->tableau_3->setItem(row, col, new QTableWidgetItem(value));
        }
        row++;
    }

    QString btnText = triCroissantVehicule ? "Tri Date ↑" : "Tri Date ↓";
    if (ui->btnTriDateVehicule) {
        ui->btnTriDateVehicule->setText(btnText);
    }
}

// ========== RECHERCHE VÉHICULE ==========
void MainWindow::on_lineEditRechercheVehicule_textChanged(const QString &text)
{
    if (!ui->tableau_3) {
        return;
    }

    QString searchText = text.trimmed();
    if (searchText.isEmpty()) {
        chargerVehicules();
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT "
                  "FROM VEHICULE WHERE UPPER(IMMAT) LIKE :search "
                  "ORDER BY ID_VEHI DESC");
    query.bindValue(":search", "%" + searchText.toUpper() + "%");

    if (!query.exec()) {
        qDebug() << "❌ Erreur recherche:" << query.lastError().text();
        return;
    }

    ui->tableau_3->setRowCount(0);
    int row = 0;
    while (query.next()) {
        ui->tableau_3->insertRow(row);
        for (int col = 0; col < 7; ++col) {
            QString value = query.value(col).toString();
            if (col == 6 && !value.isEmpty()) {
                QDate date = query.value(col).toDate();
                value = date.toString("dd/MM/yyyy");
            }
            ui->tableau_3->setItem(row, col, new QTableWidgetItem(value));
        }
        row++;
    }
}

// ========================================
// FONCTIONS ARDUINO RFID
// ========================================

void MainWindow::onRFIDScanned(const QString &rfidCode)
{
    qDebug() << "🔍 RFID scanné:" << rfidCode;
    
    // Rechercher le résident avec ce code RFID
    QSqlQuery query;
    query.prepare(
        "SELECT r.ID, r.NOM, r.PRENOM, r.EMAIL, r.TELEPHONE, "
        "       m.ID, m.ADRESSE, m.TYPE "
        "FROM GEST_RESIDENT r "
        "LEFT JOIN GEST_MAISON m ON r.ID_MAISON = m.ID "
        "WHERE r.RFID_CODE = :rfid"
    );
    query.bindValue(":rfid", rfidCode);
    
    if (!query.exec()) {
        QMessageBox::warning(this, "Erreur", 
            "Erreur lors de la recherche du résident:\n" + query.lastError().text());
        return;
    }
    
    if (query.next()) {
        // Résident trouvé
        QString nom = query.value(1).toString();
        QString prenom = query.value(2).toString();
        QString email = query.value(3).toString();
        QString telephone = query.value(4).toString();
        
        QString maisonInfo;
        if (!query.value(5).isNull()) {
            QString adresse = query.value(6).toString();
            QString type = query.value(7).toString();
            
            maisonInfo = QString(
                "\n\n🏠 MAISON ASSIGNÉE:\n"
                "Adresse: %1\n"
                "Type: %2"
            ).arg(adresse, type);
        } else {
            maisonInfo = "\n\n⚠️ Aucune maison assignée";
        }
        
        QString message = QString(
            "✅ ACCÈS AUTORISÉ\n\n"
            "👤 RÉSIDENT:\n"
            "Nom: %1 %2\n"
            "Email: %3\n"
            "Téléphone: %4"
            "%5\n\n"
            "🚪 La porte s'ouvre..."
        ).arg(nom, prenom, email, telephone, maisonInfo);
        
        QMessageBox::information(this, "RFID - Accès Autorisé", message);
        
        // 🚪 OUVRIR LE SERVO MOTEUR
        arduinoRFID->ouvrirPorte();
        
    } else {
        // Résident non trouvé - log only, no dialog
        qDebug() << "❌ Accès refusé - RFID inconnu:";
    }
}

// Contrôle manuel du servo-moteur - Bouton Ouvrir
void MainWindow::on_btnOuvrirServo_clicked()
{
    if (!arduinoRFID->isConnected()) {
        QMessageBox::warning(this, "Servo", "Arduino non connecté !");
        return;
    }
    
    qDebug() << "🔧 Envoi commande manuelle: OPEN";
    QByteArray commande = "OPEN\n";
    arduinoRFID->getSerialPort()->write(commande);
    arduinoRFID->getSerialPort()->flush();
    QMessageBox::information(this, "Servo", "✅ Commande OPEN envoyée!\nLe servo ouvrira et fermera automatiquement.");
}

// Contrôle manuel du servo-moteur - Bouton Fermer
void MainWindow::on_btnFermerServo_clicked()
{
    if (!arduinoRFID->isConnected()) {
        QMessageBox::warning(this, "Servo", "Arduino non connecté !");
        return;
    }
    
    qDebug() << "🔧 Envoi commande manuelle: CLOSE";
    QByteArray commande = "CLOSE\n";
    arduinoRFID->getSerialPort()->write(commande);
    arduinoRFID->getSerialPort()->flush();
    QMessageBox::information(this, "Servo", "✅ Commande CLOSE envoyée!");
}

// Test automatique du servo-moteur
void MainWindow::on_btnTestServo_clicked()
{
    if (!arduinoRFID->isConnected()) {
        QMessageBox::warning(this, "Servo Test", "Arduino non connecté !");
        return;
    }
    
    qDebug() << "🔧 TEST AUTOMATIQUE DU SERVO";
    QByteArray commande = "TEST\n";
    arduinoRFID->getSerialPort()->write(commande);
    arduinoRFID->getSerialPort()->flush();
    QMessageBox::information(this, "Test Servo", "✅ Test lancé!\nLe servo va bouger pendant 2 secondes.");
}

// ============================================================
// GESTION DES CABINETS
// ============================================================

void MainWindow::chargerCabinets()
{
    if (!ui->tableau_12) {
        return;
    }

    QString error;
    const QList<Cabinet> cabinets = Cabinet::fetchAll(&error);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Cabinets"), tr("Impossible de charger les cabinets :\n%1").arg(error));
        return;
    }

    ui->tableau_12->clearContents();
    ui->tableau_12->setRowCount(cabinets.size());

    int row = 0;
    for (const Cabinet &c : cabinets) {
        ui->tableau_12->setItem(row, 0, new QTableWidgetItem(QString::number(c.id())));
        ui->tableau_12->setItem(row, 1, new QTableWidgetItem(c.nom()));
        ui->tableau_12->setItem(row, 2, new QTableWidgetItem(c.adresse()));
        ui->tableau_12->setItem(row, 3, new QTableWidgetItem(c.email()));
        ui->tableau_12->setItem(row, 4, new QTableWidgetItem(c.specialite()));
        ui->tableau_12->setItem(row, 5, new QTableWidgetItem(c.telephone()));
        ui->tableau_12->setItem(row, 6, new QTableWidgetItem(c.idResident() > 0 ? QString::number(c.idResident()) : QString()));
        ++row;
    }
}

void MainWindow::onAjouterCabinet()
{
    if (!ui->idcabiline || !ui->nomline_2 || !ui->prenomline_2 || !ui->specline || !ui->telephoneline_2 || !ui->emailline_2) {
        QMessageBox::warning(this, tr("Cabinets"), tr("Formulaire incomplet"));
        return;
    }

    bool ok = false;
    const int id = ui->idcabiline->text().trimmed().toInt(&ok);
    if (!ok || id <= 0) {
        QMessageBox::warning(this, tr("Cabinets"), tr("L'identifiant doit être un entier positif."));
        return;
    }

    if (Cabinet::idExists(id)) {
        QMessageBox::warning(this, tr("Cabinets"), tr("L'identifiant %1 existe déjà.").arg(id));
        return;
    }

    const QString nom = ui->nomline_2->text().trimmed();
    const QString adresse = ui->prenomline_2->text().trimmed();
    const QString specialite = ui->specline->text().trimmed();
    const QString telephone = ui->telephoneline_2->text().trimmed();
    const QString email = ui->emailline_2->text().trimmed();

    if (nom.isEmpty() || adresse.isEmpty() || specialite.isEmpty() || telephone.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, tr("Cabinets"), tr("Tous les champs sont obligatoires (sauf ID résident)."));
        return;
    }

    QRegularExpression emailRegex(QStringLiteral("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"));
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, tr("Cabinets"), tr("Adresse e-mail invalide."));
        return;
    }

    int idResident = -1;
    if (ui->medecinline) {
        const QString residentText = ui->medecinline->text().trimmed();
        if (!residentText.isEmpty()) {
            idResident = residentText.toInt(&ok);
            if (!ok) {
                QMessageBox::warning(this, tr("Cabinets"), tr("L'ID résident doit être numérique."));
                return;
            }
        }
    }

    Cabinet cabinet(id, nom, adresse, specialite, telephone, email, idResident);
    QString error;
    if (cabinet.ajouter(&error)) {
        chargerCabinets();
        reinitialiserFormulaireCabinet();
        QMessageBox::information(this, tr("Cabinets"), tr("Cabinet ajouté."));
    } else {
        QMessageBox::critical(this, tr("Cabinets"), tr("Échec de l'ajout :\n%1").arg(error));
    }
}

void MainWindow::onModifierCabinet()
{
    if (!ui->tableau_12 || ui->tableau_12->currentRow() < 0) {
        QMessageBox::warning(this, tr("Cabinets"), tr("Veuillez sélectionner un cabinet."));
        return;
    }

    const int oldId = ui->tableau_12->item(ui->tableau_12->currentRow(), 0)->text().toInt();

    bool ok = false;
    const int newId = ui->idcabiline->text().trimmed().toInt(&ok);
    if (!ok || newId <= 0) {
        QMessageBox::warning(this, tr("Cabinets"), tr("L'identifiant doit être un entier positif."));
        return;
    }

    const QString nom = ui->nomline_2->text().trimmed();
    const QString adresse = ui->prenomline_2->text().trimmed();
    const QString specialite = ui->specline->text().trimmed();
    const QString telephone = ui->telephoneline_2->text().trimmed();
    const QString email = ui->emailline_2->text().trimmed();

    if (nom.isEmpty() || adresse.isEmpty() || specialite.isEmpty() || telephone.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, tr("Cabinets"), tr("Tous les champs sont obligatoires (sauf ID résident)."));
        return;
    }

    int idResident = -1;
    if (ui->medecinline) {
        const QString residentText = ui->medecinline->text().trimmed();
        if (!residentText.isEmpty()) {
            idResident = residentText.toInt(&ok);
            if (!ok) {
                QMessageBox::warning(this, tr("Cabinets"), tr("L'ID résident doit être numérique."));
                return;
            }
        }
    }

    Cabinet cabinet(newId, nom, adresse, specialite, telephone, email, idResident);
    QString error;
    if (cabinet.modifier(oldId, &error)) {
        chargerCabinets();
        reinitialiserFormulaireCabinet();
        QMessageBox::information(this, tr("Cabinets"), tr("Cabinet modifié."));
    } else {
        QMessageBox::critical(this, tr("Cabinets"), tr("Échec de la modification :\n%1").arg(error));
    }
}

void MainWindow::onSupprimerCabinet()
{
    if (!ui->tableau_12 || ui->tableau_12->currentRow() < 0) {
        QMessageBox::warning(this, tr("Cabinets"), tr("Veuillez sélectionner un cabinet."));
        return;
    }

    const int id = ui->tableau_12->item(ui->tableau_12->currentRow(), 0)->text().toInt();
    const QString nom = ui->tableau_12->item(ui->tableau_12->currentRow(), 1)->text();

    if (QMessageBox::question(this, tr("Cabinets"), tr("Supprimer le cabinet %1 (%2) ?").arg(id).arg(nom)) != QMessageBox::Yes) {
        return;
    }

    QString error;
    if (Cabinet::supprimer(id, &error)) {
        chargerCabinets();
        reinitialiserFormulaireCabinet();
        QMessageBox::information(this, tr("Cabinets"), tr("Cabinet supprimé."));
    } else {
        QMessageBox::critical(this, tr("Cabinets"), tr("Échec de la suppression :\n%1").arg(error));
    }
}

void MainWindow::onExporterCabinetsPdf()
{
    if (!ui->tableau_12 || ui->tableau_12->rowCount() == 0) {
        QMessageBox::information(this, tr("Cabinets"), tr("Aucune donnée à exporter."));
        return;
    }

    QString chemin = QFileDialog::getSaveFileName(this,
                                                  tr("Exporter les cabinets"),
                                                  QDir::homePath() + "/cabinets.pdf",
                                                  tr("Documents PDF (*.pdf)"));
    if (chemin.isEmpty()) {
        return;
    }
    if (!chemin.endsWith(".pdf", Qt::CaseInsensitive)) {
        chemin += ".pdf";
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(chemin);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    QTextDocument document;
    document.setHtml(construireHtmlCabinets());
    document.print(&printer);

    QMessageBox::information(this, tr("Cabinets"), tr("Le fichier %1 a été généré.").arg(QFileInfo(chemin).fileName()));
}

void MainWindow::onCabinetSelectionChanged()
{
    if (!ui->tableau_12) {
        return;
    }

    const int row = ui->tableau_12->currentRow();
    if (row < 0) {
        return;
    }

    if (ui->idcabiline) ui->idcabiline->setText(ui->tableau_12->item(row, 0)->text());
    if (ui->nomline_2) ui->nomline_2->setText(ui->tableau_12->item(row, 1)->text());
    if (ui->prenomline_2) ui->prenomline_2->setText(ui->tableau_12->item(row, 2)->text());
    if (ui->emailline_2) ui->emailline_2->setText(ui->tableau_12->item(row, 3)->text());
    if (ui->specline) ui->specline->setText(ui->tableau_12->item(row, 4)->text());
    if (ui->telephoneline_2) ui->telephoneline_2->setText(ui->tableau_12->item(row, 5)->text());
    if (ui->medecinline) ui->medecinline->setText(ui->tableau_12->item(row, 6)->text());
}

void MainWindow::onOuvrirCabinetAvance()
{
    CabinetDialog dialog(this);
    dialog.exec();
}

void MainWindow::reinitialiserFormulaireCabinet()
{
    if (ui->idcabiline) ui->idcabiline->clear();
    if (ui->nomline_2) ui->nomline_2->clear();
    if (ui->prenomline_2) ui->prenomline_2->clear();
    if (ui->specline) ui->specline->clear();
    if (ui->telephoneline_2) ui->telephoneline_2->clear();
    if (ui->emailline_2) ui->emailline_2->clear();
    if (ui->medecinline) ui->medecinline->clear();
}

QString MainWindow::construireHtmlCabinets() const
{
    QString error;
    const QList<Cabinet> cabinets = Cabinet::fetchAll(&error);
    QString html = QStringLiteral("<h2>Liste des cabinets</h2>");
    if (!error.isEmpty()) {
        html += QStringLiteral("<p>%1</p>").arg(error.toHtmlEscaped());
        return html;
    }

    html += QStringLiteral("<table border='1' cellspacing='0' cellpadding='4'>");
    html += QStringLiteral("<tr><th>ID</th><th>Nom</th><th>Adresse</th><th>Email</th><th>Spécialité</th><th>Téléphone</th><th>ID résident</th></tr>");
    for (const Cabinet &c : cabinets) {
        html += QStringLiteral("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td></tr>")
                    .arg(c.id())
                    .arg(c.nom().toHtmlEscaped())
                    .arg(c.adresse().toHtmlEscaped())
                    .arg(c.email().toHtmlEscaped())
                    .arg(c.specialite().toHtmlEscaped())
                    .arg(c.telephone().toHtmlEscaped())
                    .arg(c.idResident() > 0 ? QString::number(c.idResident()) : QString());
    }
    html += QStringLiteral("</table>");
    return html;
}

// ============================================================
// MAINTENANCE ET RECOMMANDATIONS JARDINS
// ============================================================

void MainWindow::onOuvrirMaintenanceDialog()
{
    MaintenanceDialog dialog(this);
    dialog.exec();
}

void MainWindow::onOuvrirRecommandationDialog()
{
    RecommandationDialog dialog(this);
    dialog.exec();
}

// ============================================================
// CAPTEUR TEMPÉRATURE DHT11
// ============================================================

void MainWindow::onTemperatureRecue(float temperature, float humidite)
{
    qDebug() << QString("🌡️ Température: %1°C | 💧 Humidité: %2%")
                .arg(temperature, 0, 'f', 1)
                .arg(humidite, 0, 'f', 1);
    
    // Afficher dans l'interface graphique
    ui->labelTemperature->setText(QString("🌡️ Température: %1°C").arg(temperature, 0, 'f', 1));
    ui->labelHumidite->setText(QString("💧 Humidité: %1%").arg(humidite, 0, 'f', 1));
}

QStringList MainWindow::obtenirJardinsAArroser(float temperatureAmbiante)
{
    QStringList jardinsAArroser;
    
    QString error;
    QList<Jardin> jardins = Jardin::fetchAll(&error);
    
    if (!error.isEmpty()) {
        qWarning() << "❌ Erreur lors de la récupération des jardins:" << error;
        return jardinsAArroser;
    }
    
    qDebug() << "🔍 Vérification jardins - Temp ambiante:" << temperatureAmbiante << "°C";
    qDebug() << "🌿 Nombre de jardins en BD:" << jardins.size();
    
    // Comparer la température ambiante avec chaque jardin
    for (const Jardin &j : jardins) {
        qDebug() << QString("  Jardin #%1: %2 - Sol: %3°C vs Ambiant: %4°C")
                    .arg(j.id())
                    .arg(j.emplacement())
                    .arg(j.temperatureMoyenneSol(), 0, 'f', 1)
                    .arg(temperatureAmbiante, 0, 'f', 1);
        
        // Si la température ambiante est SUPÉRIEURE à la température du sol du jardin
        // cela signifie que le jardin est plus froid et nécessite un arrosage
        if (temperatureAmbiante > j.temperatureMoyenneSol()) {
            QString info = QString("Jardin #%1 (%2) - Sol: %3°C < Ambiant: %4°C")
                          .arg(j.id())
                          .arg(j.emplacement())
                          .arg(j.temperatureMoyenneSol(), 0, 'f', 1)
                          .arg(temperatureAmbiante, 0, 'f', 1);
            jardinsAArroser.append(info);
            qDebug() << "    ✅ NÉCESSITE ARROSAGE";
        } else {
            qDebug() << "    ❌ Pas besoin d'arrosage";
        }
    }
    
    return jardinsAArroser;
}

void MainWindow::onAlerteArrosage(const QStringList &jardinsAArroser)
{
    if (jardinsAArroser.isEmpty()) {
        return;
    }
    
    // Activer automatiquement la pompe (s'arrête après 3s automatiquement)
    if (m_temperatureSensor && m_temperatureSensor->isConnected()) {
        m_temperatureSensor->activerPompe();
        qDebug() << "💦 POMPE ACTIVÉE AUTOMATIQUEMENT pour arroser" << jardinsAArroser.size() << "jardin(s)";
    }
    
    // Construire le message d'alerte
    QString message = QString("💦 <b>ALERTE ARROSAGE</b><br><br>"
                             "La température ambiante est élevée.<br>"
                             "%1 jardin(s) nécessite(nt) un arrosage :<br><br>")
                     .arg(jardinsAArroser.size());
    
    for (const QString &jardin : jardinsAArroser) {
        message += "• " + jardin + "<br>";
    }
    
    message += "<br><b>🚿 Pompe d'eau activée pendant 3 secondes !</b>";
    
    // Afficher dans une boîte de dialogue
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Alerte Arrosage Jardins");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    
    // Log dans la console
    qDebug() << "========================================";
    qDebug() << "💦 ALERTE ARROSAGE -" << jardinsAArroser.size() << "jardin(s)";
    qDebug() << "========================================";
    for (const QString &jardin : jardinsAArroser) {
        qDebug() << "  •" << jardin;
    }
    qDebug() << "========================================";
}

void MainWindow::onConnecterCapteurTemperature()
{
    // Si déjà connecté, proposer de déconnecter
    if (m_temperatureSensor->isConnected()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, 
            "Capteur Température", 
            "Le capteur est déjà connecté. Voulez-vous le déconnecter ?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            m_temperatureSensor->deconnecter();
            ui->btnConnecterCapteur->setText("🔌 Connecter Capteur");
            ui->btnConnecterCapteur->setStyleSheet(
                "QPushButton { background-color: #17a2b8; color: white; border-radius: 5px; font: bold 10pt \"Arial\"; }"
                "QPushButton:hover { background-color: #138496; }"
            );
            // Désactiver le bouton de lecture
            if (ui->btnLireTemperature) {
                ui->btnLireTemperature->setEnabled(false);
                ui->btnLireTemperature->setText("▶️ Lire Température");
            }
            QMessageBox::information(this, "Capteur Température", "Capteur DHT11 déconnecté");
        }
        return;
    }
    
    // Lister les ports disponibles
    QStringList ports = ArduinoRFID::getAvailablePorts();
    if (ports.isEmpty()) {
        QMessageBox::warning(this, "Capteur Température", "Aucun port série disponible !");
        return;
    }
    
    // Demander à l'utilisateur de choisir un port
    bool ok;
    QString port = QInputDialog::getItem(this, "Capteur Température", 
                                        "Sélectionnez le port du capteur DHT11:",
                                        ports, 0, false, &ok);
    if (ok && !port.isEmpty()) {
        if (m_temperatureSensor->connecter(port)) {
            ui->btnConnecterCapteur->setText("✅ Capteur Connecté");
            ui->btnConnecterCapteur->setStyleSheet(
                "QPushButton { background-color: #28a745; color: white; border-radius: 5px; font: bold 10pt \"Arial\"; }"
                "QPushButton:hover { background-color: #218838; }"
            );
            // Activer le bouton de lecture
            if (ui->btnLireTemperature) {
                ui->btnLireTemperature->setEnabled(true);
            }
            QMessageBox::information(this, "Capteur Température", 
                                   QString("✅ Capteur DHT11 connecté sur %1\n\n"
                                           "Cliquez sur '▶️ Lire Température' pour démarrer la lecture\n"
                                           "et la détection automatique des jardins à arroser.").arg(port));
        } else {
            QMessageBox::warning(this, "Capteur Température", 
                               QString("❌ Échec de connexion sur %1\n\n"
                                       "Vérifiez que:\n"
                                       "• L'Arduino est bien branché\n"
                                       "• Le sketch DHT11 est téléversé\n"
                                       "• Le port série est correct").arg(port));
        }
    }
}

void MainWindow::onLireTemperatureClicked()
{
    if (!m_temperatureSensor->isConnected()) {
        QMessageBox::warning(this, "Capteur Température", 
                           "❌ Capteur non connecté !\n\nConnectez d'abord le capteur DHT11.");
        return;
    }
    
    if (m_temperatureSensor->isReading()) {
        // Arrêter la lecture
        m_temperatureSensor->arreterLecture();
        qDebug() << "⏹️ Lecture température arrêtée par l'utilisateur";
    } else {
        // Démarrer la lecture
        m_temperatureSensor->demarrerLecture();
        qDebug() << "▶️ Lecture température démarrée par l'utilisateur";
    }
}

void MainWindow::onDeconnecterCapteurTemperature()
{
    if (m_temperatureSensor->isConnected()) {
        m_temperatureSensor->deconnecter();
        // Désactiver le bouton de lecture
        if (ui->btnLireTemperature) {
            ui->btnLireTemperature->setEnabled(false);
        }
        QMessageBox::information(this, "Capteur Température", "Capteur DHT11 déconnecté");
    } else {
        QMessageBox::warning(this, "Capteur Température", "Capteur non connecté !");
    }
}

// ================================ CAMÉRA OV7670 ================================

void MainWindow::on_btnDemarrerCamera_clicked()
{
    // Déconnecter temporairement le LCD/RFID pour libérer le port
    QString portUtilise;
    if (arduinoRFID && arduinoRFID->getSerialPort() && arduinoRFID->getSerialPort()->isOpen()) {
        portUtilise = arduinoRFID->getSerialPort()->portName();
        qDebug() << "📷 Déconnexion temporaire du LCD/RFID sur" << portUtilise;
        arduinoRFID->getSerialPort()->close();
    }
    
    // Chercher le port de la caméra
    QString cameraPort;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    
    // Utiliser le même port que le LCD (COM9)
    if (!portUtilise.isEmpty()) {
        cameraPort = portUtilise;
    } else {
        // Ou chercher un port Arduino disponible
        for (const QSerialPortInfo &info : ports) {
            if (info.description().contains("Arduino", Qt::CaseInsensitive) ||
                info.manufacturer().contains("Arduino", Qt::CaseInsensitive)) {
                cameraPort = info.portName();
                break;
            }
        }
    }
    
    if (cameraPort.isEmpty()) {
        QMessageBox::warning(this, "Erreur", 
            "❌ Aucun Arduino détecté!\n\n"
            "Vérifiez que l'Arduino avec la caméra est branché.");
        return;
    }
    
    // Lancer SerialPortReader.exe en arrière-plan pour capturer les images
    // Chercher le fichier dans plusieurs emplacements possibles
    QString readerPath;
    QStringList possiblePaths;
    possiblePaths << QCoreApplication::applicationDirPath() + "/SerialPortReader.exe"
                  << QDir::currentPath() + "/SerialPortReader.exe"
                  << "C:/Users/walae/Desktop/qt_smartcity/integration/SerialPortReader.exe"
                  << "../SerialPortReader.exe"
                  << "SerialPortReader.exe";
    
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            readerPath = path;
            break;
        }
    }
    
    if (readerPath.isEmpty()) {
        QMessageBox::critical(this, "Erreur",
            QString("❌ SerialPortReader.exe introuvable!\n\n"
                   "Chemins recherchés:\n"
                   "• %1\n"
                   "• %2\n\n"
                   "Veuillez placer SerialPortReader.exe dans le dossier de l'application.")
                   .arg(QCoreApplication::applicationDirPath())
                   .arg(QDir::currentPath()));
        
        // Reconnecter le LCD/RFID
        if (!portUtilise.isEmpty() && arduinoRFID) {
            arduinoRFID->getSerialPort()->open(QIODevice::ReadWrite);
        }
        return;
    }
    
    qDebug() << "📷 SerialPortReader trouvé:" << readerPath;
    
    // Créer le processus SerialPortReader
    if (!cameraSerial) {
        cameraSerial = new QSerialPort(this);  // Utilisé comme marqueur pour savoir si camera active
    }
    
    // Lancer SerialPortReader.exe avec les paramètres: port baudrate outputFolder
    QProcess *readerProcess = new QProcess(this);
    QString outputFolder = QDir::temp().filePath("camera_captures");
    QDir().mkpath(outputFolder);  // Créer le dossier de sortie
    
    QStringList arguments;
    arguments << cameraPort << "2000000" << outputFolder;
    
    readerProcess->start(readerPath, arguments);
    
    if (!readerProcess->waitForStarted(3000)) {
        QMessageBox::critical(this, "Erreur",
            QString("❌ Impossible de démarrer SerialPortReader.exe\n\n"
                   "Erreur: %1").arg(readerProcess->errorString()));
        delete readerProcess;
        
        // Reconnecter le LCD/RFID
        if (!portUtilise.isEmpty() && arduinoRFID) {
            arduinoRFID->getSerialPort()->open(QIODevice::ReadWrite);
        }
        return;
    }
    
    // Sauvegarder le processus pour pouvoir l'arrêter plus tard
    cameraSerial->setProperty("readerProcess", QVariant::fromValue((void*)readerProcess));
    cameraSerial->setProperty("outputFolder", outputFolder);
    cameraSerial->setProperty("portUtilise", portUtilise);
    
    // Créer un timer pour vérifier les nouvelles images capturées
    QTimer *imageCheckTimer = new QTimer(this);
    connect(imageCheckTimer, &QTimer::timeout, this, [this, outputFolder]() {
        // Chercher la dernière image capturée
        QDir captureDir(outputFolder);
        QStringList filters;
        filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";
        QFileInfoList images = captureDir.entryInfoList(filters, QDir::Files, QDir::Time);
        
        if (!images.isEmpty()) {
            // Charger et afficher la dernière image
            QString latestImage = images.first().absoluteFilePath();
            QImage image(latestImage);
            
            if (!image.isNull()) {
                lastCapturedImage = image;
                
                // Afficher dans le label
                QPixmap pixmap = QPixmap::fromImage(image.scaled(
                    ui->labelCameraPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                ui->labelCameraPreview->setPixmap(pixmap);
            }
        }
    });
    imageCheckTimer->start(500);  // Vérifier toutes les 500ms
    cameraSerial->setProperty("imageCheckTimer", QVariant::fromValue((void*)imageCheckTimer));
    
    // Activer le mode streaming
    isCameraStreaming = true;
    
    // Mettre à jour l'interface
    ui->btnDemarrerCamera->setEnabled(false);
    ui->btnCapturerOCR->setEnabled(true);
    ui->btnArreterCamera->setEnabled(true);
    ui->labelCameraPreview->setText("📷 Flux Vidéo Actif\n(Chargement...)");
    ui->labelCameraPreview->setStyleSheet("background-color: #000; border: 2px solid #28a745; color: #28a745; font: bold 11pt \"Arial\";");
    
    qDebug() << "📷 SerialPortReader démarré pour" << cameraPort;
    qDebug() << "📁 Images sauvegardées dans:" << outputFolder;
    QMessageBox::information(this, "Caméra", 
        QString("✅ Caméra démarrée sur %1\n\n"
                "Les images sont capturées en temps réel.\n"
                "Cliquez sur 'Capturer' pour analyser une image.").arg(cameraPort));
}

void MainWindow::on_btnCapturerOCR_clicked()
{
    if (!isCameraStreaming || !cameraSerial) {
        QMessageBox::warning(this, "Erreur", "La caméra n'est pas démarrée!");
        return;
    }
    
    // Utiliser la dernière image reçue du flux
    if (lastCapturedImage.isNull()) {
        QMessageBox::warning(this, "Erreur", "Aucune image disponible! Attendez quelques secondes...");
        return;
    }
    
    // Sauvegarder l'image capturée
    QString tempImagePath = QDir::temp().filePath("camera_manual_capture.jpg");
    if (lastCapturedImage.save(tempImagePath, "JPEG", 90)) {
        qDebug() << "📸 Capture manuelle sauvegardée:" << tempImagePath;
        
        // Afficher un aperçu de ce qui va être analysé
        ui->labelCameraPreview->setPixmap(QPixmap::fromImage(lastCapturedImage.scaled(
            ui->labelCameraPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        
        // Lancer l'OCR avec Tesseract
        analyserImageOCR(tempImagePath);
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de sauvegarder l'image capturée!");
    }
}

void MainWindow::on_btnArreterCamera_clicked()
{
    // Arrêter le processus SerialPortReader.exe
    if (cameraSerial) {
        QProcess *readerProcess = (QProcess*)cameraSerial->property("readerProcess").value<void*>();
        if (readerProcess) {
            readerProcess->terminate();
            if (!readerProcess->waitForFinished(3000)) {
                readerProcess->kill();  // Forcer l'arrêt si nécessaire
            }
            delete readerProcess;
        }
        
        // Arrêter le timer de vérification des images
        QTimer *imageCheckTimer = (QTimer*)cameraSerial->property("imageCheckTimer").value<void*>();
        if (imageCheckTimer) {
            imageCheckTimer->stop();
            delete imageCheckTimer;
        }
        
        // Récupérer le port utilisé pour reconnecter le LCD
        QString portUtilise = cameraSerial->property("portUtilise").toString();
        
        delete cameraSerial;
        cameraSerial = nullptr;
        
        // Reconnecter le LCD/RFID
        if (!portUtilise.isEmpty() && arduinoRFID && arduinoRFID->getSerialPort()) {
            arduinoRFID->getSerialPort()->open(QIODevice::ReadWrite);
            qDebug() << "📟 LCD/RFID reconnecté sur" << portUtilise;
        }
    }
    
    isCameraStreaming = false;
    
    // Mettre à jour l'interface
    ui->btnDemarrerCamera->setEnabled(true);
    ui->btnCapturerOCR->setEnabled(false);
    ui->btnArreterCamera->setEnabled(false);
    ui->labelCameraPreview->setText("📷 Aperçu Caméra\n(En attente...)");
    ui->labelCameraPreview->setStyleSheet("background-color: #000; border: 2px solid #28a745; color: #fff; font: 10pt \"Arial\";");
    ui->labelCameraPreview->clear();
    
    qDebug() << "⏹️ Caméra arrêtée";
    QMessageBox::information(this, "Caméra", "Caméra arrêtée.\nLCD/RFID reconnecté.");
}

// Fonction onCameraDataReceived() n'est plus utilisée avec SerialPortReader.exe
void MainWindow::onCameraDataReceived()
{
    // Cette fonction n'est plus utilisée car SerialPortReader.exe gère la réception
    // des données du port série et sauvegarde automatiquement les images
}

// ============================================================
// VIDÉO JARDIN
// ============================================================

void MainWindow::onOuvrirVideoDialogJardin()
{
    VideoDialog dialog(this);
    dialog.exec();
}

// ============================================================
// ARDUINO LCD + KEYPAD
// ============================================================

void MainWindow::onConnecterLCD()
{
    // Essayer d'abord COM10 (port confirmé par test Python)
    qDebug() << "Tentative connexion LCD sur COM10...";
    if (arduinoLCD->connecter("COM10")) {
        qDebug() << "✅ Arduino LCD connecté sur COM10";
        qDebug() << "🎹 Vous pouvez saisir des matricules avec le clavier 4x4";
        return;
    }
    
    // Essayer COM4
    qDebug() << "Tentative connexion LCD sur COM4...";
    if (arduinoLCD->connecter("COM4")) {
        qDebug() << "✅ Arduino LCD connecté sur COM4";
        qDebug() << "🎹 Vous pouvez saisir des matricules avec le clavier 4x4";
        return;
    }
    
    // Si échec, essayer les autres ports
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    
    if (ports.isEmpty()) {
        qWarning() << "⚠️ Aucun port série disponible pour Arduino LCD";
        return;
    }
    
    // Essayer chaque port
    for (const QSerialPortInfo &portInfo : ports) {
        QString portName = portInfo.portName();
        if (portName == "COM4" || portName == "COM10") continue;  // Déjà essayé
        
        qDebug() << "Tentative connexion LCD sur" << portName;
        
        if (arduinoLCD->connecter(portName)) {
            qDebug() << "✅ Arduino LCD connecté automatiquement sur" << portName;
            qDebug() << "🎹 Vous pouvez saisir des matricules avec le clavier 4x4";
            return;
        }
    }
    
    qWarning() << "⚠️ Arduino LCD non trouvé - Vérifiez la connexion";
}

void MainWindow::onDeconnecterLCD()
{
    arduinoLCD->deconnecter();
    qDebug() << "Arduino LCD déconnecté";
}

void MainWindow::onToucheKeypadPressee(const QString &touche)
{
    qDebug() << "🎹 onToucheKeypadPressee appelé avec touche:" << touche;
    
    // * = Effacer le champ de saisie
    if (touche == "*") {
        if (ui->lineEditMatriculeLCD) {
            ui->lineEditMatriculeLCD->clear();
            qDebug() << "🗑️ Champ matricule effacé";
        }
        return;
    }
    
    // # = Soumettre (géré par onDonneesKeypadEnvoyees)
    if (touche == "#") {
        qDebug() << "⏭️ Touche # ignorée (soumission gérée par ENVOI)";
        return;
    }
    
    // Ajouter la touche au champ de saisie
    if (ui->lineEditMatriculeLCD) {
        QString texteActuel = ui->lineEditMatriculeLCD->text();
        ui->lineEditMatriculeLCD->setText(texteActuel + touche);
        
        qDebug() << "🔢 Touche saisie:" << touche << "→ Matricule:" << ui->lineEditMatriculeLCD->text();
    }
}

void MainWindow::onDonneesKeypadEnvoyees(const QString &donnees)
{
    qDebug() << "🚀 onDonneesKeypadEnvoyees appelé avec données:" << donnees;
    
    // Lorsque # est pressé, mettre la matricule dans le champ et lancer la recherche
    if (ui->lineEditMatriculeLCD) {
        ui->lineEditMatriculeLCD->setText(donnees);
    }
    
    // Recherche dédiée pour le clavier Arduino (sans utiliser le bouton)
    rechercherMatriculeKeypad(donnees);
}

void MainWindow::rechercherMatriculeKeypad(const QString &matricule)
{
    qDebug() << "🎹 Recherche clavier pour matricule:" << matricule;
    
    QString immatRecherche = matricule.trimmed().toUpper();
    
    if (immatRecherche.isEmpty()) {
        qDebug() << "⚠️ Matricule vide, recherche ignorée";
        return;
    }
    
    // Rechercher le véhicule dans la base de données
    QSqlQuery query;
    query.prepare("SELECT IMMATRICULATION, MARQUE, MODELE, TYPE FROM GEST_VEHICULE WHERE UPPER(IMMATRICULATION) = :immat");
    query.bindValue(":immat", immatRecherche);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur SQL:" << query.lastError().text();
        
        // Afficher sur LCD l'erreur
        if (arduinoLCD && arduinoLCD->estConnecte()) {
            arduinoLCD->afficherTexte("Erreur DB", "Reessayez");
        }
        return;
    }
    
    if (query.next()) {
        // Véhicule trouvé
        QString immat = query.value(0).toString();
        QString marque = query.value(1).toString();
        QString modele = query.value(2).toString();
        QString type = query.value(3).toString();
        
        qDebug() << "✅ Véhicule trouvé:" << immat << marque << modele;
        
        // Afficher sur LCD
        if (arduinoLCD && arduinoLCD->estConnecte()) {
            QString ligne1 = immat.left(16);
            QString ligne2 = QString("%1 %2").arg(marque.left(8), modele.left(7));
            arduinoLCD->afficherTexte(ligne1, ligne2);
        }
        
        // Message dans la console
        qDebug() << "📟 Envoyé au LCD:" << immat << "|" << marque << modele;
        
    } else {
        qDebug() << "⚠️ Véhicule non trouvé:" << immatRecherche;
        
        // Afficher sur LCD que le véhicule n'existe pas
        if (arduinoLCD && arduinoLCD->estConnecte()) {
            arduinoLCD->afficherTexte("Non trouve", immatRecherche.left(16));
        }
    }
}

// ================================ ANCIENNE FONCTION OCR ================================






