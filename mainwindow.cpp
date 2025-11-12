#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPixmap>
<<<<<<< HEAD
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
=======
#include <QFile>
#include <QDir>
>>>>>>> ca35564c4967a2d463a1f10ffb114ccf76ef4dbf

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
<<<<<<< HEAD
    this->resize(1920, 1080);
    this->move(0, 0);

    QPixmap logo(":/images/Lg.png");
    ui->logo->setPixmap(logo);
    ui->logo->setScaledContents(true);

    connectButtons();
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
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
        ui->typecombo_2->setCurrentText(ui->tableau_3->item(row, 4)->text());
        ui->etatcombo_2->setCurrentText(ui->tableau_3->item(row, 5)->text());
        ui->serviceline_2->setText(ui->tableau_3->item(row, 6)->text());
        QString dateStr = ui->tableau_3->item(row, 7)->text();  // get date from table
        QDate date = QDate::fromString(dateStr, "dd/MM/yyyy");  // convert to QDate
        ui->datemaintline_2->setText(date.toString("dd/MM/yyyy"));

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

    // CRUD Véhicules (connexion manuelle uniquement, pas de connexion automatique)
    connect(ui->ajouter_3, &QPushButton::clicked, this, &MainWindow::onAjouterVehicule);
    connect(ui->modifier_3, &QPushButton::clicked, this, &MainWindow::onModifierVehicule);
    connect(ui->supprimer_3, &QPushButton::clicked, this, &MainWindow::onSupprimerVehicule);
    connect(ui->exporter_3, &QPushButton::clicked, this, &MainWindow::onExporterVehicule);

    // CRUD Maisons (connexion manuelle uniquement, pas de connexion automatique)
    connect(ui->ajouter_7, &QPushButton::clicked, this, &MainWindow::onAjouterMaison);
    connect(ui->modifier_7, &QPushButton::clicked, this, &MainWindow::onModifierMaison);
    connect(ui->supprimer_6, &QPushButton::clicked, this, &MainWindow::onSupprimerMaison);
    connect(ui->exporter_7, &QPushButton::clicked, this, &MainWindow::onExporterMaison);
    
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
        ui->nivsecline->setText(ui->tableau_7->item(row, 3)->text());
        ui->statuschoix->setCurrentText(ui->tableau_7->item(row, 2)->text());
        ui->typechoix_2->setCurrentText(ui->tableau_7->item(row, 5)->text());
        ui->nbrpieceschoix->setCurrentText(ui->tableau_7->item(row, 4)->text());
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

    // Initialize Type ComboBox for vehicles with common types (already defined in UI, just set default)
    ui->typecombo_2->setCurrentIndex(-1); // No default selection

    // Initialize État ComboBox for vehicles with valid states
    ui->etatcombo_2->clear();
    ui->etatcombo_2->addItems(QStringList() << "Neuf" << "Usé" << "En panne");
    ui->etatcombo_2->setCurrentIndex(-1); // No default selection

    // Setup recherche and tri for Véhicules
    // Hide the recherche QLabel and replace it with QLineEdit for vehicles
    if (ui->recherche_2) {
        ui->recherche_2->hide();
    }
    
    // Find or create recherche line edit widget for vehicles
    QList<QLineEdit*> vehiculeLineEdits = ui->trirecherche_2->findChildren<QLineEdit*>();
    if (!vehiculeLineEdits.isEmpty()) {
        m_rechercheVehiculeLineEdit = vehiculeLineEdits.first();
        m_rechercheVehiculeLineEdit->setPlaceholderText(tr("Rechercher par immatriculation"));
        m_rechercheVehiculeLineEdit->setGeometry(10, 10, 301, 25);
        m_rechercheVehiculeLineEdit->setStyleSheet("background-color: white; color: black; font: 10pt \"Arial\";");
        connect(m_rechercheVehiculeLineEdit, &QLineEdit::textChanged, this, &MainWindow::onRechercheVehiculeChanged);
    } else {
        // Create a QLineEdit at the same position as the label
        m_rechercheVehiculeLineEdit = new QLineEdit(ui->trirecherche_2);
        m_rechercheVehiculeLineEdit->setPlaceholderText(tr("Rechercher par immatriculation"));
        m_rechercheVehiculeLineEdit->setGeometry(10, 10, 301, 25);
        m_rechercheVehiculeLineEdit->setStyleSheet("background-color: white; color: black; font: 10pt \"Arial\";");
        connect(m_rechercheVehiculeLineEdit, &QLineEdit::textChanged, this, &MainWindow::onRechercheVehiculeChanged);
    }
    
    // Connect tri combo boxes for vehicles
    connect(ui->triemail_2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTriTypeChanged);
    connect(ui->trisalaire_2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTriEtatChanged);

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
}

