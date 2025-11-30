#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPixmap>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QColor>
#include <QAbstractItemView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPdfWriter>
#include <QSqlError>
#include <QPrinter>
#include <QPageSize>
#include <QTextDocument>
#include <QFileDialog>
#include <QPainter>
#include <QDir>
#include <QDateTime>
#include <QLineEdit>
#include <QComboBox>
#include <algorithm>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>
#include <QtCharts/QLegend>
#include <QVBoxLayout>
#include <QDialog>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QDate>
#include <QPushButton>
#include <QFont>
#include "Employee.h"
#include "vehicule.h"
#include "InputController.h"
#include "maison.h"
#include "resident.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "connection.h"
#include <QJsonArray>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "maison.h"
#include "alerte.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QDesktopServices>
#include <QUrl>

#include <QMessageBox>
#include <QSqlQueryModel>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include <QDate>
#include <QPrinter>
#include <QTextDocument>
#include <QGraphicsView>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QLineEdit { color: black; }");
    if (ui->idmaisonline) ui->idmaisonline->setStyleSheet("color: black; background-color: white;");
    if (ui->adresseline_2) ui->adresseline_2->setStyleSheet("color: black; background-color: white;");
    if (ui->nivsecline) ui->nivsecline->setStyleSheet("color: black; background-color: white;");
    net = new QNetworkAccessManager(this);
    sceneCarte = new QGraphicsScene(this);
    viewCarte = new QGraphicsView(sceneCarte, ui->cruds_10);
    viewCarte->setGeometry(QRect(720, 80, 370, 230));
    viewCarte->setStyleSheet("background-color: white;");
    connect(ui->tribox, &QComboBox::currentTextChanged,
            this, &MainWindow::onTriBoxChanged);
    networkManager = new QNetworkAccessManager(this);
    this->resize(1920, 1080);
    this->move(0, 0);

    QPixmap logo(":/images/Lg.png");
    ui->logo->setPixmap(logo);
    ui->logo->setScaledContents(true);

    connectButtons();
    connectAlerteButtons();
    connectJardinUi();
    refreshJardinTable();
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
    if (ui->tableau_7) ui->tableau_7->setSortingEnabled(true);
    if (ui->triMaisons) connect(ui->triMaisons, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTriMaisonsChanged);
    if (ui->rechercheMaisons) connect(ui->rechercheMaisons, &QLineEdit::textChanged, this, &MainWindow::onRechercheMaisonsChanged);
    if (ui->btnExporterMaisons) connect(ui->btnExporterMaisons, &QPushButton::clicked, this, &MainWindow::onExporterMaison);
    connect(ui->tableau_3, &QTableWidget::cellClicked, this, [this](int row, int){
        qDebug() << "🟢 [DEBUG] cellClicked fired - row =" << row;

        // Ensure there are enough columns before accessing items
        if (ui->tableau_3->columnCount() < 8) {
            qDebug() << "⚠️ [DEBUG] Table doesn't have expected columns!";
            return;
        }

        // Fill inputs
        ui->immatline_2->setText(ui->tableau_3->item(row, 1)->text());
        ui->marqueline_2->setText(ui->tableau_3->item(row, 2)->text());
        ui->modeleline_2->setText(ui->tableau_3->item(row, 3)->text());
        ui->triemail_2->setCurrentText(ui->tableau_3->item(row, 4)->text());
        ui->Etatline_2->setCurrentText(ui->tableau_3->item(row, 5)->text());
        ui->serviceline_2->setText(ui->tableau_3->item(row, 6)->text());
        QString dateStr = ui->tableau_3->item(row, 7)->text();  // get date from table
        QDate date = QDate::fromString(dateStr, "dd/MM/yyyy");  // convert to QDate
        ui->datemaintline_2->setDate(date);
        ui->datemaintline_2->setDisplayFormat("dd/MM/yyyy");


        selectedImmat = ui->tableau_3->item(row, 1)->text();
        qDebug() << "✅ [DEBUG] selectedImmat set to:" << selectedImmat;
    });
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

    // CRUD Employés
    connect(ui->ajouter,  &QPushButton::clicked, this, &MainWindow::onAjouterEmploye);
    connect(ui->modifier, &QPushButton::clicked, this, &MainWindow::onModifierEmploye);
    connect(ui->supprimer,&QPushButton::clicked, this, &MainWindow::onSupprimerEmploye);
    connect(ui->exporter, &QPushButton::clicked, this, &MainWindow::onExporterPDF);
    
    // Statistiques Employés
    if (ui->stat_emp) {
        connect(ui->stat_emp, &QPushButton::clicked, this, &MainWindow::onStatistiqueEmploye);
    }

    // CRUD Véhicules (connexion manuelle uniquement, pas de connexion automatique)
    connect(ui->ajouter_3,  &QPushButton::clicked, this, &MainWindow::on_ajouter_3_clicked);
    connect(ui->modifier_3, &QPushButton::clicked, this, &MainWindow::on_modifier_3_clicked);
    connect(ui->supprimer_3,&QPushButton::clicked, this, &MainWindow::on_supprimer_3_clicked);
    connect(ui->exporter_3,&QPushButton::clicked, this, &MainWindow::on_exporter_3_clicked);

    // CRUD Maisons
    connect(ui->ajouter_7, &QPushButton::clicked, this, &MainWindow::onAjouterMaison);
    connect(ui->modifier_7, &QPushButton::clicked, this, &MainWindow::onModifierMaison);
    connect(ui->supprimer_6, &QPushButton::clicked, this, &MainWindow::onSupprimerMaison);
    if (ui->Alertes) {
        connect(ui->Alertes, &QPushButton::clicked, this, &MainWindow::on_Alertes_clicked);
    }
    
    // CRUD Residents (connexion manuelle uniquement, pas de connexion automatique)
    connect(ui->ajouter_5, &QPushButton::clicked, this, &MainWindow::onAjouterResident);
    connect(ui->modifier_5, &QPushButton::clicked, this, &MainWindow::onModifierResident);
    connect(ui->supprimer_5, &QPushButton::clicked, this, &MainWindow::onSupprimerResident);
    connect(ui->exporter_5, &QPushButton::clicked, this, &MainWindow::onExporterResident);
    
    // Connect table selection for maison
    connect(ui->tableau_7, &QTableWidget::cellClicked, this, [this](int row, int){
        if (ui->tableau_7->columnCount() < 6 || row < 0) return;
        
        // Store the old ID before filling the form
        QString oldIdStr = ui->tableau_7->item(row, 0)->text();
        bool ok;
        selectedMaisonId = oldIdStr.toInt(&ok);
        if (!ok) selectedMaisonId = -1;
        
        qDebug() << "🔍 [DEBUG] Maison sélectionnée - Ancien ID stocké:" << selectedMaisonId;
        
    // Fill inputs from selected row
    ui->idmaisonline->setText(oldIdStr);
    ui->adresseline_2->setText(ui->tableau_7->item(row, 1)->text());
    ui->nivsecline->setText(ui->tableau_7->item(row, 2)->text());
    ui->statuschoix->setCurrentText(ui->tableau_7->item(row, 3)->text());
    ui->typechoix_2->setCurrentText(ui->tableau_7->item(row, 4)->text());
    ui->nbrpieceschoix->setCurrentText(ui->tableau_7->item(row, 5)->text());
    });
    
    // Connect table selection for residents
    connect(ui->tableau_5, &QTableWidget::cellClicked, this, [this](int row, int){
        if (ui->tableau_5->columnCount() < 8 || row < 0) return;
        
        // Store the old ID before filling the form
        QString oldIdStr = ui->tableau_5->item(row, 0)->text();
        bool ok;
        selectedResidentId = oldIdStr.toInt(&ok);
        if (!ok) selectedResidentId = -1;
        
        qDebug() << "🔍 [DEBUG] Resident sélectionné - Ancien ID stocké:" << selectedResidentId;
        
        // Fill inputs from selected row
        ui->idresidentline_2->setText(oldIdStr);
        ui->nomline_7->setText(ui->tableau_5->item(row, 1)->text());
        QString dateStr = ui->tableau_5->item(row, 2)->text();
        ui->datenaissanceline_2->setText(dateStr);
        ui->adresseline_4->setText(ui->tableau_5->item(row, 3)->text());
        ui->telephoneline_5->setText(ui->tableau_5->item(row, 4)->text());
        ui->emailline_4->setText(ui->tableau_5->item(row, 5)->text());
        ui->statusline_2->setText(ui->tableau_5->item(row, 6)->text());
        // SITUATION_FAMILILE (situationline_2) - column 7
        ui->situationline_2->setText(ui->tableau_5->item(row, 7)->text());
    });


    // Table selection -> fill ID for delete/edit
    ui->tableau->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableau->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->tableau, &QTableWidget::itemSelectionChanged, this, &MainWindow::onTableSelectionChanged);
    
    // Hide the recherche QLabel and replace it with QLineEdit
    if (ui->recherche) {
        ui->recherche->hide();
    }


    
    // Setup recherche and tri combo boxes for Employés
    ui->triemail->clear();
    ui->triemail->addItems(QStringList() << "Email" << "A-Z" << "Z-A");
    ui->trisalaire->clear();
    ui->trisalaire->addItems(QStringList() << "Salaire" << "Croissant" << "Décroissant");
    connect(ui->triemail, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTriEmailChanged);
    connect(ui->trisalaire, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTriSalaireChanged);
    
    // Find or create recherche line edit widget (position it where the label was)
    QList<QLineEdit*> lineEdits = ui->trirecherche->findChildren<QLineEdit*>();
    if (!lineEdits.isEmpty()) {
        m_rechercheLineEdit = lineEdits.first();
        m_rechercheLineEdit->setPlaceholderText(tr("Rechercher (Nom & prenom)"));
        m_rechercheLineEdit->setGeometry(10, 10, 301, 25);
        m_rechercheLineEdit->setStyleSheet("background-color: white; color: black; font: 10pt \"Arial\";");
        connect(m_rechercheLineEdit, &QLineEdit::textChanged, this, &MainWindow::onRechercheChanged);
    } else {
        // Create a QLineEdit at the same position as the label
        m_rechercheLineEdit = new QLineEdit(ui->trirecherche);
        m_rechercheLineEdit->setPlaceholderText(tr("Rechercher (Nom & prenom)"));
        m_rechercheLineEdit->setGeometry(10, 10, 301, 25);
        m_rechercheLineEdit->setStyleSheet("background-color: white; color: black; font: 10pt \"Arial\";");
        connect(m_rechercheLineEdit, &QLineEdit::textChanged, this, &MainWindow::onRechercheChanged);
    }
}
void MainWindow::onGestionEmployes()
{
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
    onChargerEmployes();
}

void MainWindow::onGestionVehicules()
{
    ui->stackedWidget->setCurrentWidget(ui->pageVehicules);
    loadVehicules();
}

void MainWindow::onGestionResidents()
{
    ui->stackedWidget->setCurrentWidget(ui->pageResidents);
    loadResidents();
}

void MainWindow::onGestionMaisons()
{
    ui->stackedWidget->setCurrentWidget(ui->pageMaisons);
    loadMaisons();
}

void MainWindow::onGestionJardins()
{
    ui->stackedWidget->setCurrentWidget(ui->pageJardins);
    refreshJardinTable();
}

void MainWindow::onGestionCabinets()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCabinets);
}

void MainWindow::onDeconnexion()
{
    close();
}
void MainWindow::on_btnChatbot_clicked()
{
    ui->stackedWidget_3->setCurrentWidget(ui->pageChatbot);
}

void MainWindow::on_btnBackFromChat_clicked()
{
    ui->stackedWidget_3->setCurrentWidget(ui->page_9);
}

void MainWindow::on_btnBackFromRecom_clicked()
{
    ui->stackedWidget_3->setCurrentWidget(ui->page_9);
}

// === Employés CRUD ===
void MainWindow::onChargerEmployes()
{
    // Connection/driver sanity check + ping
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QMessageBox::warning(this, tr("Erreur DB"), tr("Base non ouverte. Driver: %1\n%2").arg(db.driverName(), db.lastError().text()));
        return;
    }
    QSqlQuery ping(db);
    if (!ping.exec("SELECT 1 FROM DUAL")) {
        QMessageBox::warning(this, tr("Erreur DB"), QString("Ping failed: %1 | %2 | %3")
                             .arg(ping.lastError().driverText())
                             .arg(ping.lastError().databaseText())
                             .arg(ping.lastError().nativeErrorCode()));
        return;
    }

    QString err;
    m_allEmployees = Employee::fetchAll(err); // Store all employees for filtering
    if (!err.isEmpty()) {
        qWarning() << "Erreur liste employés:" << err;
        QMessageBox::warning(this, tr("Erreur DB"), err);
        return;
    }

    appliquerRechercheEtTri();
}

