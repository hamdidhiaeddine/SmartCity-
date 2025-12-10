#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "maintenance.h"
#include "recommandationplante.h"
#include "maintenancedialog.h"
#include "recommandationdialog.h"
#include "videodialog.h"

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
#include <QLocale>

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
    if (ui->tableau_8) {
        ui->tableau_8->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableau_8->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableau_8->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableau_8->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    if (ui->tableau_12) {
        ui->tableau_12->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableau_12->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableau_12->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableau_12->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    connectButtons();
    m_utilisateurActuel = "Responsable";
    
    // ⚠️ NE PAS rafraîchir avant que la connexion DB soit établie
    // rafraichirResidents() sera appelé après la connexion dans Connection::createConnection()
    
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
}

MainWindow::~MainWindow()
{
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
    if (ui->btnJardinStats) {
        connect(ui->btnJardinStats, &QPushButton::clicked, this, &MainWindow::onAfficherJardinStats);
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
    if (ui->tableau_8) {
        connect(ui->tableau_8, &QTableWidget::itemSelectionChanged, this, &MainWindow::onJardinSelectionChanged);
        // Connecter aussi pour charger les données de maintenance et recommandations
        connect(ui->tableau_8, &QTableWidget::itemSelectionChanged, this, &MainWindow::onJardinSelectionChangedForMaintenance);
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
    if (ui->btnCabinetAvance) {
        connect(ui->btnCabinetAvance, &QPushButton::clicked, this, &MainWindow::onOuvrirCabinetAvance);
    }
    if (ui->tableau_12) {
        connect(ui->tableau_12, &QTableWidget::itemSelectionChanged, this, &MainWindow::onCabinetSelectionChanged);
    }
    
    // Boutons pour les dialogues Jardin
    if (ui->maintenance) {
        connect(ui->maintenance, &QPushButton::clicked, this, &MainWindow::onOuvrirMaintenanceDialog);
    }
    if (ui->Recommendation) {
        connect(ui->Recommendation, &QPushButton::clicked, this, &MainWindow::onOuvrirRecommandationDialog);
    }
    
    // Bouton Video pour Cabinet
    if (ui->Video) {
        connect(ui->Video, &QPushButton::clicked, this, &MainWindow::onOuvrirVideoDialog);
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
    
    // Style pour texte noir dans les champs de saisie Jardin
    QString inputStyle = "QLineEdit, QComboBox { color: black; background-color: white; }";
    if (ui->idjardinline) ui->idjardinline->setStyleSheet(inputStyle);
    if (ui->emplacementline) ui->emplacementline->setStyleSheet(inputStyle);
    if (ui->superficieline) ui->superficieline->setStyleSheet(inputStyle);
    if (ui->superficieline_2) ui->superficieline_2->setStyleSheet(inputStyle);
    
    // Configurer le combo avec les types de jardin valides pour la BD
    if (ui->typechoix) {
        ui->typechoix->setStyleSheet(inputStyle);
        ui->typechoix->clear();
        ui->typechoix->addItems({"Public", "Privé", "Communautaire", "Botanique", "Potager"});
    }
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
    QVector<Employee> employes = Employee::fetchAll(errorText);
    
    if (!errorText.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Impossible de charger les employés:\n" + errorText);
        return;
    }
    
    ui->tableau->clearContents();
    ui->tableau->setRowCount(employes.size());
    
    for (int i = 0; i < employes.size(); ++i) {
        const Employee &emp = employes[i];
        ui->tableau->setItem(i, 0, new QTableWidgetItem(QString::number(emp.idEmploye)));
        ui->tableau->setItem(i, 1, new QTableWidgetItem(emp.nom));
        ui->tableau->setItem(i, 2, new QTableWidgetItem(emp.prenom));
        ui->tableau->setItem(i, 3, new QTableWidgetItem(emp.email));
        ui->tableau->setItem(i, 4, new QTableWidgetItem(emp.poste));
        ui->tableau->setItem(i, 5, new QTableWidgetItem(QString::number(emp.salaire)));
        ui->tableau->setItem(i, 6, new QTableWidgetItem(emp.adresse));
        ui->tableau->setItem(i, 7, new QTableWidgetItem(QString::number(emp.telephone)));
    }
    
    qDebug() << "✓" << employes.size() << "employés chargés";
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
    if (row < 0) {
        return;
    }
    
    // Remplir le formulaire avec les données sélectionnées
    if (ui->nomline) ui->nomline->setText(ui->tableau->item(row, 1)->text());
    if (ui->prenomline) ui->prenomline->setText(ui->tableau->item(row, 2)->text());
    if (ui->emailline) ui->emailline->setText(ui->tableau->item(row, 3)->text());
    if (ui->posteline) ui->posteline->setText(ui->tableau->item(row, 4)->text());
    if (ui->salaireline) ui->salaireline->setText(ui->tableau->item(row, 5)->text());
    if (ui->adresseline) ui->adresseline->setText(ui->tableau->item(row, 6)->text());
    if (ui->telephoneline) ui->telephoneline->setText(ui->tableau->item(row, 7)->text());
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
        ui->tableau_3->setItem(row, 0, new QTableWidgetItem(query.value("ID_VEHI").toString()));
        ui->tableau_3->setItem(row, 1, new QTableWidgetItem(query.value("IMMAT").toString()));
        ui->tableau_3->setItem(row, 2, new QTableWidgetItem(query.value("MARQUE").toString()));
        ui->tableau_3->setItem(row, 3, new QTableWidgetItem(query.value("MODELE").toString()));
        ui->tableau_3->setItem(row, 4, new QTableWidgetItem(query.value("TYPE").toString()));
        ui->tableau_3->setItem(row, 5, new QTableWidgetItem(query.value("ETAT").toString()));
        ui->tableau_3->setItem(row, 6, new QTableWidgetItem(query.value("SERVICE").toString()));
        ui->tableau_3->setItem(row, 7, new QTableWidgetItem(query.value("DATE_MAINT").toString()));
        ++row;
    }
    
    qDebug() << "✓" << row << "véhicules chargés";
}

void MainWindow::onAjouterVehicule()
{
    if (!ui->immatline_2 || !ui->marqueline_2 || !ui->modeleline_2 || !ui->Typeline_2 || !ui->Etatline_2
        || !ui->service_2 || !ui->datemaintline_2) {
        QMessageBox::warning(this, "Erreur", "Les champs du formulaire véhicule sont introuvables !");
        return;
    }
    
    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    QString type = ui->Typeline_2->text().trimmed();
    QString etat = ui->Etatline_2->text().trimmed();
    QString service = ui->service_2->text().trimmed();
    
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
    
    QString oldImmat = ui->tableau_3->item(row, 1)->text();
    
    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    QString type = ui->Typeline_2->text().trimmed();
    QString etat = ui->Etatline_2->text().trimmed();
    QString service = ui->service_2->text().trimmed();
    
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
    
    QString immat = ui->tableau_3->item(row, 1)->text();
    QString marque = ui->tableau_3->item(row, 2)->text();
    QString modele = ui->tableau_3->item(row, 3)->text();
    
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
        return;
    }
    
    // Remplir le formulaire avec les données sélectionnées
    if (ui->immatline_2) ui->immatline_2->setText(ui->tableau_3->item(row, 1)->text());
    if (ui->marqueline_2) ui->marqueline_2->setText(ui->tableau_3->item(row, 2)->text());
    if (ui->modeleline_2) ui->modeleline_2->setText(ui->tableau_3->item(row, 3)->text());
    if (ui->Typeline_2) ui->Typeline_2->setText(ui->tableau_3->item(row, 4)->text());
    if (ui->Etatline_2) ui->Etatline_2->setText(ui->tableau_3->item(row, 5)->text());
    if (ui->service_2) ui->service_2->setText(ui->tableau_3->item(row, 6)->text());
    if (ui->datemaintline_2) ui->datemaintline_2->setText(ui->tableau_3->item(row, 7)->text());
}

void MainWindow::reinitialiserFormulaireVehicule()
{
    if (ui->immatline_2) ui->immatline_2->clear();
    if (ui->marqueline_2) ui->marqueline_2->clear();
    if (ui->modeleline_2) ui->modeleline_2->clear();
    if (ui->Typeline_2) ui->Typeline_2->clear();
    if (ui->Etatline_2) ui->Etatline_2->clear();
    if (ui->service_2) ui->service_2->clear();
    if (ui->datemaintline_2) ui->datemaintline_2->clear();
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
    
    // Remplir le formulaire avec les données sélectionnées
    if (ui->idmaisonline) ui->idmaisonline->setText(ui->tableau_7->item(row, 0)->text());
    if (ui->adresseline_2) ui->adresseline_2->setText(ui->tableau_7->item(row, 1)->text());
    if (ui->nivsecline) ui->nivsecline->setText(ui->tableau_7->item(row, 2)->text());
}

void MainWindow::reinitialiserFormulaireMaison()
{
    if (ui->idmaisonline) ui->idmaisonline->clear();
    if (ui->adresseline_2) ui->adresseline_2->clear();
    if (ui->nivsecline) ui->nivsecline->clear();
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
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 4, typeItem);
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

    const QString typeChoix = ui->typechoix ? ui->typechoix->currentText().trimmed() : QString();
    if (typeChoix.isEmpty()) {
        QMessageBox::warning(this, tr("Jardins"), tr("Veuillez sélectionner le type de jardin."));
        return;
    }

    Jardin jardin(id, emplacement, superficie, typeSol, typeChoix);
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

    Jardin jardin(newId, emplacement, superficie, typeSol, typeChoix);
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
    if (ui->typechoix) {
        const QString type = ui->tableau_8->item(row, 4)->text();
        const int index = ui->typechoix->findText(type);
        if (index >= 0) {
            ui->typechoix->setCurrentIndex(index);
        }
    }
}

void MainWindow::onAfficherJardinStats()
{
    QString error;
    const QList<Jardin> jardins = Jardin::fetchAll(&error);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Jardins"), tr("Impossible de récupérer les statistiques :\n%1").arg(error));
        return;
    }

    QList<JardinStat> stats;
    stats.reserve(jardins.size());
    for (const Jardin &j : jardins) {
        JardinStat stat;
        stat.id = j.id();
        stat.emplacement = j.emplacement();
        stat.superficie = QString::number(j.superficie(), 'f', 2);
        stat.typeSol = j.typeSol();
        stat.typeChoix = j.typeChoix();
        stats.append(stat);
    }

    JardinStatisticsDialog dialog(this);
    dialog.setJardinsData(stats);
    dialog.exec();
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
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 4, typeItem);
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
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 4, typeItem);
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
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 4, typeItem);
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
        QTableWidgetItem *typeItem = new QTableWidgetItem(j.typeChoix());
        typeItem->setToolTip(tr("Type de sol : %1").arg(j.typeSol()));
        ui->tableau_8->setItem(row, 4, typeItem);
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
    html += QStringLiteral("<tr><th>ID</th><th>Emplacement</th><th>Superficie</th><th>Type de sol</th><th>Type</th></tr>");
    const QLocale locale;
    for (const Jardin &j : jardins) {
        html += QStringLiteral("<tr><td>%1</td><td>%2</td><td>%3 m²</td><td>%4</td><td>%5</td></tr>")
                    .arg(j.id())
                    .arg(j.emplacement().toHtmlEscaped())
                    .arg(locale.toString(j.superficie(), 'f', 2))
                    .arg(j.typeSol().toHtmlEscaped())
                    .arg(j.typeChoix().toHtmlEscaped());
    }
    html += QStringLiteral("</table>");
    return html;
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
// GESTION DE LA MAINTENANCE PRÉDICTIVE
// ============================================================