void MainWindow::onGestionCabinets()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCabinets);
}

void MainWindow::onDeconnexion()
{
    close();
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

// === Véhicules CRUD ===
void MainWindow::onAjouterVehicule()
{
    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    // Get text from editable ComboBox (use lineEdit if editable, otherwise currentText)
    QString type = ui->typecombo_2->isEditable() && ui->typecombo_2->lineEdit() 
                   ? ui->typecombo_2->lineEdit()->text().trimmed() 
                   : ui->typecombo_2->currentText().trimmed();
    QString etat = ui->etatcombo_2->currentText().trimmed(); // QComboBox
    QString service = ui->serviceline_2->text().trimmed();
    QDate date_maint = QDate::fromString(ui->datemaintline_2->text(), "dd/MM/yyyy");


    // === CONTRÔLES DE SAISIE ===
    if (immat.isEmpty() || marque.isEmpty() || modele.isEmpty() || service.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Tous les champs doivent être remplis."));
        return;
    }

    // Vérif immatriculation
    QRegularExpression immatRegex("^[A-Za-z0-9\\-]{4,15}$");
    if (!immatRegex.match(immat).hasMatch()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Format d'immatriculation invalide (ex : 123-TN-456)."));
        return;
    }

    // Vérif Type - Allow any non-empty value (ComboBox is editable)
    if (type.isEmpty() || type.toLower() == "type" || type.toLower() == "type de véhicule") {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez choisir un type de véhicule."));
        return;
    }

    // Vérif État (ComboBox)
    if (etat.isEmpty() || (etat != "Neuf" && etat != "Usé" && etat != "En panne")) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner un état valide (Neuf, Usé, En panne)."));
        return;
    }

    // Vérif date
    if (!date_maint.isValid()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Date de maintenance invalide. Format attendu : jj/MM/aaaa"));
        return;
    }
    if (date_maint > QDate::currentDate()) {
        QMessageBox::warning(this, tr("Erreur"), tr("La date de maintenance ne peut pas être dans le futur."));
        return;
    }

    // Vérif doublon immat
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM \"SYSTEM\".\"VEHICULES\" WHERE \"IMMATT\" = :IMMAT");
    checkQuery.bindValue(":IMMAT", immat);
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Un véhicule avec cette immatriculation existe déjà !"));
        return;
    }

    // === AJOUT APRÈS VALIDATION ===
    Vehicule v(immat, marque, modele, type, etat, service, date_maint);

    if (v.ajouter()) {
        QMessageBox::information(this, tr("Succès"), tr("Véhicule ajouté avec succès."));
        // Clear input fields after successful insert
        ui->immatline_2->clear();
        ui->marqueline_2->clear();
        ui->modeleline_2->clear();
        ui->typecombo_2->setCurrentIndex(-1);
        ui->etatcombo_2->setCurrentIndex(0);
        ui->serviceline_2->clear();
        ui->datemaintline_2->clear();
        selectedImmat.clear();
        loadVehicules();
        qDebug() << "✅ Véhicule ajouté :" << immat << marque << modele << etat;
    } else {
        QMessageBox::warning(this, tr("Erreur"), tr("Échec de l'ajout du véhicule."));
    }
}











void MainWindow::onModifierVehicule()
{
    qDebug() << "🔎 [DEBUG] onModifierVehicule() triggered";
    qDebug() << "🔎 [DEBUG] selectedImmat current value:" << selectedImmat;

    if (selectedImmat.isEmpty()) {
        qDebug() << "⚠️ [DEBUG] No vehicle selected before modifying!";
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez d'abord sélectionner un véhicule à modifier."));
        return;
    }

    // Read current form values
    QString immat   = ui->immatline_2->text().trimmed();
    QString marque  = ui->marqueline_2->text().trimmed();
    QString modele  = ui->modeleline_2->text().trimmed();
    // Get text from editable ComboBox
    QString type    = ui->typecombo_2->isEditable() && ui->typecombo_2->lineEdit() 
                      ? ui->typecombo_2->lineEdit()->text().trimmed() 
                      : ui->typecombo_2->currentText().trimmed();
    QString etat    = ui->etatcombo_2->currentText().trimmed();
    QString service = ui->serviceline_2->text().trimmed();
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

        QMessageBox::information(this, tr("Succès"), tr("Véhicule modifié avec succès."));
        // Clear input fields after successful update
        ui->immatline_2->clear();
        ui->marqueline_2->clear();
        ui->modeleline_2->clear();
        ui->typecombo_2->setCurrentIndex(-1);
        ui->etatcombo_2->setCurrentIndex(0);
        ui->serviceline_2->clear();
        ui->datemaintline_2->clear();
        selectedImmat.clear();  // reset after update
    } else {
        QMessageBox::warning(this, tr("Erreur"), tr("Échec de la modification du véhicule."));
    }
}