void MainWindow::onAjouterEmploye()
{
    // Préparer les données d'entrée
    EmployeeInput input;
    input.nom = ui->nomline->text();
    input.prenom = ui->prenomline->text();
    input.poste = ui->posteline->text();
    input.salaire = ui->salaireline->text();
    input.adresse = ui->adresseline->text();
    input.telephone = ui->telephoneline->text();
    input.email = ui->emailline->text();
    input.idRes = ""; // Optionnel, peut être ajouté plus tard si nécessaire

    // Valider les données
    ValidationResult validation = InputController::validateCreate(input);
    
    if (!validation.ok) {
        // Construire le message d'erreur avec tous les champs en erreur
        QStringList erreurs;
        if (validation.fieldErrors.contains("nom")) {
            erreurs << "Nom: " + validation.fieldErrors["nom"];
        }
        if (validation.fieldErrors.contains("prenom")) {
            erreurs << "Prénom: " + validation.fieldErrors["prenom"];
        }
        if (validation.fieldErrors.contains("poste")) {
            erreurs << "Poste: " + validation.fieldErrors["poste"];
        }
        if (validation.fieldErrors.contains("salaire")) {
            erreurs << "Salaire: " + validation.fieldErrors["salaire"];
        }
        if (validation.fieldErrors.contains("adresse")) {
            erreurs << "Adresse: " + validation.fieldErrors["adresse"];
        }
        if (validation.fieldErrors.contains("telephone")) {
            erreurs << "Téléphone: " + validation.fieldErrors["telephone"];
        }
        if (validation.fieldErrors.contains("email")) {
            erreurs << "Email: " + validation.fieldErrors["email"];
        }
        if (validation.fieldErrors.contains("idRes")) {
            erreurs << "ID Résident: " + validation.fieldErrors["idRes"];
        }
        
        QMessageBox::warning(this, tr("Erreur de validation"), 
                           tr("Veuillez corriger les erreurs suivantes:\n\n%1")
                           .arg(erreurs.join("\n")));
        return;
    }

    // Utiliser les données normalisées et validées
    Employee e = validation.normalized;
    
    // Si votre DB nécessite un ID manuel (Oracle), le définir depuis le champ
    bool okId = false;
    int id = ui->idline->text().toInt(&okId);
    if (okId && id > 0) e.idEmploye = id;

    int newId = 0; QString err;
    if (e.insert(newId, err)) {
        // Log history
        QString logMsg = QString("[%1] AJOUT - ID: %2 | Nom: %3 | Prénom: %4 | Email: %5 | Poste: %6 | Salaire: %7")
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                         .arg(newId > 0 ? newId : e.idEmploye)
                         .arg(e.nom)
                         .arg(e.prenom)
                         .arg(e.email)
                         .arg(e.poste)
                         .arg(e.salaire);
        qDebug() << logMsg;
        
        QMessageBox::information(this, tr("Succès"), tr("Employé ajouté avec succès."));
        m_lastInsertedId = (newId > 0 ? newId : e.idEmploye);
        // Clear input fields after successful insert
        ui->tableau->clearSelection(); // Clear table selection first to prevent repopulating idline
        ui->idline->clear();
        ui->nomline->clear();
        ui->prenomline->clear();
        ui->posteline->clear();
        ui->salaireline->clear();
        ui->adresseline->clear();
        ui->telephoneline->clear();
        ui->emailline->clear();
        onChargerEmployes();
    } else {
        QMessageBox::warning(this, tr("Erreur"), err);
    }
}

void MainWindow::onModifierEmploye()
{
    bool okId = false;
    const int id = ui->idline->text().toInt(&okId);
    if (!okId || id <= 0) { 
        QMessageBox::warning(this, tr("Erreur"), tr("ID invalide")); 
        return; 
    }

    // Préparer les données d'entrée
    EmployeeInput input;
    input.nom = ui->nomline->text();
    input.prenom = ui->prenomline->text();
    input.poste = ui->posteline->text();
    input.salaire = ui->salaireline->text();
    input.adresse = ui->adresseline->text();
    input.telephone = ui->telephoneline->text();
    input.email = ui->emailline->text();
    input.idRes = ""; // Optionnel

    // Valider les données
    ValidationResult validation = InputController::validateUpdate(input);
    
    if (!validation.ok) {
        // Construire le message d'erreur avec tous les champs en erreur
        QStringList erreurs;
        if (validation.fieldErrors.contains("nom")) {
            erreurs << "Nom: " + validation.fieldErrors["nom"];
        }
        if (validation.fieldErrors.contains("prenom")) {
            erreurs << "Prénom: " + validation.fieldErrors["prenom"];
        }
        if (validation.fieldErrors.contains("poste")) {
            erreurs << "Poste: " + validation.fieldErrors["poste"];
        }
        if (validation.fieldErrors.contains("salaire")) {
            erreurs << "Salaire: " + validation.fieldErrors["salaire"];
        }
        if (validation.fieldErrors.contains("adresse")) {
            erreurs << "Adresse: " + validation.fieldErrors["adresse"];
        }
        if (validation.fieldErrors.contains("telephone")) {
            erreurs << "Téléphone: " + validation.fieldErrors["telephone"];
        }
        if (validation.fieldErrors.contains("email")) {
            erreurs << "Email: " + validation.fieldErrors["email"];
        }
        if (validation.fieldErrors.contains("idRes")) {
            erreurs << "ID Résident: " + validation.fieldErrors["idRes"];
        }
        
        QMessageBox::warning(this, tr("Erreur de validation"), 
                           tr("Veuillez corriger les erreurs suivantes:\n\n%1")
                           .arg(erreurs.join("\n")));
        return;
    }

    // Utiliser les données normalisées et validées
    Employee e = validation.normalized;

    QString err;
    if (e.updateById(id, err)) {
        // Log history
        QString logMsg = QString("[%1] MODIFICATION - ID: %2 | Nom: %3 | Prénom: %4 | Email: %5 | Poste: %6 | Salaire: %7")
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                         .arg(id)
                         .arg(e.nom)
                         .arg(e.prenom)
                         .arg(e.email)
                         .arg(e.poste)
                         .arg(e.salaire);
        qDebug() << logMsg;
        
        QMessageBox::information(this, tr("Succès"), tr("Employé modifié avec succès."));
        // Clear input fields after successful update
        ui->tableau->clearSelection();
        ui->idline->clear();
        ui->nomline->clear();
        ui->prenomline->clear();
        ui->posteline->clear();
        ui->salaireline->clear();
        ui->adresseline->clear();
        ui->telephoneline->clear();
        ui->emailline->clear();
        onChargerEmployes();
    } else {
        QMessageBox::warning(this, tr("Erreur"), err);
    }
}

void MainWindow::onSupprimerEmploye()
{
    // Prefer selected row's ID
    int id = -1;
    if (ui->tableau->currentRow() >= 0) {
        QTableWidgetItem *it = ui->tableau->item(ui->tableau->currentRow(), 0);
        if (it) id = it->text().toInt();
    }
    bool okId = id > 0;
    if (!okId) {
        id = ui->idline->text().toInt(&okId);
    }
    if (!okId || id <= 0) { QMessageBox::warning(this, tr("Erreur"), tr("ID invalide")); return; }
    if (QMessageBox::question(this, tr("Confirmer"), tr("Supprimer l'employé?")) != QMessageBox::Yes) return;

    QString err;
    if (Employee::removeById(id, err)) {
        // Log history
        QString logMsg = QString("[%1] SUPPRESSION - ID: %2")
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                         .arg(id);
        qDebug() << logMsg;
        
        QMessageBox::information(this, tr("Succès"), tr("Employé supprimé."));
        // Clear input fields after successful delete
        ui->tableau->clearSelection();
        ui->idline->clear();
        ui->nomline->clear();
        ui->prenomline->clear();
        ui->posteline->clear();
        ui->salaireline->clear();
        ui->adresseline->clear();
        ui->telephoneline->clear();
        ui->emailline->clear();
        onChargerEmployes();
    } else {
        QMessageBox::warning(this, tr("Erreur"), err);
    }
}

void MainWindow::onTableSelectionChanged()
{
    if (ui->tableau->currentRow() < 0) return;
    int row = ui->tableau->currentRow();
    
    // Populate all input fields from selected row
    QTableWidgetItem *idItem = ui->tableau->item(row, 0);
    QTableWidgetItem *nomItem = ui->tableau->item(row, 1);
    QTableWidgetItem *prenomItem = ui->tableau->item(row, 2);
    QTableWidgetItem *emailItem = ui->tableau->item(row, 3);
    QTableWidgetItem *posteItem = ui->tableau->item(row, 4);
    QTableWidgetItem *salaireItem = ui->tableau->item(row, 5);
    QTableWidgetItem *adresseItem = ui->tableau->item(row, 6);
    QTableWidgetItem *telephoneItem = ui->tableau->item(row, 7);
    
    if (idItem) ui->idline->setText(idItem->text());
    if (nomItem) ui->nomline->setText(nomItem->text());
    if (prenomItem) ui->prenomline->setText(prenomItem->text());
    if (emailItem) ui->emailline->setText(emailItem->text());
    if (posteItem) ui->posteline->setText(posteItem->text());
    if (salaireItem) ui->salaireline->setText(salaireItem->text());
    if (adresseItem) ui->adresseline->setText(adresseItem->text());
    if (telephoneItem) ui->telephoneline->setText(telephoneItem->text());
}

void MainWindow::onExporterPDF()
{
    if (ui->tableau->rowCount() == 0) {
        QMessageBox::warning(this, tr("Avertissement"), tr("Aucune donnée à exporter."));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer le PDF"), 
                                                     QDir::homePath() + "/employes.pdf",
                                                     tr("PDF Files (*.pdf)"));
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setOutputFileName(fileName);

    QTextDocument doc;
    QString html = "<html><head><style>"
                   "table { border-collapse: collapse; width: 100%; }"
                   "th, td { border: 1px solid #000; padding: 8px; text-align: left; }"
                   "th { background-color: #4CAF50; color: white; font-weight: bold; }"
                   "tr:nth-child(even) { background-color: #f2f2f2; }"
                   "h1 { text-align: center; color: #333; }"
                   "</style></head><body>";
    html += "<h1>Liste des Employés</h1>";
    html += "<table><thead><tr>";

    // Headers
    for (int col = 0; col < ui->tableau->columnCount(); ++col) {
        QTableWidgetItem *header = ui->tableau->horizontalHeaderItem(col);
        if (header) {
            html += "<th>" + header->text().toHtmlEscaped() + "</th>";
        }
    }
    html += "</tr></thead><tbody>";

    // Data rows
    for (int row = 0; row < ui->tableau->rowCount(); ++row) {
        html += "<tr>";
        for (int col = 0; col < ui->tableau->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableau->item(row, col);
            QString text = item ? item->text() : "";
            html += "<td>" + text.toHtmlEscaped() + "</td>";
        }
        html += "</tr>";
    }
    html += "</tbody></table></body></html>";

    doc.setHtml(html);
    QRectF pageRect = printer.pageLayout().paintRectPixels(printer.resolution());
    doc.setPageSize(pageRect.size());
    doc.print(&printer);

    QMessageBox::information(this, tr("Succès"), 
                            tr("PDF exporté avec succès:\n%1").arg(fileName));
}

void MainWindow::appliquerRechercheEtTri()
{
    if (m_allEmployees.isEmpty()) {
        ui->tableau->setRowCount(0);
        return;
    }

    QVector<Employee> filtered = m_allEmployees;

    // Appliquer recherche par nom/prénom (alphabétique par lettre)
    if (m_rechercheLineEdit && !m_rechercheLineEdit->text().isEmpty()) {
        QString searchText = m_rechercheLineEdit->text().trimmed().toUpper();
        QVector<Employee> temp;
        for (const Employee &e : filtered) {
            QString fullName = (e.nom + " " + e.prenom).toUpper();
            if (fullName.contains(searchText)) {
                temp.append(e);
            }
        }
        filtered = temp;
    }

    // Appliquer tri par email et salaire
    int emailIndex = ui->triemail->currentIndex();
    int salaireIndex = ui->trisalaire->currentIndex();
    
    // Si les deux tris sont actifs, combiner (salaire prioritaire, puis email)
    if (emailIndex > 0 && salaireIndex > 0) {
        std::sort(filtered.begin(), filtered.end(), 
                  [emailIndex, salaireIndex](const Employee &a, const Employee &b) {
                      // D'abord par salaire
                      if (salaireIndex == 1) { // Croissant
                          if (a.salaire != b.salaire) return a.salaire < b.salaire;
                      } else { // Décroissant
                          if (a.salaire != b.salaire) return a.salaire > b.salaire;
                      }
                      // Ensuite par email
                      if (emailIndex == 1) { // A-Z
                          return a.email.toUpper() < b.email.toUpper();
                      } else { // Z-A
                          return a.email.toUpper() > b.email.toUpper();
                      }
                  });
    } else {
        // Tri par email seul
        if (emailIndex == 1) { // A-Z
            std::sort(filtered.begin(), filtered.end(), 
                      [](const Employee &a, const Employee &b) {
                          return a.email.toUpper() < b.email.toUpper();
                      });
        } else if (emailIndex == 2) { // Z-A
            std::sort(filtered.begin(), filtered.end(), 
                      [](const Employee &a, const Employee &b) {
                          return a.email.toUpper() > b.email.toUpper();
                      });
        }
        
        // Tri par salaire seul
        if (salaireIndex == 1) { // Croissant
            std::sort(filtered.begin(), filtered.end(), 
                      [](const Employee &a, const Employee &b) {
                          return a.salaire < b.salaire;
                      });
        } else if (salaireIndex == 2) { // Décroissant
            std::sort(filtered.begin(), filtered.end(), 
                      [](const Employee &a, const Employee &b) {
                          return a.salaire > b.salaire;
                      });
        }
    }

    // Afficher les résultats dans le tableau
    ui->tableau->clear();
    ui->tableau->setColumnCount(8);
    ui->tableau->setRowCount(filtered.size());
    QStringList headers; headers << " ID Employee" << "Nom" << "Prénom" << "Email" << "Poste" << "Salaire" << "Adresse" << "Téléphone";
    ui->tableau->setHorizontalHeaderLabels(headers);
    
    int highlightRow = -1;
    for (int i = 0; i < filtered.size(); ++i) {
        const Employee &e = filtered[i];
        ui->tableau->setItem(i, 0, new QTableWidgetItem(QString::number(e.idEmploye)));
        ui->tableau->setItem(i, 1, new QTableWidgetItem(e.nom));
        ui->tableau->setItem(i, 2, new QTableWidgetItem(e.prenom));
        ui->tableau->setItem(i, 3, new QTableWidgetItem(e.email));
        ui->tableau->setItem(i, 4, new QTableWidgetItem(e.poste));
        ui->tableau->setItem(i, 5, new QTableWidgetItem(QString::number(e.salaire, 'f', 2)));
        ui->tableau->setItem(i, 6, new QTableWidgetItem(e.adresse));
        ui->tableau->setItem(i, 7, new QTableWidgetItem(QString::number(e.telephone)));
        if (m_lastInsertedId > 0 && e.idEmploye == m_lastInsertedId) {
            highlightRow = i;
        }
    }
    
    if (highlightRow >= 0) {
        const QColor bg(200, 255, 200); // light green
        for (int col = 0; col < ui->tableau->columnCount(); ++col) {
            QTableWidgetItem *it = ui->tableau->item(highlightRow, col);
            if (it) it->setBackground(bg);
        }
        ui->tableau->setCurrentCell(highlightRow, 0);
        m_lastInsertedId = -1;
        ui->tableau->clearSelection();
        ui->idline->clear();
    } else {
        ui->tableau->clearSelection();
        ui->idline->clear();
    }
}

