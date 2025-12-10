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
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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
        
        // Connexion automatique de l'Arduino au démarrage
        qDebug() << "";
        qDebug() << "🔄 Connexion automatique de l'Arduino...";
        if (arduinoRFID->connectArduino()) {
            qDebug() << "✅ Arduino connecté automatiquement au démarrage";
        } else {
            qWarning() << "⚠️ Échec de la connexion automatique de l'Arduino";
            qWarning() << "💡 Vous pouvez réessayer manuellement depuis 'Gestion Maisons'";
        }
    }
    qDebug() << "========================================";
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
}

void MainWindow::onGestionCabinets()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCabinets);
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
                              tr("Impossible d'ajouter le résident. Vérifiez la base de données."));
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
                              tr("La mise à jour a échoué."));
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
                              tr("La suppression a échoué."));
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
        ui->tableau_3->setItem(row, 6, new QTableWidgetItem(query.value("DATE_MAINTENANCE").toString()));
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
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout du véhicule !");
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
    
    if (v.modifier(oldImmat)) {
        QMessageBox::information(this, "Succès", "Véhicule modifié avec succès !");
        reinitialiserFormulaireVehicule();
        chargerVehicules();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification du véhicule !");
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
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du véhicule !");
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
    if (!arduinoRFID || !arduinoRFID->isConnected()) {
        QMessageBox::warning(this, "Arduino", "Arduino non connecté !");
        return;
    }
    
    // Format: LCD|ligne1|ligne2
    // Ligne 1: Matricule
    // Ligne 2: Marque Modele
    QString ligne1 = QString("Mat:%1").arg(immat);
    QString ligne2 = QString("%1 %2").arg(marque, modele);
    
    // Limiter à 16 caractères par ligne pour LCD 16x2
    if (ligne1.length() > 16) ligne1 = ligne1.left(16);
    if (ligne2.length() > 16) ligne2 = ligne2.left(16);
    
    QString commande = QString("LCD|%1|%2\n").arg(ligne1, ligne2);
    
    qDebug() << "📟 Envoi au LCD:" << commande;
    arduinoRFID->getSerialPort()->write(commande.toUtf8());
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
    
    qDebug() << "🔧 Envoi commande manuelle: 1 (OUVRIR)";
    QByteArray commande = "1\n";
    arduinoRFID->getSerialPort()->write(commande);
    arduinoRFID->getSerialPort()->flush();
}

// Contrôle manuel du servo-moteur - Bouton Fermer
void MainWindow::on_btnFermerServo_clicked()
{
    if (!arduinoRFID->isConnected()) {
        QMessageBox::warning(this, "Servo", "Arduino non connecté !");
        return;
    }
    
    qDebug() << "🔧 Envoi commande manuelle: 0 (FERMER)";
    QByteArray commande = "0\n";
    arduinoRFID->getSerialPort()->write(commande);
    arduinoRFID->getSerialPort()->flush();
}

// Test automatique du servo-moteur
void MainWindow::on_btnTestServo_clicked()
{
    if (!arduinoRFID->isConnected()) {
        QMessageBox::warning(this, "Servo Test", "Arduino non connecté !");
        return;
    }
    
    qDebug() << "🔧 TEST AUTOMATIQUE DU SERVO";
    arduinoRFID->ouvrirPorte();
}