void MainWindow::onSupprimerVehicule()
{
    int row = ui->tableau_3->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner un véhicule à supprimer."));
        return;
    }

    QString immat = ui->tableau_3->item(row, 1)->text();
    
    if (QMessageBox::question(this, tr("Confirmer"), tr("Supprimer le véhicule %1?").arg(immat)) != QMessageBox::Yes) {
        return;
    }

    Vehicule v;
    bool success = v.supprimer(immat);

    if (success) {
        loadVehicules(); // Refresh table from database
        QMessageBox::information(this, tr("Succès"), tr("Véhicule supprimé."));
        // Clear input fields
        ui->immatline_2->clear();
        ui->marqueline_2->clear();
        ui->modeleline_2->clear();
        ui->typecombo_2->setCurrentIndex(-1);
        ui->etatcombo_2->setCurrentIndex(0);
        ui->serviceline_2->clear();
        ui->datemaintline_2->clear();
        selectedImmat.clear();
    } else {
        QMessageBox::warning(this, tr("Erreur"), tr("La suppression a échoué."));
    }
}

void MainWindow::onExporterVehicule()
{
    if (ui->tableau_3->rowCount() == 0) {
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
    painter.drawText(0, 120, pdf.width(), 100, Qt::AlignCenter, "Liste des Véhicules");

    // --- MARGES ET DIMENSIONS ---
    int leftMargin = 80;
    int rightMargin = 80;
    int topMargin = 260;
    int headerHeight = 100;
    int rowHeight = 100;
    int colCount = ui->tableau_3->columnCount();
    int rowCount = ui->tableau_3->rowCount();
    int tableWidth = pdf.width() - (leftMargin + rightMargin);

    // --- LARGEUR DES COLONNES (proportionnelle) ---
    QVector<int> colWeights = {8, 18, 15, 15, 10, 10, 12, 20};
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

    for (int col = 0; col < colCount; ++col) {
        QString headerText = ui->tableau_3->horizontalHeaderItem(col)->text();
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

void MainWindow::loadVehicules()
{
    // Load all vehicles from database and store in cache
    QSqlQuery q;
    if (!q.exec("SELECT \"IMMATT\", \"MARQUE\", \"MODELE\", \"TYPE\", \"ETAT\", \"SRV\", "
                "TO_CHAR(\"DATE_DE_MAIN\", 'DD/MM/YYYY') AS DATE_MAINT "
                "FROM \"SYSTEM\".\"VEHICULES\" ORDER BY \"IMMATT\" DESC"))
    {
        QMessageBox::critical(this, "Erreur SQL", q.lastError().text());
        return;
    }

    // Store all vehicles in cache for filtering
    m_allVehicules.clear();
    while (q.next()) {
        VehiculeData v;
        v.immat = q.value(0).toString();
        v.marque = q.value(1).toString();
        v.modele = q.value(2).toString();
        v.type = q.value(3).toString();
        v.etat = q.value(4).toString();
        v.service = q.value(5).toString();
        v.date_maint = q.value(6).toString();
        m_allVehicules.append(v);
    }

    // Apply search and filters
    appliquerRechercheEtTriVehicules();
}

void MainWindow::appliquerRechercheEtTriVehicules()
{
    if (m_allVehicules.isEmpty()) {
        ui->tableau_3->setRowCount(0);
        return;
    }

    QVector<VehiculeData> filtered = m_allVehicules;

    // Apply recherche by immatriculation
    if (m_rechercheVehiculeLineEdit && !m_rechercheVehiculeLineEdit->text().isEmpty()) {
        QString searchText = m_rechercheVehiculeLineEdit->text().trimmed().toUpper();
        QVector<VehiculeData> temp;
        for (const VehiculeData &v : filtered) {
            if (v.immat.toUpper().contains(searchText)) {
                temp.append(v);
            }
        }
        filtered = temp;
    }

    // Apply filter by Type
    int typeIndex = ui->triemail_2->currentIndex();
    if (typeIndex > 0) { // 0 is "Type" placeholder
        QString selectedType = ui->triemail_2->currentText();
        QVector<VehiculeData> temp;
        for (const VehiculeData &v : filtered) {
            if (v.type == selectedType) {
                temp.append(v);
            }
        }
        filtered = temp;
    }

    // Apply filter by État
    int etatIndex = ui->trisalaire_2->currentIndex();
    if (etatIndex > 0) { // 0 is "État" placeholder
        QString selectedEtat = ui->trisalaire_2->currentText();
        QVector<VehiculeData> temp;
        for (const VehiculeData &v : filtered) {
            if (v.etat == selectedEtat) {
                temp.append(v);
            }
        }
        filtered = temp;
    }

    // Display results in table
    ui->tableau_3->clear();
    ui->tableau_3->setColumnCount(8);
    ui->tableau_3->setRowCount(filtered.size());
    QStringList headers;
    headers << "ID" << "Immatriculation" << "Marque" << "Modèle"
            << "Type" << "État" << "Service" << "Date de maintenance";
    ui->tableau_3->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < filtered.size(); ++i) {
        const VehiculeData &v = filtered[i];
        ui->tableau_3->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1))); // ID
        ui->tableau_3->setItem(i, 1, new QTableWidgetItem(v.immat));
        ui->tableau_3->setItem(i, 2, new QTableWidgetItem(v.marque));
        ui->tableau_3->setItem(i, 3, new QTableWidgetItem(v.modele));
        ui->tableau_3->setItem(i, 4, new QTableWidgetItem(v.type));
        ui->tableau_3->setItem(i, 5, new QTableWidgetItem(v.etat));
        ui->tableau_3->setItem(i, 6, new QTableWidgetItem(v.service));
        ui->tableau_3->setItem(i, 7, new QTableWidgetItem(v.date_maint));
    }

    ui->tableau_3->setColumnHidden(0, true); // Hide ID column
    ui->tableau_3->resizeColumnsToContents();
}