void MainWindow::onRechercheChanged()
{
    appliquerRechercheEtTri();
}

void MainWindow::onTriEmailChanged()
{
    appliquerRechercheEtTri();
}

void MainWindow::onTriSalaireChanged()
{
    appliquerRechercheEtTri();
}

void MainWindow::onStatistiqueEmploye()
{
    // Récupérer les statistiques par poste
    QString errorText;
    QMap<QString, int> stats = Employee::getStatistiquesParPoste(errorText);
    
    if (!errorText.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), 
                           tr("Erreur lors de la récupération des statistiques:\n%1").arg(errorText));
        return;
    }
    
    if (stats.isEmpty()) {
        QMessageBox::information(this, tr("Information"), 
                                tr("Aucune donnée disponible pour les statistiques."));
        return;
    }
    
    // Créer la série de données pour le graphique en camembert
    QPieSeries* series = new QPieSeries();
    
    // Calculer le total
    int total = 0;
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        total += it.value();
    }
    
    // Créer les tranches du graphique
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        QString poste = it.key();
        int count = it.value();
        double perc = (total > 0) ? (double(count) / total) * 100.0 : 0.0;
        
        QString label = QString("%1\n%2 employé(s) (%3%)")
                            .arg(poste)
                            .arg(count)
                            .arg(QString::number(perc, 'f', 1));
        
        QPieSlice* slice = series->append(label, count);
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
        slice->setLabelFont(QFont("Arial", 10, QFont::Bold));
        
        // Couleur aléatoire pour chaque tranche
        QColor color = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 200, 250);
        slice->setBrush(color);
        
        // Effet d'animation au survol
        QObject::connect(slice, &QPieSlice::hovered, [slice](bool hovered) {
            if (hovered) {
                slice->setExploded(true);
                slice->setExplodeDistanceFactor(0.15);
                slice->setLabelFont(QFont("Arial", 11, QFont::Bold));
            } else {
                slice->setExploded(false);
                slice->setLabelFont(QFont("Arial", 10, QFont::Normal));
            }
        });
    }
    
    // Créer le graphique
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des employés par poste");
    chart->setTitleFont(QFont("Arial", 16, QFont::Bold));
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 9));
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setTheme(QChart::ChartThemeLight);
    
    // Créer la vue du graphique
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    // Bouton de fermeture
    QPushButton* closeButton = new QPushButton("Fermer");
    closeButton->setFixedWidth(120);
    closeButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; border-radius: 8px; padding: 8px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #d32f2f; }"
    );
    
    // Créer le dialogue
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Statistiques des employés par poste");
    dialog->resize(900, 700);
    dialog->setModal(true);
    
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);
    
    // Connecter le bouton de fermeture
    QObject::connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);
    
    // Centrer le dialogue sur la fenêtre principale
    QRect parentRect = this->geometry();
    int x = parentRect.center().x() - dialog->width() / 2;
    int y = parentRect.center().y() - dialog->height() / 2;
    dialog->move(x, y);
    
    // Afficher le dialogue
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

// === Véhicules CRUD ===
void MainWindow::on_ajouter_3_clicked()
{
    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    QString type = ui->triemail_2->currentText().trimmed();
    QString etatOriginal = ui->Etatline_2->currentText();   // valeur brute
    QString etat = etatOriginal.trimmed();                  // nettoyée
    QString service = ui->serviceline_2->text().trimmed();
    QDate date_maint = ui->datemaintline_2->date();

    // ==========================================================
    // 🔍 DEBUG : Pour afficher exactement ce que contient "etat"
    // ==========================================================
    qDebug() << "===== DEBUG ETAT =====";
    qDebug() << "ETAT (brut) =" << etatOriginal;
    qDebug() << "ETAT (trimmed) =" << etat;
    qDebug() << "Longueur (brut) =" << etatOriginal.size();
    qDebug() << "Longueur (trimmed) =" << etat.size();

    for (int i = 0; i < etatOriginal.size(); ++i) {
        qDebug() << "brut[" << i << "] =" << etatOriginal.at(i);
    }
    for (int i = 0; i < etat.size(); ++i) {
        qDebug() << "trim[" << i << "] =" << etat.at(i);
    }
    qDebug() << "========================";
    // FIN DEBUG
    // ==========================================================


    // === CONTRÔLES DE SAISIE ===
    if (immat.isEmpty() || marque.isEmpty() || modele.isEmpty() || service.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs doivent être remplis.");
        return;
    }

    // Vérif immatriculation
    QRegularExpression immatRegex("^\\d{3}-TN-\\d{1,4}$");
    if (!immatRegex.match(immat).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Format d'immatriculation invalide (ex : 123-TN-4567).");
        return;
    }

    // Vérif Type
    if (type.isEmpty() || type == "Type") {
        QMessageBox::warning(this, "Erreur", "Veuillez choisir un type de véhicule.");
        return;
    }

    // Vérif État (ComboBox) — version CORRIGÉE
    QString etatNorm = etat.toLower();  // normalisation

    if (etatNorm != "neuf" && etatNorm != "usé" && etatNorm != "en panne") {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez sélectionner un état valide (Neuf, Usé, En panne).");
        return;
    }


    // Vérif date
    if (!date_maint.isValid()) {
        QMessageBox::warning(this, "Erreur", "Date de maintenance invalide. Format attendu : jj/MM/aaaa");
        return;
    }
    if (date_maint > QDate::currentDate()) {
        QMessageBox::warning(this, "Erreur", "La date de maintenance ne peut pas être dans le futur.");
        return;
    }

    // Vérif doublon immat
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM SYSTEM.VEHICULE WHERE IMMAT = :IMMAT");
    checkQuery.bindValue(":IMMAT", immat);
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Erreur", "Un véhicule avec cette immatriculation existe déjà !");
        return;
    }

    // === AJOUT FINAL ===
    Vehicule v(immat, marque, modele, type, etat, service, date_maint);

    if (v.ajouter()) {
        QMessageBox::information(this, "Succès", "Véhicule ajouté avec succès !");
        loadVehicules();
        qDebug() << "✅ Véhicule ajouté :" << immat << marque << modele << etat;
    } else {
        QMessageBox::critical(this, "Erreur SQL", "Échec de l'ajout du véhicule !");
    }
}

void MainWindow::on_modifier_3_clicked()
{
    qDebug() << "🔎 [DEBUG] on_modifier_3_clicked() triggered";
    qDebug() << "🔎 [DEBUG] selectedImmat current value:" << selectedImmat;

    if (selectedImmat.isEmpty()) {
        qDebug() << "⚠️ [DEBUG] No vehicle selected before modifying!";
        QMessageBox::warning(this, "Erreur", "Veuillez d'abord sélectionner un véhicule à modifier.");
        return;
    }

    // Read current form values
    QString immat   = ui->immatline_2->text();
    QString marque  = ui->marqueline_2->text();
    QString modele  = ui->modeleline_2->text();
    QString type    = ui->triemail_2->currentText();
    QString etat    = ui->Etatline_2->currentText();
    QString service = ui->serviceline_2->text();
    QDate   date_maint = QDate::fromString(ui->datemaintline_2->text(), "dd/MM/yyyy");

    qDebug() << "✅ [DEBUG] Form values:"
             << "Immat:" << immat
             << "| Marque:" << marque
             << "| Modele:" << modele
             << "| Type:" << type
             << "| Etat:" << etat
             << "| Service:" << service
             << "| Date:" << date_maint.toString("dd/MM/yyyy");

    Vehicule v(immat, marque, modele, type, etat, service, date_maint);

    bool success = v.modifier(selectedImmat);
    qDebug() << "🧩 [DEBUG] modifier() called with oldImmat =" << selectedImmat
             << "| result =" << success;

    if (success) {
        // ✅ Instant UI update of the selected row (no full reload needed)
        int row = ui->tableau_3->currentRow();
        if (row >= 0 && row < ui->tableau_3->rowCount()) {
            // Assuming columns: 0=ID (hidden), 1=IMMAT, 2=MARQUE, 3=MODELE, 4=TYPE, 5=ETAT, 6=SERVICE, 7=DATE
            if (ui->tableau_3->columnCount() >= 8) {
                ui->tableau_3->item(row, 1)->setText(immat);
                ui->tableau_3->item(row, 2)->setText(marque);
                ui->tableau_3->item(row, 3)->setText(modele);
                ui->tableau_3->item(row, 4)->setText(type);
                ui->tableau_3->item(row, 5)->setText(etat);
                ui->tableau_3->item(row, 6)->setText(service);
                ui->tableau_3->item(row, 7)->setText(date_maint.toString("dd/MM/yyyy"));
                qDebug() << "✅ [DEBUG] UI row updated instantly at row" << row;
            } else {
                qDebug() << "⚠️ [DEBUG] Column count is less than expected; skipping instant UI update.";
            }
        } else {
            qDebug() << "⚠️ [DEBUG] currentRow() invalid; skipping instant UI update.";
        }

        // And also refresh from DB to stay 100% in sync (optional but safe)
        loadVehicules();

        QMessageBox::information(this, "Succès", "Véhicule modifié avec succès !");
        selectedImmat.clear();  // reset after update
    } else {
        QMessageBox::warning(this, "Erreur", "Échec de la modification du véhicule !");
    }
}

void MainWindow::on_supprimer_3_clicked()
{
    // Example: delete selected row from database and refresh table
    int row = ui->tableau_3->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Suppression", "Veuillez sélectionner un véhicule à supprimer.");
        return;
    }

    QString immat = ui->tableau_3->item(row, 1)->text(); // Assuming first column is immat
    Vehicule v;
    bool success = v.supprimer(immat); // Use your Vehicule::supprimer(QString) function

    if (success) {
        ui->tableau_3->removeRow(row);
        QMessageBox::information(this, "Suppression", "Véhicule supprimé avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "La suppression a échoué !");
    }
}


void MainWindow::on_exporter_3_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";

    QPdfWriter pdf(fileName);
    pdf.setPageSize(QPageSize::A4);
    pdf.setResolution(300);

    QPainter painter(&pdf);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- TITRE ---
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(0, 120, pdf.width(), 100, Qt::AlignCenter, "Liste des Véhicules");

    // --- MARGES ET DIMENSIONS ---
    int leftMargin = 80;
    int rightMargin = 80;
    int topMargin = 260;
    int headerHeight = 100;   // plus haut qu’avant
    int rowHeight = 100;      // lignes plus grandes
    int colCount = ui->tableau_3->columnCount();
    int rowCount = ui->tableau_3->rowCount();
    int tableWidth = pdf.width() - (leftMargin + rightMargin);

    // --- LARGEUR DES COLONNES (proportionnelle) ---
    QVector<int> colWeights = {8, 18, 15, 15, 10, 10, 12, 20}; // dernière colonne plus large
    int totalWeight = 0;
    for (int w : colWeights) totalWeight += w;

    QVector<int> colWidths;
    for (int i = 0; i < colCount; ++i)
        colWidths.append((tableWidth * colWeights[i]) / totalWeight);

    // --- EN-TÊTES ---
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setBrush(QColor(210, 210, 210));
    painter.setPen(Qt::black);

    int x = leftMargin;
    painter.drawRect(leftMargin, topMargin, tableWidth, headerHeight);

    for (int col = 0; col < colCount; ++col) {
        QString headerText = ui->tableau_3->horizontalHeaderItem(col)->text();

        // Forcer une ligne multiple si nécessaire
        if (headerText.contains("Date de maintenance", Qt::CaseInsensitive))
            headerText = "Date de\nmaintenance";

        QRect headerRect(x, topMargin, colWidths[col], headerHeight);
        painter.drawRect(headerRect);
        painter.drawText(headerRect.adjusted(4, 6, -4, -4),
                         Qt::AlignCenter | Qt::TextWordWrap,
                         headerText);
        x += colWidths[col];
    }

    // --- CONTENU DU TABLEAU ---
    painter.setFont(QFont("Arial", 11));
    painter.setBrush(Qt::NoBrush);
    int y = topMargin + headerHeight;

    for (int row = 0; row < rowCount; ++row) {
        x = leftMargin;
        for (int col = 0; col < colCount; ++col) {
            QRect cellRect(x, y, colWidths[col], rowHeight);
            painter.drawRect(cellRect);

            QTableWidgetItem *item = ui->tableau_3->item(row, col);
            QString text = item ? item->text() : "";

            // Alignement centré verticalement avec un léger padding horizontal
            painter.drawText(cellRect.adjusted(10, 0, -10, 0),
                             Qt::AlignVCenter | Qt::AlignLeft,
                             text);
            x += colWidths[col];
        }
        y += rowHeight;
    }

    painter.end();
    QMessageBox::information(this, "Export PDF", "Le tableau a été exporté avec succès !");
}