void MainWindow::chargerMaintenancePredictive()
{
    if (!ui->tableau_9) {
        return;
    }

    QString error;
    const QList<Maintenance> maintenances = Maintenance::fetchMaintenancePredictive(&error);
    if (!error.isEmpty()) {
        qWarning() << "Erreur chargement maintenance:" << error;
        return;
    }

    ui->tableau_9->clearContents();
    ui->tableau_9->setRowCount(maintenances.size());

    int row = 0;
    for (const Maintenance &m : maintenances) {
        // Colonnes: Plantes (type), Zones (jardin id), prédictive (date recommandée), Sol (statut)
        ui->tableau_9->setItem(row, 0, new QTableWidgetItem(m.typeMaintenance()));
        ui->tableau_9->setItem(row, 1, new QTableWidgetItem(QString::number(m.idJardin())));
        ui->tableau_9->setItem(row, 2, new QTableWidgetItem(m.dateRecommandee().toString("dd/MM/yyyy")));
        ui->tableau_9->setItem(row, 3, new QTableWidgetItem(m.statut()));
        ++row;
    }
}

void MainWindow::chargerRecommandationsPlantes(const QString &typeSol)
{
    if (!ui->tableau_11) {
        return;
    }

    QString error;
    const QList<RecommandationPlante> recommandations = RecommandationPlante::fetchByTypeSol(typeSol, &error);
    if (!error.isEmpty()) {
        qWarning() << "Erreur chargement recommandations:" << error;
        return;
    }

    ui->tableau_11->clearContents();
    ui->tableau_11->setRowCount(recommandations.size());

    int row = 0;
    for (const RecommandationPlante &rp : recommandations) {
        // Colonnes: Espèces (id), Compatibilité (type sol), suggestions (recommandation)
        ui->tableau_11->setItem(row, 0, new QTableWidgetItem(QString::number(rp.idPlante())));
        ui->tableau_11->setItem(row, 1, new QTableWidgetItem(rp.typeSol()));
        ui->tableau_11->setItem(row, 2, new QTableWidgetItem(rp.recommandation()));
        ++row;
    }
}