void MainWindow::onRechercheVehiculeChanged()
{
    appliquerRechercheEtTriVehicules();
}

void MainWindow::onTriTypeChanged()
{
    appliquerRechercheEtTriVehicules();
}

void MainWindow::onTriEtatChanged()
{
    appliquerRechercheEtTriVehicules();
}

QChartView* MainWindow::createVehiculePieChart()
{
    // Create the data series
    QPieSeries* series = new QPieSeries();

    // Fetch data from the database
    QSqlQuery query;
    if (!query.exec("SELECT \"TYPE\", COUNT(*) FROM \"SYSTEM\".\"VEHICULES\" GROUP BY \"TYPE\"")) {
        qDebug() << "❌ SQL Error (stats TYPE):" << query.lastError().text();
        return nullptr;
    }

    int total = 0;
    QMap<QString, int> data;
    while (query.next()) {
        QString type = query.value(0).toString();
        int count = query.value(1).toInt();
        data[type] = count;
        total += count;
    }

    // Create slices dynamically
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

        QColor color = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 200, 250);
        slice->setBrush(color);

        // Hover animation effect
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

    // Create chart
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des véhicules par type (%)");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 9));
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setTheme(QChart::ChartThemeLight);

    // Create chart view
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Close button
    QPushButton* closeButton = new QPushButton("Fermer");
    closeButton->setFixedWidth(120);
    closeButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; border-radius: 8px; padding: 6px; }"
        "QPushButton:hover { background-color: #d32f2f; }"
    );

    // Create dialog container
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Statistiques des véhicules");
    dialog->resize(800, 600);
    dialog->setModal(true);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    // Connect close button
    QObject::connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    // Center the dialog on main window
    QRect parentRect = this->geometry();
    int x = parentRect.center().x() - dialog->width() / 2;
    int y = parentRect.center().y() - dialog->height() / 2;
    dialog->move(x, y);

    // Auto delete and show
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();

    return nullptr;
}

void MainWindow::on_pushButton_3_clicked()
{
    createVehiculePieChart();
}

// ========== MAISON CRUD IMPLEMENTATIONS ==========

void MainWindow::loadMaisons()
{
    Maison::afficher(ui->tableau_7);
}