void MainWindow::loadVehicules()
{
    ui->tableau_3->clear();
    ui->tableau_3->setRowCount(0);
    ui->tableau_3->setColumnCount(9); // +1 colonne cachée

    QStringList headers;
    headers << "ID" << "Immatriculation" << "Marque" << "Modèle"
            << "Type" << "État" << "Service" << "Date de maintenance" << "ORDER_DATE";

    ui->tableau_3->setHorizontalHeaderLabels(headers);

    QSqlQuery q;
    if (!q.exec("SELECT ID_VEHI, IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, "
                "TO_CHAR(DATE_MAINT, 'DD/MM/YYYY') AS DATE_FR, "
                "TO_CHAR(DATE_MAINT, 'YYYY-MM-DD') AS DATE_ORDER "
                "FROM SYSTEM.VEHICULE"))
    {
        QMessageBox::critical(this, "Erreur SQL", q.lastError().text());
        return;
    }

    int row = 0;
    while (q.next()) {
        ui->tableau_3->insertRow(row);

        // remplit les colonnes visibles
        for (int col = 0; col < 8; ++col)
            ui->tableau_3->setItem(row, col, new QTableWidgetItem(q.value(col).toString()));

        // colonne cachée ORDER_DATE
        ui->tableau_3->setItem(row, 8, new QTableWidgetItem(q.value(8).toString()));

        row++;
    }

    ui->tableau_3->setColumnHidden(0, true);  // cacher ID
    ui->tableau_3->setColumnHidden(8, true);  // cacher ORDER_DATE
    ui->tableau_3->resizeColumnsToContents();
}
QChartView* MainWindow::createVehiculePieChart()
{
    // 🔹 Create the data series
    QPieSeries* series = new QPieSeries();

    // 🔹 Fetch data from the database (STAT PAR ÉTAT)
    QSqlQuery query;
    if (!query.exec("SELECT ETAT, COUNT(*) FROM SYSTEM.VEHICULE GROUP BY ETAT")) {
        qDebug() << "❌ SQL Error (stats ETAT):" << query.lastError().text();
        return nullptr;
    }

    int total = 0;
    QMap<QString, int> data;
    while (query.next()) {
        QString etat = query.value(0).toString();
        int count = query.value(1).toInt();
        data[etat] = count;
        total += count;
    }

    // 🔹 Create slices dynamically
    for (auto it = data.begin(); it != data.end(); ++it) {
        double perc = (total > 0) ? (double(it.value()) / total) * 100.0 : 0.0;
        QString label = QString("%1 - %2 véhicules (%3%)")
                            .arg(it.key())
                            .arg(it.value())
                            .arg(QString::number(perc, 'f', 1));

        QPieSlice* slice = series->append(label, it.value());
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
        slice->setLabelFont(QFont("Arial", 10, QFont::Bold));

        QColor color = QColor::fromHsv(
            QRandomGenerator::global()->bounded(360),
            200,
            250
            );
        slice->setBrush(color);

        QObject::connect(slice, &QPieSlice::hovered, [slice](bool hovered) {
            if (hovered) {
                slice->setExploded(true);
                slice->setExplodeDistanceFactor(0.15);
                slice->setLabelFont(QFont("Arial", 11, QFont::Bold));
            } else {
                slice->setExploded(false);
                slice->setLabelFont(QFont("Arial", 10, QFont::Normal));
            }
        });
    }

    // 🔹 Create chart
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des véhicules par état (%)");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 9));
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setTheme(QChart::ChartThemeLight);

    // 🔹 Create chart view
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 🔹 Create dialog container
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Statistiques des véhicules par état");
    dialog->resize(800, 600);
    dialog->setModal(true);

    QPushButton* closeButton = new QPushButton("Fermer");
    closeButton->setFixedWidth(120);
    closeButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; border-radius: 8px; padding: 6px; }"
        "QPushButton:hover { background-color: #d32f2f; }"
        );

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    QObject::connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    QRect parentRect = this->geometry();
    int x = parentRect.center().x() - dialog->width() / 2;
    int y = parentRect.center().y() - dialog->height() / 2;
    dialog->move(x, y);

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();

    return nullptr;
}

void MainWindow::on_pushButton_3_clicked()
{
    createVehiculePieChart();  // ✅ just call it, no need for message box
}

void MainWindow::onTriBoxChanged(const QString &text)
{
    qDebug() << "🔄 [DEBUG] Tri demandé :" << text;

    ui->tableau_3->setSortingEnabled(true);

    if (text == "A-Z") {
        qDebug() << "🔠 [DEBUG] Tri A-Z sur colonne 'Marque' (index 2)";
        ui->tableau_3->sortItems(2, Qt::AscendingOrder);
    }
    else if (text == "Z-A") {
        qDebug() << "🔠 [DEBUG] Tri Z-A sur colonne 'Marque' (index 2)";
        ui->tableau_3->sortItems(2, Qt::DescendingOrder);
    }
    else if (text == "Date croissant") {
        qDebug() << "📅 [DEBUG] Tri DATE croissant (ancien -> récent) sur ORDER_DATE (index 8)";
        ui->tableau_3->sortItems(8, Qt::AscendingOrder);
    }
    else if (text == "date decroissant") {
        qDebug() << "📅 [DEBUG] Tri DATE décroissant (récent -> ancien) sur ORDER_DATE (index 8)";
        ui->tableau_3->sortItems(8, Qt::DescendingOrder);
    }
    else {
        qDebug() << "⚠️ [DEBUG] Option inconnue :" << text;
    }

    // Affiche la première valeur de date pour vérifier l'ordre
    if (ui->tableau_3->rowCount() > 0) {
        QString firstDate = ui->tableau_3->item(0, 8)->text();
        qDebug() << "📌 [DEBUG] Première valeur ORDER_DATE après tri =" << firstDate;
    } else {
        qDebug() << "⚠️ [DEBUG] Tableau vide !";
    }
}

void MainWindow::on_btnTriDate_clicked()
{
    if (!ui || !ui->tableau_3) return;
    ui->tableau_3->setSortingEnabled(true);
    ui->tableau_3->sortItems(8, triCroissant ? Qt::AscendingOrder : Qt::DescendingOrder);
    triCroissant = !triCroissant;
}

// Recherche/tri véhicules (implémentations manquantes)
void MainWindow::onRechercheVehiculeChanged()
{
    // No-op sûr: la recherche peut être gérée ailleurs
}

void MainWindow::onTriTypeChanged()
{
    if (ui && ui->tableau_3)
        ui->tableau_3->sortItems(4, Qt::AscendingOrder);
}

void MainWindow::onTriEtatChanged()
{
    if (ui && ui->tableau_3)
        ui->tableau_3->sortItems(5, Qt::AscendingOrder);
}




void MainWindow::on_btnSendChat_clicked()

{
    QString userMessage = ui->chatInput->text().trimmed();
    if (userMessage.isEmpty())
        return;

    ui->chatHistory->append("<b>Vous :</b> " + userMessage);
    ui->chatInput->clear();

    sendMessageToAzureAI(userMessage);
}
void MainWindow::sendMessageToAzureAI(const QString &message)
{
    qDebug() << "🚀 sendMessageToAzureAI() démarre";
    //preparation des informations de connextion azur
    QString endpoint = "https://ai-kassem.cognitiveservices.azure.com"; // url de service
    QString apiKey = "";  // placeholder
    QString apiVersion = "2024-12-01-preview";
    QString model = "gpt-4o";

    QString url = endpoint + "/openai/deployments/" + model + "/chat/completions?api-version=" + apiVersion;

    qDebug() << "🌍 URL Azure =" << url;
    //creaton de la requette HTTP
    QNetworkRequest request{ QUrl(url) };
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("api-key", apiKey.toUtf8());

    // role de l’IA
    QString systemPrompt = R"(
Tu es SmartHelp, assistant SmartCity. Réponds toujours en texte simple, sans markdown, en maximum 3 lignes. Donne des explications claires et courtes sur l’utilisation des fonctions, champs, erreurs ou actions dans l’application.
Ton rôle est d’aider l’utilisateur à comprendre et utiliser les fonctionnalités de l’application de manière simple, claire et utile.

Voici ce que tu dois faire :

1. Expliquer les modules et fonctionnalités de l’application SmartCity, notamment :
   • Gestion des véhicules (ajouter, modifier, supprimer, rechercher)
   • Gestion des résidents
   • Gestion des employés
   • Gestion des incidents et maintenance
   • Historique, logs et états des objets
   • Interaction avec l’interface Qt : boutons, champs, tables, filtres

2. Fournir des explications détaillées sur chaque champ d’un formulaire :
   • immatriculation
   • marque
   • modèle
   • type
   • état
   • service
   • date
   • identifiants liés (ID_RES, ID_EMP, etc.)

3. Guider l’utilisateur dans les actions :
   • comment ajouter un véhicule
   • comment modifier un véhicule
   • comment supprimer un véhicule
   • comment rechercher un véhicule
   • comment éviter les erreurs

4. Décrire les erreurs possibles :
   • champ vide
   • format incorrect
   • date invalide (ex : date future)
   • doublon d’immatriculation
   • ID inexistant
   • échec de connexion à la base Oracle

5. Toujours répondre :
   • poliment
   • clairement
   • avec des étapes simples
   • en donnant des exemples quand nécessaire

6. Ne pas inventer des fonctionnalités inexistantes. Si ce n'est pas dans SmartCity, tu expliques calmement que ce n’est pas prévu.

Ton objectif : être un assistant intégré, très utile, qui explique l’utilisation de l’application SmartCity comme si tu étais un guide interactif.
)";

    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = systemPrompt;

    // --- JSON ---
    QJsonObject userMsg;

    userMsg["role"] = "user";
    userMsg["content"] = message;

    QJsonArray arr;
    arr.append(systemMsg);
    arr.append(userMsg);

    QJsonObject payload;
    payload["messages"] = arr;
    payload["temperature"] = 0.7;

    QByteArray jsonData = QJsonDocument(payload).toJson();

    qDebug() << "📦 Payload =" << jsonData;

    // --- SEND WITH CRASH DETECTION ---
    QNetworkReply *reply = nullptr;

    try {
        reply = networkManager->post(request, jsonData);
    }
    catch(...) {
        qDebug() << "💥 EXCEPTION : Crash pendant POST() !";
        return;
    }

    connect(reply, &QNetworkReply::errorOccurred,
            [](QNetworkReply::NetworkError code){
                qDebug() << "❌ ERROR immediate =" << code;
            });

    connect(reply, &QNetworkReply::finished, [this, reply]() {

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "❌ ERROR :" << reply->errorString();
            ui->chatHistory->append("<b>Bot :</b> Erreur : " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        qDebug() << "📩 Réponse =" << data;

        QJsonDocument json = QJsonDocument::fromJson(data);
        QString bot = json["choices"][0]["message"]["content"].toString();

        ui->chatHistory->append("<b>Bot :</b> " + bot);

        reply->deleteLater();
    });
}
QString MainWindow::processChatMessage(const QString &msg)
{
    QString m = msg.toLower();

    // --- Aide sur l'ajout ---
    if (m.contains("ajouter") && m.contains("vehicule"))
        return "Pour ajouter un véhicule : remplissez les champs à gauche puis cliquez sur 'Ajouter'.";

    // --- Aide sur la modification ---
    if (m.contains("modifier") || m.contains("update"))
        return "Pour modifier : sélectionnez un véhicule dans le tableau puis cliquez sur 'Modifier'.";

    // --- Aide sur la suppression ---
    if (m.contains("supprimer") || m.contains("delete"))
        return "Pour supprimer : sélectionnez un véhicule puis cliquez sur 'Supprimer'.";

    // --- Aide sur la recherche ---
    if (m.contains("recherche") || m.contains("chercher") || m.contains("filtrer"))
        return "Tapez une immatriculation dans la barre de recherche pour filtrer les résultats.";

    // --- Aide sur l'état ---
    if (m.contains("etat") || m.contains("panne") || m.contains("usé") || m.contains("neuf"))
        return "Les états disponibles sont : Neuf, Usé, En panne.";

    // --- Aide sur la date de maintenance ---
    if (m.contains("date") || m.contains("maintenance") || m.contains("maint"))
        return "La date de maintenance doit être dans le passé. Vous pouvez trier par date via le bouton 'Tri par date'.";

    // --- Aide sur le tri ---
    if (m.contains("tri") || m.contains("trier"))
        return "Le bouton 'Tri par date' permet de trier les véhicules du plus ancien au plus récent et inversement.";

    // --- Message par défaut ---
    return "Je n’ai pas compris 😅. Essayez avec des mots comme : ajouter véhicule, modifier, supprimer, tri, recherche, date maintenance.";
}