void MainWindow::onAfficherMaintenancePredictive()
{
    chargerMaintenancePredictive();
    // Naviguer vers la page de maintenance prédictive
    if (ui->stackedWidget_6) {
        ui->stackedWidget_6->setCurrentWidget(ui->page_17);
    }
}

void MainWindow::onJardinSelectionChangedForMaintenance()
{
    if (!ui->tableau_8) {
        return;
    }

    const int row = ui->tableau_8->currentRow();
    if (row < 0) {
        return;
    }

    // Récupérer le type de sol du jardin sélectionné
    QTableWidgetItem *typeSolItem = ui->tableau_8->item(row, 3);
    if (typeSolItem) {
        const QString typeSol = typeSolItem->text();
        chargerRecommandationsPlantes(typeSol);
    }

    // Récupérer l'ID du jardin pour charger ses maintenances
    QTableWidgetItem *idItem = ui->tableau_8->item(row, 0);
    if (idItem) {
        const int idJardin = idItem->text().toInt();
        
        // Charger les maintenances pour ce jardin
        if (ui->tableau_9) {
            QString error;
            const QList<Maintenance> maintenances = Maintenance::fetchByJardin(idJardin, &error);
            
            ui->tableau_9->clearContents();
            ui->tableau_9->setRowCount(maintenances.size());
            
            int r = 0;
            for (const Maintenance &m : maintenances) {
                ui->tableau_9->setItem(r, 0, new QTableWidgetItem(m.typeMaintenance()));
                ui->tableau_9->setItem(r, 1, new QTableWidgetItem(QString::number(m.idJardin())));
                ui->tableau_9->setItem(r, 2, new QTableWidgetItem(m.dateRecommandee().toString("dd/MM/yyyy")));
                ui->tableau_9->setItem(r, 3, new QTableWidgetItem(m.statut()));
                ++r;
            }
        }
    }
}

// ============================================================
// DIALOGUES JARDIN ET CABINET
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

void MainWindow::onOuvrirVideoDialog()
{
    VideoDialog dialog(this);
    dialog.exec();
}