void MainWindow::onAjouterMaison()
{
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
    // Get ID from input
    QString idStr = ui->idmaisonline->text().trimmed();
    if (idStr.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner une maison à supprimer."));
        return;
    }
    
    bool ok;
    int id = idStr.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Erreur"), tr("ID invalide."));
        return;
    }

    // Confirmation
    int ret = QMessageBox::question(this, tr("Confirmer"), tr("Supprimer la maison?"),
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

    // Delete
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
        if (errorMsg.isEmpty()) {
            errorMsg = tr("Vérifiez la console pour plus de détails.");
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

=======

    // === COMPREHENSIVE LOGO DEBUGGING ===
    qDebug() << "=== LOGO DEBUGGING START ===";

    // 1. Check if file exists physically
    QString physicalPath = "C:/Users/hamdi/OneDrive/Documents/menuges/Resources/images/Lg.png";
    QFile physicalFile(physicalPath);
    qDebug() << "Physical file exists:" << physicalFile.exists() << "at:" << physicalPath;

    // 2. Check all possible resource paths
    QStringList resourcePaths = {
        ":/Resources/image/Lg.png",
        ":/image/Lg.png",
        ":/Lg.png",
        "qrc:/Resources/image/Lg.png",
        "qrc:/image/Lg.png"
    };

    for(const QString &path : resourcePaths) {
        qDebug() << "Resource path" << path << "exists:" << QFile::exists(path);
    }

    // 3. Try to load the pixmap with different paths
    QPixmap logo;
    QString successfulPath;

    for(const QString &path : resourcePaths) {
        if(QFile::exists(path)) {
            logo.load(path);
            if(!logo.isNull()) {
                successfulPath = path;
                qDebug() << "Successfully loaded logo from:" << path;
                qDebug() << "Logo size:" << logo.size();
                break;
            }
        }
    }

    // 4. If resource loading failed, try physical path
    if(logo.isNull() && physicalFile.exists()) {
        logo.load(physicalPath);
        if(!logo.isNull()) {
            successfulPath = physicalPath;
            qDebug() << "Successfully loaded logo from physical path:" << physicalPath;
        }
    }

    // 5. Set the logo if loaded
    if(!logo.isNull()) {
        ui->label_6->setPixmap(logo);
        ui->label_6->setScaledContents(true);
        qDebug() << "Logo set on label_6 successfully";

        // Debug label properties
        qDebug() << "Label_6 size:" << ui->label_6->size();
        qDebug() << "Label_6 is visible:" << ui->label_6->isVisible();
        qDebug() << "Label_6 geometry:" << ui->label_6->geometry();
    } else {
        qDebug() << "FAILED: Could not load logo from any source!";

        // List files in resources directory for debugging
        qDebug() << "Listing files in :/Resources/images/";
        QDir resourceDir(":/Resources/images/");
        if(resourceDir.exists()) {
            QStringList files = resourceDir.entryList();
            qDebug() << "Files in resource dir:" << files;
        } else {
            qDebug() << "Resource directory doesn't exist!";
        }
    }

    qDebug() << "=== LOGO DEBUGGING END ===";

    // Rest of your existing code
    ui->main->setCurrentIndex(0);

    connect(ui->gestionemployes,  &QPushButton::clicked, this, &MainWindow::showEmployees);
    connect(ui->gestionvehicules, &QPushButton::clicked, this, &MainWindow::showVehicules);
    connect(ui->gestionresidents, &QPushButton::clicked, this, &MainWindow::showResidents);
    connect(ui->gestionmaisons,   &QPushButton::clicked, this, &MainWindow::showMaisons);
    connect(ui->gestionjardins,   &QPushButton::clicked, this, &MainWindow::showJardins);
    connect(ui->gestioncabinets,  &QPushButton::clicked, this, &MainWindow::showCabinets);
}

// ... rest of your functions remain the same
MainWindow::~MainWindow() { delete ui; }

void MainWindow::showEmployees() {
    qDebug() << "Employees clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->Employe_2));
}

void MainWindow::showVehicules() {
    qDebug() << "Vehicules clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->vehicule));
}

void MainWindow::showResidents() {
    qDebug() << "Residents clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->resident));
}

void MainWindow::showMaisons() {
    qDebug() << "Maisons clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->maison));
}

void MainWindow::showJardins() {
    qDebug() << "Jardins clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->jardin));
}

void MainWindow::showCabinets() {
    qDebug() << "Cabinets clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->cabinet));
}
>>>>>>> ca35564c4967a2d463a1f10ffb114ccf76ef4dbf