QString MainWindow::buildMaintenancePromptFromCurrentVehicle() const
{
    QString immat   = ui->immatline_2->text();
    QString marque  = ui->marqueline_2->text();
    QString modele  = ui->modeleline_2->text();
    QString type    = ui->triemail_2->currentText();
    QString etat    = ui->Etatline_2->currentText();
    QString service = ui->serviceline_2->text();
    QDate   date_maint = QDate::fromString(ui->datemaintline_2->text(), "dd/MM/yyyy");

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

void MainWindow::sendRecommendationToAzureAI(const QString &message)
{
    qDebug() << "🚀 sendRecommendationToAzureAI() démarre";

    QString endpoint   = "https://ai-kassem.cognitiveservices.azure.com";
    QString apiKey     = "445NLYUwthBdj5EbFvxbCxV2XSdJWKYartumAOvqEFMtKEofmdpuJQQJ99BGACfhMk5XJ3w3AAAAACOGRpAA"; // ⚠️ garde ton vrai key en privé dans ton projet
    QString apiVersion = "2024-12-01-preview";
    QString model      = "gpt-4o";

    QString url = endpoint
                  + "/openai/deployments/"
                  + model
                  + "/chat/completions?api-version="
                  + apiVersion;

    qDebug() << "🌍 URL Azure =" << url;

    QNetworkRequest request{ QUrl(url) };
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("api-key", apiKey.toUtf8());

    // SYSTEM MESSAGE (spécial maintenance, max 3 lignes, texte simple)
    QString systemPrompt = R"(Tu es SmartHelp, assistant SmartCity.
Tu donnes des recommandations de maintenance pour les véhicules.
Réponds toujours en français, en texte simple, sans markdown, en maximum 10 lignes.)";

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = systemPrompt;

    // USER MESSAGE (les infos du véhicule + consigne déjà incluses dans message)
    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = message;

    QJsonArray arr;
    arr.append(systemMsg);
    arr.append(userMsg);

    QJsonObject payload;
    payload["messages"]    = arr;
    payload["temperature"] = 0.4; // un peu plus sérieux pour de la maintenance

    QByteArray jsonData = QJsonDocument(payload).toJson();
    qDebug() << "📦 Payload =" << jsonData;

    QNetworkReply *reply = nullptr;

    try {
        reply = networkManager->post(request, jsonData);
    }
    catch (...) {
        qDebug() << "💥 EXCEPTION : Crash pendant POST() !";
        return;
    }

    connect(reply, &QNetworkReply::errorOccurred,
            [](QNetworkReply::NetworkError code){
                qDebug() << "❌ ERROR immediate =" << code;
            });

    connect(reply, &QNetworkReply::finished, [this, reply]() {

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "❌ ERROR :" << reply->errorString();
            ui->textRecom->setPlainText("Erreur Azure : " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        qDebug() << "📩 Réponse =" << data;

        QJsonDocument json = QJsonDocument::fromJson(data);
        QString bot = json["choices"][0]["message"]["content"].toString();

        // 📝 Afficher la reco dans la zone de texte
        ui->textRecom->setPlainText(bot.trimmed());

        reply->deleteLater();
    });
}
void MainWindow::on_btnRecom_clicked()
{
    qDebug() << "🔎 [DEBUG] on_btnRecom_clicked() triggered";
    qDebug() << "🔎 [DEBUG] selectedImmat current value:" << selectedImmat;

    if (selectedImmat.isEmpty()) {
        qDebug() << "⚠️ [DEBUG] No vehicle selected before recommendation!";
        QMessageBox::warning(this,
                             "Erreur",
                             "Veuillez d'abord sélectionner un véhicule avant de demander une recommandation.");
        return;
    }

    // Construire le message pour l'IA à partir du véhicule courant
    QString prompt = buildMaintenancePromptFromCurrentVehicle();
    qDebug() << "📨 [DEBUG] Maintenance prompt envoyé à Azure :" << prompt;

    // Optionnel : nettoyer la zone avant la réponse
    ui->textRecom->clear();
    ui->textRecom->setPlainText("Génération de la recommandation en cours...");

    // Lancer l'appel Azure
    sendRecommendationToAzureAI(prompt);

    // Afficher la page de recommandation
    ui->stackedWidget_3->setCurrentWidget(ui->page_11);
}
// ========== MAISON CRUD IMPLEMENTATIONS ==========

void MainWindow::loadMaisons()
{
    Maison::afficher(ui->tableau_7);
}

void MainWindow::onAjouterMaison()
{
    Connection c; c.reconnect(); c.createOrPatchTableMaisons();
    // Read form values
    QString adresse = ui->adresseline_2->text().trimmed();
    QString securite = ui->nivsecline->text().trimmed();
    QString statut = ui->statuschoix->currentText().trimmed();
    // Get text from ComboBox (use lineEdit if editable, otherwise currentText)
    QString type = ui->typechoix_2->isEditable() && ui->typechoix_2->lineEdit() 
                   ? ui->typechoix_2->lineEdit()->text().trimmed() 
                   : ui->typechoix_2->currentText().trimmed();
    int nbr_pieces = ui->nbrpieceschoix->currentText().toInt();
    
    // Debug: Afficher les valeurs lues depuis l'UI
    qDebug() << "🔍 [DEBUG] Valeurs lues depuis l'UI:";
    qDebug() << "  - Adresse:" << adresse;
    qDebug() << "  - Sécurité:" << securite;
    qDebug() << "  - Statut:" << statut;
    qDebug() << "  - Type:" << type;
    qDebug() << "  - Nbr pièces:" << nbr_pieces;

    // Validation
    QString error;
    if (!Maison::validateAdresse(adresse, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }
    if (!Maison::validateSecurite(securite, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }
    if (!Maison::validateStatut(statut, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }
    if (!Maison::validateType(type, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }
    if (!Maison::validateNbrPieces(nbr_pieces, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }

    // Read ID if provided (optional)
    QString idStr = ui->idmaisonline->text().trimmed();
    int id_maison = -1; // -1 = auto-generate
    if (!idStr.isEmpty()) {
        bool ok;
        id_maison = idStr.toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, tr("Erreur"), tr("ID invalide. L'ID sera généré automatiquement."));
            id_maison = -1;
        }
    }
    
    // Create Maison object and add
    Maison maison(adresse, statut, securite, nbr_pieces, type);
    QString errorMsg;
    
    if (maison.ajouter(id_maison, &errorMsg)) {
        QMessageBox::information(this, tr("Succès"), tr("Maison ajoutée avec succès."));
        QSqlQuery verify(QSqlDatabase::database("qt_oracle"));
        verify.prepare("SELECT ADRESSE FROM GEST_MAISON WHERE ID=:id");
        verify.bindValue(":id", maison.getId());
        if (verify.exec() && verify.next()) {
            qDebug() << "Adresse insérée:" << verify.value(0).toString();
        } else {
            qDebug() << "Vérification adresse échouée:" << verify.lastError().text();
        }
        // Clear input fields
        ui->idmaisonline->clear();
        ui->adresseline_2->clear();
        ui->nivsecline->clear();
        ui->statuschoix->setCurrentIndex(-1); // Deselect instead of index 0
        ui->typechoix_2->setCurrentIndex(-1); // Deselect instead of index 0 (which is empty)
        ui->nbrpieceschoix->setCurrentIndex(-1); // Deselect instead of index 0
        // Reload table
        loadMaisons();
    } else {
        if (errorMsg.isEmpty()) {
            errorMsg = tr("Vérifiez la console pour plus de détails.");
        }
        QMessageBox::warning(this, tr("Erreur"), errorMsg);
    }
}

void MainWindow::onModifierMaison()
{
    // Get ID from input
    QString idStr = ui->idmaisonline->text().trimmed();
    if (idStr.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner une maison à modifier."));
        return;
    }
    
    bool ok;
    int newId = idStr.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Erreur"), tr("ID invalide."));
        return;
    }
    
    // Determine old ID for WHERE clause:
    // - If selectedMaisonId is valid (>0), user selected from table, use it as oldId
    // - If selectedMaisonId is invalid but ID in field exists in DB, use it as oldId
    // - This allows modification even if user manually changed the ID in the field
    int oldId = -1;
    if (selectedMaisonId > 0) {
        // User selected from table, use the original selected ID as oldId
        oldId = selectedMaisonId;
        qDebug() << "✅ [DEBUG] Utilisation de selectedMaisonId comme oldId:" << oldId;
    } else {
        // No selection from table, check if the ID in field exists
        if (Maison::idExists(newId)) {
            oldId = newId;
            qDebug() << "⚠️ [DEBUG] Pas de sélection, mais ID du champ existe. Utilisation comme oldId:" << oldId;
        } else {
            // ID doesn't exist, can't modify
            QMessageBox::warning(this, tr("Erreur"), 
                                 tr("L'ID %1 n'existe pas dans la base de données.\nVeuillez sélectionner une maison existante depuis le tableau.").arg(newId));
            return;
        }
    }

    // Read form values
    QString adresse = ui->adresseline_2->text().trimmed();
    QString securite = ui->nivsecline->text().trimmed();
    QString statut = ui->statuschoix->currentText().trimmed();
    // Get text from ComboBox (use lineEdit if editable, otherwise currentText)
    QString type = ui->typechoix_2->isEditable() && ui->typechoix_2->lineEdit() 
                   ? ui->typechoix_2->lineEdit()->text().trimmed() 
                   : ui->typechoix_2->currentText().trimmed();
    int nbr_pieces = ui->nbrpieceschoix->currentText().toInt();
    
    // Debug: Afficher les valeurs lues depuis l'UI
    qDebug() << "🔍 [DEBUG MODIFIER] Valeurs lues depuis l'UI:";
    qDebug() << "  - Ancien ID (WHERE):" << oldId;
    qDebug() << "  - Nouvel ID:" << newId;
    qDebug() << "  - Adresse:" << adresse;
    qDebug() << "  - Sécurité:" << securite;
    qDebug() << "  - Statut:" << statut;
    qDebug() << "  - Type:" << type;
    qDebug() << "  - Nbr pièces:" << nbr_pieces;

    // Validation
    QString error;
    if (!Maison::validateAdresse(adresse, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }
    if (!Maison::validateSecurite(securite, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }
    if (!Maison::validateStatut(statut, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }
    if (!Maison::validateType(type, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }
    if (!Maison::validateNbrPieces(nbr_pieces, error)) {
        QMessageBox::warning(this, tr("Erreur de validation"), error);
        return;
    }

    // Create Maison object and update
    Maison maison(adresse, statut, securite, nbr_pieces, type);
    QString errorMsg;
    
    // Use old ID in WHERE clause, and new ID if changed
    int idToUse = (newId != oldId) ? newId : -1; // -1 = keep old ID
    if (maison.modifier(oldId, idToUse, &errorMsg)) {
        QMessageBox::information(this, tr("Succès"), tr("Maison modifiée avec succès."));
        // Clear input fields
        ui->idmaisonline->clear();
        ui->adresseline_2->clear();
        ui->nivsecline->clear();
        ui->statuschoix->setCurrentIndex(-1);
        ui->typechoix_2->setCurrentIndex(-1);
        ui->nbrpieceschoix->setCurrentIndex(-1);
        selectedMaisonId = -1; // Reset selection
        // Reload table
        loadMaisons();
    } else {
        if (errorMsg.isEmpty()) {
            errorMsg = tr("Vérifiez la console pour plus de détails.");
        }
        QMessageBox::warning(this, tr("Erreur"), errorMsg);
    }
}

void MainWindow::onSupprimerMaison()
{
    Connection c; if (!c.reconnect()) { QMessageBox::warning(this, tr("Erreur DB"), tr("Connexion base de données indisponible.")); return; }
    QString idStr = ui->idmaisonline->text().trimmed();
    int id = -1; bool ok = false;
    if (!idStr.isEmpty()) { id = idStr.toInt(&ok); }
    if (!ok || id <= 0) {
        if (selectedMaisonId > 0) { id = selectedMaisonId; ok = true; }
    }
    if (!ok || id <= 0) { QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner une maison valide à supprimer.")); return; }

    // Confirmation
    int ret = QMessageBox::question(this, tr("Confirmer"), tr("Supprimer la maison?"),
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

    Maison maison;
    QString errorMsg;
    if (maison.supprimer(id, &errorMsg)) {
        QMessageBox::information(this, tr("Succès"), tr("Maison supprimée."));
        // Clear input fields
        ui->idmaisonline->clear();
        ui->adresseline_2->clear();
        ui->nivsecline->clear();
        ui->statuschoix->setCurrentIndex(-1);
        ui->typechoix_2->setCurrentIndex(-1);
        ui->nbrpieceschoix->setCurrentIndex(-1);
        // Reload table
        loadMaisons();
    } else {
        if (errorMsg.isEmpty()) { errorMsg = tr("Vérifiez la console pour plus de détails."); }
        QSqlQuery verify(QSqlDatabase::database("qt_oracle"));
        verify.prepare("SELECT 1 FROM GEST_MAISON WHERE ID=:id");
        verify.bindValue(":id", id);
        if (verify.exec() && !verify.next()) {
            QMessageBox::information(this, tr("Info"), tr("La ligne n'existe plus."));
            loadMaisons();
            return;
        }
        QMessageBox::warning(this, tr("Erreur"), errorMsg);
    }
}

void MainWindow::onExporterMaison()
{
    if (ui->tableau_7->rowCount() == 0) {
        QMessageBox::warning(this, tr("Avertissement"), tr("Aucune donnée à exporter."));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";

    QPdfWriter pdf(fileName);
    pdf.setPageSize(QPageSize::A4);
    pdf.setResolution(300);

    QPainter painter(&pdf);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- TITRE ---
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(0, 120, pdf.width(), 100, Qt::AlignCenter, "Liste des Maisons");

    // --- MARGES ET DIMENSIONS ---
    int leftMargin = 80;
    int rightMargin = 80;
    int topMargin = 260;
    int headerHeight = 100;
    int rowHeight = 100;
    int colCount = ui->tableau_7->columnCount();
    int rowCount = ui->tableau_7->rowCount();
    int tableWidth = pdf.width() - (leftMargin + rightMargin);

    // --- LARGEUR DES COLONNES (proportionnelle) ---
    QVector<int> colWeights = {10, 25, 15, 15, 15, 20};
    int totalWeight = 0;
    for (int w : colWeights) totalWeight += w;

    QVector<int> colWidths;
    for (int i = 0; i < colCount && i < colWeights.size(); ++i)
        colWidths.append((tableWidth * colWeights[i]) / totalWeight);

    // --- EN-TÊTES ---
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setBrush(QColor(210, 210, 210));
    painter.setPen(Qt::black);
    int x = leftMargin;

    for (int col = 0; col < colCount; ++col) {
        QTableWidgetItem *headerItem = ui->tableau_7->horizontalHeaderItem(col);
        QString headerText = headerItem ? headerItem->text() : QString("Colonne %1").arg(col + 1);

        QRect headerRect(x, topMargin, colWidths[col], headerHeight);
        painter.drawRect(headerRect);
        painter.drawText(headerRect.adjusted(4, 6, -4, -4),
                         Qt::AlignCenter | Qt::TextWordWrap,
                         headerText);
        x += colWidths[col];
    }

    // --- CONTENU DU TABLEAU ---
    painter.setFont(QFont("Arial", 11));
    painter.setBrush(Qt::NoBrush);
    int y = topMargin + headerHeight;

    for (int row = 0; row < rowCount; ++row) {
        x = leftMargin;
        for (int col = 0; col < colCount && col < colWidths.size(); ++col) {
            QRect cellRect(x, y, colWidths[col], rowHeight);
            painter.drawRect(cellRect);
            QTableWidgetItem *item = ui->tableau_7->item(row, col);
            QString text = item ? item->text() : "";
            painter.drawText(cellRect.adjusted(10, 0, -10, 0),
                             Qt::AlignVCenter | Qt::AlignLeft,
                             text);
            x += colWidths[col];
        }
        y += rowHeight;
    }

    painter.end();
    QMessageBox::information(this, tr("Succès"), tr("Le tableau a été exporté avec succès."));
}

// ========== RESIDENT CRUD IMPLEMENTATIONS ==========

void MainWindow::loadResidents()
{
    QString err;
    QVector<Resident> residents = Resident::fetchAll(err);
    
    if (!err.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Erreur lors du chargement des résidents: %1").arg(err));
        return;
    }
    
    // Clear and setup table
    ui->tableau_5->clear();
    ui->tableau_5->setColumnCount(8);
    ui->tableau_5->setRowCount(residents.size());
    
    QStringList headers;
    headers << "ID resident" << "Nom & Prenom" << "Date naiss" << "Adresse" 
            << "Telephone" << "Email" << "Status" << "Situation familiale";
    ui->tableau_5->setHorizontalHeaderLabels(headers);
    
    // Fill table
    for (int i = 0; i < residents.size(); ++i) {
        const Resident &r = residents[i];
        ui->tableau_5->setItem(i, 0, new QTableWidgetItem(QString::number(r.idResident)));
        ui->tableau_5->setItem(i, 1, new QTableWidgetItem(r.nom + " " + r.prenom));
        ui->tableau_5->setItem(i, 2, new QTableWidgetItem(r.dtNaissance.toString("dd/MM/yyyy")));
        ui->tableau_5->setItem(i, 3, new QTableWidgetItem(r.adresse));
        ui->tableau_5->setItem(i, 4, new QTableWidgetItem(QString::number(r.telephone)));
        ui->tableau_5->setItem(i, 5, new QTableWidgetItem(r.email));
        ui->tableau_5->setItem(i, 6, new QTableWidgetItem(r.statut));
        ui->tableau_5->setItem(i, 7, new QTableWidgetItem(r.situationFamiliale));
    }
}

void MainWindow::onAjouterResident()
{
    // Read form values
    QString idStr = ui->idresidentline_2->text().trimmed();
    QString nomPrenom = ui->nomline_7->text().trimmed();
    QString dateStr = ui->datenaissanceline_2->text().trimmed();
    QString adresse = ui->adresseline_4->text().trimmed();
    QString telephoneStr = ui->telephoneline_5->text().trimmed();
    QString email = ui->emailline_4->text().trimmed();
    QString statut = ui->statusline_2->text().trimmed();
    QString situationFamiliale = ui->situationline_2->text().trimmed();
    
    // Parse nom and prenom (assuming format "Nom Prenom")
    QStringList nomPrenomParts = nomPrenom.split(" ", Qt::SkipEmptyParts);
    QString nom = nomPrenomParts.size() > 0 ? nomPrenomParts[0] : "";
    QString prenom = nomPrenomParts.size() > 1 ? nomPrenomParts.mid(1).join(" ") : "";
    
    // Parse date (sans validation stricte)
    QDate dtNaissance;
    if (!dateStr.isEmpty()) {
        dtNaissance = QDate::fromString(dateStr, "dd/MM/yyyy");
        if (!dtNaissance.isValid()) {
            dtNaissance = QDate::fromString(dateStr, "yyyy-MM-dd");
        }
        // Si toujours invalide, créer une date par défaut (1900-01-01) pour éviter les erreurs SQL
        if (!dtNaissance.isValid()) {
            dtNaissance = QDate(1900, 1, 1);
        }
    } else {
        // Date vide - utiliser une date par défaut
        dtNaissance = QDate(1900, 1, 1);
    }
    
    // Parse telephone (sans validation stricte)
    bool telOk;
    long long telephone = telephoneStr.toLongLong(&telOk);
    if (!telOk || telephone <= 0) {
        telephone = 0; // Valeur par défaut si invalide
    }
    
    // Parse ID (optional)
    int idResident = -1;
    if (!idStr.isEmpty()) {
        bool ok;
        idResident = idStr.toInt(&ok);
        if (!ok) {
            // ID invalide, sera généré automatiquement (pas besoin d'afficher un message)
            idResident = -1;
        }
    }
    
    // Create Resident object
    Resident r;
    r.idResident = idResident;
    r.nom = nom;
    r.prenom = prenom;
    r.dtNaissance = dtNaissance;
    r.adresse = adresse;
    r.telephone = telephone;
    r.email = email;
    r.statut = statut;
    r.situationFamiliale = situationFamiliale;
    
    // Insert with validation
    int newId = 0;
    QString errorMsg;
    if (r.insert(newId, errorMsg)) {
        // Clear input fields BEFORE showing success message to avoid validation triggers
        ui->idresidentline_2->clear();
        ui->nomline_7->clear();
        ui->datenaissanceline_2->clear();
        ui->adresseline_4->clear();
        ui->telephoneline_5->clear();
        ui->emailline_4->clear();
        ui->statusline_2->clear();
        ui->situationline_2->clear();
        
        // Show success message
        QMessageBox::information(this, tr("Succès"), tr("Résident ajouté avec succès.\nID: %1").arg(newId));
        
        // Reload table (silently, without showing errors)
        QString err;
        QVector<Resident> residents = Resident::fetchAll(err);
        if (!err.isEmpty()) {
            // Don't show error message after successful insert
            qDebug() << "Erreur lors du chargement (après ajout):" << err;
            return;
        }
        
        // Clear and setup table
        ui->tableau_5->clear();
        ui->tableau_5->setColumnCount(8);
        ui->tableau_5->setRowCount(residents.size());
        
        QStringList headers;
        headers << "ID resident" << "Nom & Prenom" << "Date naiss" << "Adresse" 
                << "Telephone" << "Email" << "Status" << "Situation familiale";
        ui->tableau_5->setHorizontalHeaderLabels(headers);
        
        // Fill table
        for (int i = 0; i < residents.size(); ++i) {
            const Resident &r = residents[i];
            ui->tableau_5->setItem(i, 0, new QTableWidgetItem(QString::number(r.idResident)));
            ui->tableau_5->setItem(i, 1, new QTableWidgetItem(r.nom + " " + r.prenom));
            ui->tableau_5->setItem(i, 2, new QTableWidgetItem(r.dtNaissance.toString("dd/MM/yyyy")));
            ui->tableau_5->setItem(i, 3, new QTableWidgetItem(r.adresse));
            ui->tableau_5->setItem(i, 4, new QTableWidgetItem(QString::number(r.telephone)));
            ui->tableau_5->setItem(i, 5, new QTableWidgetItem(r.email));
            ui->tableau_5->setItem(i, 6, new QTableWidgetItem(r.statut));
            ui->tableau_5->setItem(i, 7, new QTableWidgetItem(r.situationFamiliale));
        }
    } else {
        // Only show error if insert failed (not after success)
        QMessageBox::warning(this, tr("Erreur"), errorMsg);
    }
}

void MainWindow::onModifierResident()
{
    // Get ID from input
    QString idStr = ui->idresidentline_2->text().trimmed();
    if (idStr.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner un résident à modifier."));
        return;
    }
    
    bool ok;
    int newId = idStr.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Erreur"), tr("ID invalide."));
        return;
    }
    
    // Determine old ID for WHERE clause:
    // - If selectedResidentId is valid (>0), user selected from table, use it as oldId
    // - If selectedResidentId is invalid but ID in field exists in DB, use it as oldId
    // - This allows modification even if user manually changed the ID in the field
    int oldId = -1;
    if (selectedResidentId > 0) {
        // User selected from table, use the original selected ID as oldId
        oldId = selectedResidentId;
        qDebug() << "✅ [DEBUG] Utilisation de selectedResidentId comme oldId:" << oldId;
        
        // If user is trying to change the ID, check if the new ID already exists
        if (newId != oldId && newId > 0) {
            if (Resident::idExists(newId)) {
                QMessageBox::warning(this, tr("Erreur"), 
                                     tr("L'ID %1 existe déjà dans la base de données.\nVeuillez utiliser un autre ID.").arg(newId));
                return;
            }
        }
    } else {
        // No selection from table, check if the ID in field exists
        if (Resident::idExists(newId)) {
            oldId = newId;
            qDebug() << "⚠️ [DEBUG] Pas de sélection, mais ID du champ existe. Utilisation comme oldId:" << oldId;
        } else {
            // ID doesn't exist, can't modify
            QMessageBox::warning(this, tr("Erreur"), 
                                 tr("L'ID %1 n'existe pas dans la base de données.\nVeuillez sélectionner un résident existant depuis le tableau.").arg(newId));
            return;
        }
    }
    
    // Read form values
    QString nomPrenom = ui->nomline_7->text().trimmed();
    QString dateStr = ui->datenaissanceline_2->text().trimmed();
    QString adresse = ui->adresseline_4->text().trimmed();
    QString telephoneStr = ui->telephoneline_5->text().trimmed();
    QString email = ui->emailline_4->text().trimmed();
    QString statut = ui->statusline_2->text().trimmed();
    QString situationFamiliale = ui->situationline_2->text().trimmed();
    
    // Parse nom and prenom
    QStringList nomPrenomParts = nomPrenom.split(" ", Qt::SkipEmptyParts);
    QString nom = nomPrenomParts.size() > 0 ? nomPrenomParts[0] : "";
    QString prenom = nomPrenomParts.size() > 1 ? nomPrenomParts.mid(1).join(" ") : "";
    
    // Parse date (sans validation stricte)
    QDate dtNaissance;
    if (!dateStr.isEmpty()) {
        dtNaissance = QDate::fromString(dateStr, "dd/MM/yyyy");
        if (!dtNaissance.isValid()) {
            dtNaissance = QDate::fromString(dateStr, "yyyy-MM-dd");
        }
        // Si toujours invalide, créer une date par défaut (1900-01-01) pour éviter les erreurs SQL
        if (!dtNaissance.isValid()) {
            dtNaissance = QDate(1900, 1, 1);
        }
    } else {
        // Date vide - utiliser une date par défaut
        dtNaissance = QDate(1900, 1, 1);
    }
    
    // Parse telephone (sans validation stricte)
    bool telOk;
    long long telephone = telephoneStr.toLongLong(&telOk);
    if (!telOk || telephone <= 0) {
        telephone = 0; // Valeur par défaut si invalide
    }
    
    // Create Resident object
    Resident r;
    r.nom = nom;
    r.prenom = prenom;
    r.dtNaissance = dtNaissance;
    r.adresse = adresse;
    r.telephone = telephone;
    r.email = email;
    r.statut = statut;
    r.situationFamiliale = situationFamiliale;
    
    // Update with validation
    // Use old ID in WHERE clause, and new ID if changed
    int idToUse = (newId != oldId) ? newId : -1; // -1 = keep old ID
    QString errorMsg;
    if (r.updateById(oldId, idToUse, &errorMsg)) {
        // Clear input fields BEFORE showing success message
        ui->idresidentline_2->clear();
        ui->nomline_7->clear();
        ui->datenaissanceline_2->clear();
        ui->adresseline_4->clear();
        ui->telephoneline_5->clear();
        ui->emailline_4->clear();
        ui->statusline_2->clear();
        ui->situationline_2->clear();
        selectedResidentId = -1; // Reset selection
        
        // Show success message
        QMessageBox::information(this, tr("Succès"), tr("Résident modifié avec succès."));
        
        // Reload table (silently, without showing errors)
        QString err;
        QVector<Resident> residents = Resident::fetchAll(err);
        if (!err.isEmpty()) {
            qDebug() << "Erreur lors du chargement (après modification):" << err;
            return;
        }
        
        // Clear and setup table
        ui->tableau_5->clear();
        ui->tableau_5->setColumnCount(8);
        ui->tableau_5->setRowCount(residents.size());
        
        QStringList headers;
        headers << "ID resident" << "Nom & Prenom" << "Date naiss" << "Adresse" 
                << "Telephone" << "Email" << "Status" << "Situation familiale";
        ui->tableau_5->setHorizontalHeaderLabels(headers);
        
        // Fill table
        for (int i = 0; i < residents.size(); ++i) {
            const Resident &r = residents[i];
            ui->tableau_5->setItem(i, 0, new QTableWidgetItem(QString::number(r.idResident)));
            ui->tableau_5->setItem(i, 1, new QTableWidgetItem(r.nom + " " + r.prenom));
            ui->tableau_5->setItem(i, 2, new QTableWidgetItem(r.dtNaissance.toString("dd/MM/yyyy")));
            ui->tableau_5->setItem(i, 3, new QTableWidgetItem(r.adresse));
            ui->tableau_5->setItem(i, 4, new QTableWidgetItem(QString::number(r.telephone)));
            ui->tableau_5->setItem(i, 5, new QTableWidgetItem(r.email));
            ui->tableau_5->setItem(i, 6, new QTableWidgetItem(r.statut));
            ui->tableau_5->setItem(i, 7, new QTableWidgetItem(r.situationFamiliale));
        }
    } else {
        QMessageBox::warning(this, tr("Erreur"), errorMsg);
    }
}

void MainWindow::onSupprimerResident()
{
    // Get ID from input or selected row
    int id = -1;
    if (ui->tableau_5->currentRow() >= 0) {
        QTableWidgetItem *it = ui->tableau_5->item(ui->tableau_5->currentRow(), 0);
        if (it) id = it->text().toInt();
    }
    
    if (id <= 0) {
        QString idStr = ui->idresidentline_2->text().trimmed();
        bool ok;
        id = idStr.toInt(&ok);
        if (!ok || id <= 0) {
            QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner un résident à supprimer."));
            return;
        }
    }
    
    if (QMessageBox::question(this, tr("Confirmer"), tr("Supprimer le résident %1?").arg(id)) != QMessageBox::Yes) {
        return;
    }
    
    QString errorMsg;
    if (Resident::removeById(id, errorMsg)) {
        // Clear input fields BEFORE showing success message
        ui->idresidentline_2->clear();
        ui->nomline_7->clear();
        ui->datenaissanceline_2->clear();
        ui->adresseline_4->clear();
        ui->telephoneline_5->clear();
        ui->emailline_4->clear();
        ui->statusline_2->clear();
        ui->situationline_2->clear();
        selectedResidentId = -1;
        
        // Show success message
        QMessageBox::information(this, tr("Succès"), tr("Résident supprimé."));
        
        // Reload table (silently, without showing errors)
        QString err;
        QVector<Resident> residents = Resident::fetchAll(err);
        if (!err.isEmpty()) {
            qDebug() << "Erreur lors du chargement (après suppression):" << err;
            return;
        }
        
        // Clear and setup table
        ui->tableau_5->clear();
        ui->tableau_5->setColumnCount(8);
        ui->tableau_5->setRowCount(residents.size());
        
        QStringList headers;
        headers << "ID resident" << "Nom & Prenom" << "Date naiss" << "Adresse" 
                << "Telephone" << "Email" << "Status" << "Situation familiale";
        ui->tableau_5->setHorizontalHeaderLabels(headers);
        
        // Fill table
        for (int i = 0; i < residents.size(); ++i) {
            const Resident &r = residents[i];
            ui->tableau_5->setItem(i, 0, new QTableWidgetItem(QString::number(r.idResident)));
            ui->tableau_5->setItem(i, 1, new QTableWidgetItem(r.nom + " " + r.prenom));
            ui->tableau_5->setItem(i, 2, new QTableWidgetItem(r.dtNaissance.toString("dd/MM/yyyy")));
            ui->tableau_5->setItem(i, 3, new QTableWidgetItem(r.adresse));
            ui->tableau_5->setItem(i, 4, new QTableWidgetItem(QString::number(r.telephone)));
            ui->tableau_5->setItem(i, 5, new QTableWidgetItem(r.email));
            ui->tableau_5->setItem(i, 6, new QTableWidgetItem(r.statut));
            ui->tableau_5->setItem(i, 7, new QTableWidgetItem(r.situationFamiliale));
        }
    } else {
        QMessageBox::warning(this, tr("Erreur"), errorMsg);
    }
}

void MainWindow::onExporterResident()
{
    if (ui->tableau_5->rowCount() == 0) {
        QMessageBox::warning(this, tr("Avertissement"), tr("Aucune donnée à exporter."));
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty())
        return;
    
    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";
    
    QPdfWriter pdf(fileName);
    pdf.setPageSize(QPageSize::A4);
    pdf.setResolution(300);
    
    QPainter painter(&pdf);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // --- TITRE ---
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(0, 120, pdf.width(), 100, Qt::AlignCenter, "Liste des Résidents");
    
    // --- MARGES ET DIMENSIONS ---
    int leftMargin = 80;
    int rightMargin = 80;
    int topMargin = 260;
    int headerHeight = 100;
    int rowHeight = 100;
    int colCount = ui->tableau_5->columnCount();
    int rowCount = ui->tableau_5->rowCount();
    int tableWidth = pdf.width() - (leftMargin + rightMargin);
    
    // --- LARGEUR DES COLONNES (proportionnelle) ---
    QVector<int> colWeights = {10, 20, 15, 15, 15, 15, 10, 15};
    int totalWeight = 0;
    for (int w : colWeights) totalWeight += w;
    
    QVector<int> colWidths;
    for (int i = 0; i < colCount && i < colWeights.size(); ++i)
        colWidths.append((tableWidth * colWeights[i]) / totalWeight);
    
    // --- EN-TÊTES ---
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setBrush(QColor(210, 210, 210));
    painter.setPen(Qt::black);
    int x = leftMargin;
    
    for (int col = 0; col < colCount; ++col) {
        QTableWidgetItem *headerItem = ui->tableau_5->horizontalHeaderItem(col);
        QString headerText = headerItem ? headerItem->text() : QString("Colonne %1").arg(col + 1);
        
        QRect headerRect(x, topMargin, colWidths[col], headerHeight);
        painter.drawRect(headerRect);
        painter.drawText(headerRect.adjusted(4, 6, -4, -4),
                         Qt::AlignCenter | Qt::TextWordWrap,
                         headerText);
        x += colWidths[col];
    }
    
    // --- CONTENU DU TABLEAU ---
    painter.setFont(QFont("Arial", 11));
    painter.setBrush(Qt::NoBrush);
    int y = topMargin + headerHeight;
    
    for (int row = 0; row < rowCount; ++row) {
        x = leftMargin;
        for (int col = 0; col < colCount && col < colWidths.size(); ++col) {
            QRect cellRect(x, y, colWidths[col], rowHeight);
            painter.drawRect(cellRect);
            QTableWidgetItem *item = ui->tableau_5->item(row, col);
            QString text = item ? item->text() : "";
            painter.drawText(cellRect.adjusted(10, 0, -10, 0),
                             Qt::AlignVCenter | Qt::AlignLeft,
                             text);
            x += colWidths[col];
        }
        y += rowHeight;
    }
    
    painter.end();
    QMessageBox::information(this, tr("Succès"), tr("Le tableau a été exporté avec succès."));
}

// ========== JARDIN CRUD IMPLEMENTATIONS ==========

void MainWindow::connectJardinUi()
{
    // Wire Jardin CRUD buttons
    if (ui->ajouter_8) {
        connect(ui->ajouter_8, &QPushButton::clicked, this, &MainWindow::onAjouterJardin);
    }
    if (ui->modifier_8) {
        connect(ui->modifier_8, &QPushButton::clicked, this, &MainWindow::onModifierJardin);
    }
    if (ui->supprimer_7) {
        connect(ui->supprimer_7, &QPushButton::clicked, this, &MainWindow::onSupprimerJardin);
    }
}

void MainWindow::refreshJardinTable()
{
    if (!ui->tableau_8) return;

    QSqlQuery query;
    // Assumes table JARDINS with columns: IDJARDIN, EMPLACEMENT, SUPERFICIE, TYPESOL, TYPECHOIX
    if (!query.exec("SELECT IDJARDIN, EMPLACEMENT, SUPERFICIE, TYPESOL, TYPECHOIX FROM JARDINS ORDER BY IDJARDIN")) {
        qDebug() << "refreshJardinTable query error:" << query.lastError().text();
        return;
    }

    ui->tableau_8->clear();
    ui->tableau_8->setColumnCount(5);
    ui->tableau_8->setHorizontalHeaderLabels(QStringList() << "ID" << "Emplacement" << "Superficie" << "TypeSol" << "TypeChoix");

    int row = 0;
    ui->tableau_8->setRowCount(0);
    while (query.next()) {
        ui->tableau_8->insertRow(row);
        ui->tableau_8->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableau_8->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableau_8->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        ui->tableau_8->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableau_8->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
        row++;
    }
}

static QString trimmedText(const QString &s) { return s.trimmed(); }

void MainWindow::onAjouterJardin()
{
    const QString id          = trimmedText(ui->idjardinline ? ui->idjardinline->text() : QString());
    const QString emplacement = trimmedText(ui->emplacementline ? ui->emplacementline->text() : QString());
    const QString superficie  = trimmedText(ui->superficieline ? ui->superficieline->text() : QString());
    const QString typesol     = trimmedText(ui->superficieline_2 ? ui->superficieline_2->text() : QString());
    const QString typechoix   = ui->typechoix ? ui->typechoix->currentText().trimmed() : QString();

    if (id.isEmpty()) {
        QMessageBox::warning(this, tr("Champs manquants"), tr("ID Jardin est obligatoire."));
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO JARDINS (IDJARDIN, EMPLACEMENT, SUPERFICIE, TYPESOL, TYPECHOIX) "
                  "VALUES (:id, :emp, :sup, :ts, :tc)");
    query.bindValue(":id", id);
    query.bindValue(":emp", emplacement);
    query.bindValue(":sup", superficie);
    query.bindValue(":ts", typesol);
    query.bindValue(":tc", typechoix);

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Ajout échoué"), tr("Erreur: %1").arg(query.lastError().text()));
        return;
    }

    QMessageBox::information(this, tr("Succès"), tr("Jardin ajouté avec succès."));
    // Clear input fields
    ui->idjardinline->clear();
    ui->emplacementline->clear();
    ui->superficieline->clear();
    ui->superficieline_2->clear();
    ui->typechoix->setCurrentIndex(-1);
    selectedJardinId = -1;
    refreshJardinTable();
}

void MainWindow::onModifierJardin()
{
    const QString id          = trimmedText(ui->idjardinline ? ui->idjardinline->text() : QString());
    const QString emplacement = trimmedText(ui->emplacementline ? ui->emplacementline->text() : QString());
    const QString superficie  = trimmedText(ui->superficieline ? ui->superficieline->text() : QString());
    const QString typesol     = trimmedText(ui->superficieline_2 ? ui->superficieline_2->text() : QString());
    const QString typechoix   = ui->typechoix ? ui->typechoix->currentText().trimmed() : QString();

    if (id.isEmpty()) {
        QMessageBox::warning(this, tr("Champs manquants"), tr("ID Jardin est obligatoire pour modifier."));
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE JARDINS SET EMPLACEMENT = :emp, SUPERFICIE = :sup, TYPESOL = :ts, TYPECHOIX = :tc WHERE IDJARDIN = :id");
    query.bindValue(":id", id);
    query.bindValue(":emp", emplacement);
    query.bindValue(":sup", superficie);
    query.bindValue(":ts", typesol);
    query.bindValue(":tc", typechoix);

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Modification échouée"), tr("Erreur: %1").arg(query.lastError().text()));
        return;
    }

    QMessageBox::information(this, tr("Succès"), tr("Jardin modifié avec succès."));
    // Clear input fields
    ui->idjardinline->clear();
    ui->emplacementline->clear();
    ui->superficieline->clear();
    ui->superficieline_2->clear();
    ui->typechoix->setCurrentIndex(-1);
    selectedJardinId = -1;
    refreshJardinTable();
}

void MainWindow::onSupprimerJardin()
{
    const QString id = trimmedText(ui->idjardinline ? ui->idjardinline->text() : QString());
    if (id.isEmpty()) {
        QMessageBox::warning(this, tr("Champs manquants"), tr("ID Jardin est obligatoire pour supprimer."));
        return;
    }

    if (QMessageBox::question(this, tr("Confirmer"), tr("Supprimer le jardin %1?").arg(id)) != QMessageBox::Yes) {
        return;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM JARDINS WHERE IDJARDIN = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Suppression échouée"), tr("Erreur: %1").arg(query.lastError().text()));
        return;
    }

    QMessageBox::information(this, tr("Succès"), tr("Jardin supprimé."));
    // Clear input fields
    ui->idjardinline->clear();
    ui->emplacementline->clear();
    ui->superficieline->clear();
    ui->superficieline_2->clear();
    ui->typechoix->setCurrentIndex(-1);
    selectedJardinId = -1;
    refreshJardinTable();
}

void MainWindow::onExporterJardin()
{
    if (ui->tableau_8->rowCount() == 0) {
        QMessageBox::warning(this, tr("Avertissement"), tr("Aucune donnée à exporter."));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";

    QPdfWriter pdf(fileName);
    pdf.setPageSize(QPageSize::A4);
    pdf.setResolution(300);

    QPainter painter(&pdf);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- TITRE ---
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(0, 120, pdf.width(), 100, Qt::AlignCenter, "Liste des Jardins");

    // --- MARGES ET DIMENSIONS ---
    int leftMargin = 80;
    int rightMargin = 80;
    int topMargin = 260;
    int headerHeight = 100;
    int rowHeight = 100;
    int colCount = ui->tableau_8->columnCount();
    int rowCount = ui->tableau_8->rowCount();
    int tableWidth = pdf.width() - (leftMargin + rightMargin);

    // --- LARGEUR DES COLONNES (proportionnelle) ---
    QVector<int> colWeights = {10, 25, 20, 20, 25};
    int totalWeight = 0;
    for (int w : colWeights) totalWeight += w;

    QVector<int> colWidths;
    for (int i = 0; i < colCount && i < colWeights.size(); ++i)
        colWidths.append((tableWidth * colWeights[i]) / totalWeight);

    // --- EN-TÊTES ---
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setBrush(QColor(210, 210, 210));
    painter.setPen(Qt::black);
    int x = leftMargin;

    for (int col = 0; col < colCount; ++col) {
        QTableWidgetItem *headerItem = ui->tableau_8->horizontalHeaderItem(col);
        QString headerText = headerItem ? headerItem->text() : QString("Colonne %1").arg(col + 1);

        QRect headerRect(x, topMargin, colWidths[col], headerHeight);
        painter.drawRect(headerRect);
        painter.drawText(headerRect.adjusted(4, 6, -4, -4),
                         Qt::AlignCenter | Qt::TextWordWrap,
                         headerText);
        x += colWidths[col];
    }

    // --- CONTENU DU TABLEAU ---
    painter.setFont(QFont("Arial", 11));
    painter.setBrush(Qt::NoBrush);
    int y = topMargin + headerHeight;

    for (int row = 0; row < rowCount; ++row) {
        x = leftMargin;
        for (int col = 0; col < colCount && col < colWidths.size(); ++col) {
            QRect cellRect(x, y, colWidths[col], rowHeight);
            painter.drawRect(cellRect);
            QTableWidgetItem *item = ui->tableau_8->item(row, col);
            QString text = item ? item->text() : "";
            painter.drawText(cellRect.adjusted(10, 0, -10, 0),
                             Qt::AlignVCenter | Qt::AlignLeft,
                             text);
            x += colWidths[col];
        }
        y += rowHeight;
    }

    painter.end();
    QMessageBox::information(this, tr("Succès"), tr("Le tableau a été exporté avec succès."));
}

/* ============================================================
 *                     CARTOGRAPHIE / ALERTES
 * ============================================================ */

/* ============================================================
 *                         NAVIGATION
 * ============================================================ */
// Les connexions générales sont déjà définies plus haut

/* ============================================================
 *                      CRUD MAISON
 * ============================================================ */
void MainWindow::connectMaisonButtons()
{
    connect(ui->ajouter_7,   &QPushButton::clicked, this, &MainWindow::onAjouterMaison);
    connect(ui->modifier_7,  &QPushButton::clicked, this, &MainWindow::onModifierMaison);
    connect(ui->supprimer_6, &QPushButton::clicked, this, &MainWindow::onSupprimerMaison);

    connect(ui->tableau_7, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onTableMaisonSelectionChanged);
}

/* ============================================================
 *                   GESTION ALERTES
 * ============================================================ */
void MainWindow::connectAlerteButtons()
{
    connect(ui->btnRetourAlertes, &QPushButton::clicked,
            this, &MainWindow::onRetourAlertes);

    connect(ui->btnAfficherCarte, &QPushButton::clicked,
            this, &MainWindow::onAfficherCarte);
}

void MainWindow::on_Alertes_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->stackedWidget_5->setCurrentIndex(1);
    refreshAlertes();
}

/* ============================================================
 *                    AJOUT MAISON
 * ============================================================ */
// Les CRUD Maisons sont déjà définis plus haut dans le fichier

/* ============================================================
 *                 CHARGEMENT DES INPUTS
 * ============================================================ */
void MainWindow::onTableMaisonSelectionChanged()
{
    if (ui->tableau_7->selectedItems().isEmpty())
        return;

    int row = ui->tableau_7->currentRow();
    int id  = ui->tableau_7->item(row, 0)->text().toInt();

    loadMaisonToInputs(id);
}

void MainWindow::loadMaisonToInputs(int id)
{
    Maison m;
    if (!m.rechercher(id)) return;

    ui->idmaisonline->setText(QString::number(m.getId()));
    ui->adresseline_2->setText(m.getAdresse());
    ui->nivsecline->setText(QString::number(m.getSecurite()));
    ui->statuschoix->setCurrentText(m.getStatut());
    ui->typechoix_2->setCurrentText(m.getType());
    ui->nbrpieceschoix->setCurrentText(QString::number(m.getNbrPieces()));
}

/* ============================================================
 *                AFFICHER TABLE MAISONS
 * ============================================================ */
void MainWindow::refreshTableMaison()
{
    ui->tableau_7->clearContents();
    ui->tableau_7->setRowCount(0);

    QSqlQueryModel *model = Maison::afficher();

    for (int i = 0; i < model->rowCount(); ++i) {
        ui->tableau_7->insertRow(i);

        for (int j = 0; j < model->columnCount(); ++j) {
            ui->tableau_7->setItem(
                i, j,
                new QTableWidgetItem(model->data(model->index(i, j)).toString())
                );
        }
    }
}

/* ============================================================
 *                   STATISTIQUES
 * ============================================================ */
QMap<QString, int> MainWindow::getStatistiquesStatus()
{
    Maison m;
    return m.getStatistiquesStatus();
}

QMap<QString, int> MainWindow::getStatistiquesNiveauSecurite()
{
    Maison m;
    return m.getStatistiquesNiveauSecurite();
}

/* ------------------------------------------------------------ */
void MainWindow::on_statistique_currentIndexChanged(int index)
{
    if (index == 0) return;

    QDialog *d = new QDialog(this);
    d->setWindowTitle("Statistiques des maisons");
    d->setMinimumSize(650, 520);

    QVBoxLayout *layout = new QVBoxLayout(d);
    QPieSeries *series = new QPieSeries();
    QString title;
    int total = 0;

    if (index == 1)
    {
        auto s = getStatistiquesStatus();
        total = s["total"];
        title = "Statistiques des statuts";

        if (s["vide"] > 0)      series->append("Vide", s["vide"]);
        if (s["occupe"] > 0)    series->append("Occupé", s["occupe"]);
        if (s["enservice"] > 0) series->append("En service", s["enservice"]);
    }

    else if (index == 2)
    {
        auto s = getStatistiquesNiveauSecurite();
        total = s["total"];
        title = "Niveau de sécurité";

        if (s["danger"] > 0)    series->append("Danger (<2)", s["danger"]);
        if (s["moyen"] > 0)     series->append("Moyen (2-3)", s["moyen"]);
        if (s["bon"] > 0)       series->append("Bon (>3)", s["bon"]);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title + " — Total : " + QString::number(total));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setVisible(true);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);

    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok);
    layout->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, d, &QDialog::accept);

    d->exec();
}

/* ============================================================
 *                RECHERCHE, TRI, EXPORT
 * ============================================================ */
void MainWindow::clearMaisonInputs()
{
    ui->idmaisonline->clear();
    ui->adresseline_2->clear();
    ui->nivsecline->clear();
    ui->statuschoix->setCurrentIndex(0);
    ui->typechoix_2->setCurrentIndex(0);
    ui->nbrpieceschoix->setCurrentIndex(0);
}

void MainWindow::onTriMaisonsChanged(int index)
{
    if (!ui->tableau_7) return;
    switch (index) {
    case 1: ui->tableau_7->sortItems(0, Qt::AscendingOrder); break;
    case 2: ui->tableau_7->sortItems(0, Qt::DescendingOrder); break;
    case 3: ui->tableau_7->sortItems(1, Qt::AscendingOrder); break;
    case 4: ui->tableau_7->sortItems(2, Qt::AscendingOrder); break;
    case 5: ui->tableau_7->sortItems(2, Qt::DescendingOrder); break;
    case 6: ui->tableau_7->sortItems(3, Qt::AscendingOrder); break;
    case 7: ui->tableau_7->sortItems(4, Qt::AscendingOrder); break;
    default: break;
    }
}

void MainWindow::onRechercheMaisonsChanged(const QString &text)
{
    QString t = text.trimmed();
    for (int r = 0; r < ui->tableau_7->rowCount(); ++r) {
        bool match = t.isEmpty();
        if (!match) {
            for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
                QTableWidgetItem *it = ui->tableau_7->item(r, c);
                if (it && it->text().contains(t, Qt::CaseInsensitive)) { match = true; break; }
            }
        }
        ui->tableau_7->setRowHidden(r, !match);
    }
}

// Recherche/Tri spécifiques non utilisés: retirés

void MainWindow::onExporterMaisons()
{
    bool ok = false;
    const QString choice = QInputDialog::getItem(
        this, "Exporter les maisons", "Format", {"PDF", "CSV"}, 0, false, &ok);
    if (!ok || choice.isEmpty()) return;
    if (choice == "PDF") exporterMaisonsPDF(); else exporterMaisonsCSV();
}

void MainWindow::exporterMaisonsPDF()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter PDF", QString(), "PDF (*.pdf)");
    if (path.isEmpty()) return;

    QString html;
    html += "<h2>Liste des maisons</h2>";
    html += "<table border='1' cellspacing='0' cellpadding='4'>";
    html += "<tr>";
    for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
        QTableWidgetItem *h = ui->tableau_7->horizontalHeaderItem(c);
        html += "<th>" + (h ? h->text() : QString("Col %1").arg(c)) + "</th>";
    }
    html += "</tr>";
    for (int r = 0; r < ui->tableau_7->rowCount(); ++r) {
        if (ui->tableau_7->isRowHidden(r)) continue;
        html += "<tr>";
        for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
            QTableWidgetItem *it = ui->tableau_7->item(r, c);
            html += "<td>" + (it ? it->text().toHtmlEscaped() : QString()) + "</td>";
        }
        html += "</tr>";
    }
    html += "</table>";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);
}

void MainWindow::exporterMaisonsCSV()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter CSV", QString(), "CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
        QTableWidgetItem *h = ui->tableau_7->horizontalHeaderItem(c);
        out << (h ? h->text() : QString("Col %1").arg(c));
        if (c < ui->tableau_7->columnCount() - 1) out << ",";
    }
    out << "\n";
    for (int r = 0; r < ui->tableau_7->rowCount(); ++r) {
        if (ui->tableau_7->isRowHidden(r)) continue;
        for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
            QTableWidgetItem *it = ui->tableau_7->item(r, c);
            out << (it ? it->text() : QString());
            if (c < ui->tableau_7->columnCount() - 1) out << ",";
        }
        out << "\n";
    }
    file.close();
}

void MainWindow::refreshAlertes()
{
    QSqlQuery query("SELECT ZONE, NIVEAU, STATUT, DATE_ALERTE FROM GEST_ALERTES",
                    QSqlDatabase::database("qt_oracle"));

    ui->tableau_10->clearContents();
    ui->tableau_10->setRowCount(0);

    QTableWidgetItem *headerZone = ui->tableau_10->horizontalHeaderItem(0);
    if (headerZone) headerZone->setText("zone");
    QTableWidgetItem *headerNiv = ui->tableau_10->horizontalHeaderItem(1);
    if (headerNiv) headerNiv->setText("niveau");
    QTableWidgetItem *headerStatut = ui->tableau_10->horizontalHeaderItem(2);
    if (headerStatut) headerStatut->setText("statut");
    QTableWidgetItem *headerDate = ui->tableau_10->horizontalHeaderItem(3);
    if (headerDate) headerDate->setText("date");
    QTableWidgetItem *headerLoc = ui->tableau_10->horizontalHeaderItem(4);
    if (headerLoc) headerLoc->setText("Localisation");

    int row = 0;
    while (query.next()) {
        ui->tableau_10->insertRow(row);

        QString zone   = query.value(0).toString();
        QString niveau = query.value(1).toString();
        QString statut = query.value(2).toString();
        QString dateA  = query.value(3).toString();

        ui->tableau_10->setItem(row, 0, new QTableWidgetItem(zone));
        ui->tableau_10->setItem(row, 1, new QTableWidgetItem(niveau));
        ui->tableau_10->setItem(row, 2, new QTableWidgetItem(statut));
        ui->tableau_10->setItem(row, 3, new QTableWidgetItem(dateA));

        QPushButton *btn = new QPushButton("Localiser");
        btn->setStyleSheet("background-color: #007bff; color: white; border-radius: 6px; padding: 4px 8px;");
        connect(btn, &QPushButton::clicked, this, [this, zone]() { loadMapForZone(zone); });
        ui->tableau_10->setCellWidget(row, 4, btn);

        row++;
    }

    ui->tableau_10->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::onAfficherCarte()
{
    int row = ui->tableau_10->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Carte", "Veuillez sélectionner une alerte.");
        return;
    }
    QString zone = ui->tableau_10->item(row, 0)->text();
    loadMapForZone(zone);
}

void MainWindow::loadMapForZone(const QString &zone)
{
    QUrl geocode("https://nominatim.openstreetmap.org/search");
    QUrlQuery query;
    query.addQueryItem("q", zone);
    query.addQueryItem("format", "json");
    query.addQueryItem("limit", "1");
    geocode.setQuery(query);
    QNetworkRequest req(geocode);
    req.setRawHeader("User-Agent", QByteArray("AtelierConnexion/1.0"));
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
        QUrl imgUrl(QString("http://staticmap.openstreetmap.de/staticmap.php?center=%1,%2&zoom=14&size=370x230&markers=%1,%2,red-pushpin").arg(lat, lon));
        QNetworkRequest imgReq(imgUrl);
        imgReq.setRawHeader("User-Agent", QByteArray("AtelierConnexion/1.0"));
        QNetworkReply *ri = net->get(imgReq);
        connect(ri, &QNetworkReply::finished, this, [this, ri, lat, lon]() {
            if (ri->error() == QNetworkReply::NoError) {
                QByteArray bytes = ri->readAll();
                ri->deleteLater();
                QPixmap pix;
                if (pix.loadFromData(bytes)) {
                    sceneCarte->clear();
                    sceneCarte->addPixmap(pix);
                    viewCarte->show();
                    return;
                }
            }
            ri->deleteLater();
            bool okLat = false, okLon = false;
            double dLat = lat.toDouble(&okLat);
            double dLon = lon.toDouble(&okLon);
            if (!okLat || !okLon) {
                QMessageBox::warning(this, "Carte", "Localisation introuvable.");
                return;
            }
            int zoom = 14;
            double latRad = qDegreesToRadians(dLat);
            double n = (1 << zoom);
            int xTile = qFloor((dLon + 180.0) / 360.0 * n);
            int yTile = qFloor((1.0 - log(tan(latRad) + 1.0 / cos(latRad)) / M_PI) / 2.0 * n);
            QUrl tileUrl(QString("http://tile.openstreetmap.org/%1/%2/%3.png").arg(QString::number(zoom), QString::number(xTile), QString::number(yTile)));
            QNetworkRequest tileReq(tileUrl);
            tileReq.setRawHeader("User-Agent", QByteArray("AtelierConnexion/1.0"));
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
                double latRad2 = qDegreesToRadians(dLat);
                double n2 = (1 << zoom);
                double pixelX = ((dLon + 180.0) / 360.0) * 256.0 * n2;
                double pixelY = ((1.0 - log(tan(latRad2) + 1.0 / cos(latRad2)) / M_PI) / 2.0) * 256.0 * n2;
                double localX = pixelX - 256.0 * xTile;
                double localY = pixelY - 256.0 * yTile;
                QPixmap composed = tilePix.copy();
                QPainter p(&composed);
                p.setRenderHint(QPainter::Antialiasing, true);
                p.setPen(QPen(Qt::red, 2));
                p.setBrush(QBrush(Qt::red));
                p.drawEllipse(QPointF(localX, localY), 6, 6);
                p.end();
                sceneCarte->clear();
                sceneCarte->addPixmap(composed);
                viewCarte->show();
            });
        });
    });
}

void MainWindow::onRetourAlertes()
{
    ui->stackedWidget_5->setCurrentIndex(0);
}
