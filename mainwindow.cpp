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
#include <QSqlError>
#include "Employee.h"

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

    connectButtons();
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

    // Table selection -> fill ID for delete/edit
    ui->tableau->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableau->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->tableau, &QTableWidget::itemSelectionChanged, this, &MainWindow::onTableSelectionChanged);
}
void MainWindow::onGestionEmployes()
{
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
    onChargerEmployes();
}

void MainWindow::onGestionVehicules()
{
    ui->stackedWidget->setCurrentWidget(ui->pageVehicules);
}

void MainWindow::onGestionResidents()
{
    ui->stackedWidget->setCurrentWidget(ui->pageResidents);
}

void MainWindow::onGestionMaisons()
{
    ui->stackedWidget->setCurrentWidget(ui->pageMaisons);
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
    const QVector<Employee> items = Employee::fetchAll(err);
    if (!err.isEmpty()) {
        qWarning() << "Erreur liste employés:" << err;
        QMessageBox::warning(this, tr("Erreur DB"), err);
    }

    ui->tableau->clear();
    ui->tableau->setColumnCount(9);
    ui->tableau->setRowCount(items.size());
    QStringList headers; headers << "ID" << "Nom" << "Prénom" << "Poste" << "Salaire" << "Adresse" << "Téléphone" << "Email" << "ID_RES";
    ui->tableau->setHorizontalHeaderLabels(headers);
    int highlightRow = -1;
    for (int i = 0; i < items.size(); ++i) {
        const Employee &e = items[i];
        ui->tableau->setItem(i, 0, new QTableWidgetItem(QString::number(e.idEmploye)));
        ui->tableau->setItem(i, 1, new QTableWidgetItem(e.nom));
        ui->tableau->setItem(i, 2, new QTableWidgetItem(e.prenom));
        ui->tableau->setItem(i, 3, new QTableWidgetItem(e.poste));
        ui->tableau->setItem(i, 4, new QTableWidgetItem(QString::number(e.salaire, 'f', 2)));
        ui->tableau->setItem(i, 5, new QTableWidgetItem(e.adresse));
        ui->tableau->setItem(i, 6, new QTableWidgetItem(QString::number(e.telephone)));
        ui->tableau->setItem(i, 7, new QTableWidgetItem(e.email));
        ui->tableau->setItem(i, 8, new QTableWidgetItem(e.idRes));
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
    }
}

void MainWindow::onAjouterEmploye()
{
    Employee e;
    // If your DB requires manual IDs (Oracle), set it from the field
    bool okId = false;
    int id = ui->idline->text().toInt(&okId);
    if (okId && id > 0) e.idEmploye = id;
    e.nom = ui->nomline->text().trimmed();
    e.prenom = ui->prenomline->text().trimmed();
    e.poste = ui->posteline->text().trimmed();
    e.salaire = ui->salaireline->text().toDouble();
    e.adresse = ui->adresseline->text().trimmed();
    e.telephone = ui->telephoneline->text().toLongLong();
    e.email = ui->emailline->text().trimmed();
    // Keep ID_RES empty for now unless you wire a valid parent
    e.idRes.clear();

    // Basic length validation for VARCHAR2(20)
    if (e.nom.size() > 20 || e.prenom.size() > 20 || e.poste.size() > 20 || e.adresse.size() > 20) {
        QMessageBox::warning(this, tr("Erreur"), tr("Champs texte max 20 caractères (Nom, Prénom, Poste, Adresse)"));
        return;
    }

    int newId = 0; QString err;
    if (e.insert(newId, err)) {
        QMessageBox::information(this, tr("Succès"), tr("Employé ajouté."));
        m_lastInsertedId = (newId > 0 ? newId : e.idEmploye);
        onChargerEmployes();
    } else {
        QMessageBox::warning(this, tr("Erreur"), err);
    }
}

void MainWindow::onModifierEmploye()
{
    bool okId = false;
    const int id = ui->idline->text().toInt(&okId);
    if (!okId || id <= 0) { QMessageBox::warning(this, tr("Erreur"), tr("ID invalide")); return; }

    Employee e;
    e.nom = ui->nomline->text().trimmed();
    e.prenom = ui->prenomline->text().trimmed();
    e.poste = ui->posteline->text().trimmed();
    e.salaire = ui->salaireline->text().toDouble();
    e.adresse = ui->adresseline->text().trimmed();
    e.telephone = ui->telephoneline->text().toLongLong();
    e.email = ui->emailline->text().trimmed();
    e.idRes.clear();

    QString err;
    if (e.updateById(id, err)) {
        QMessageBox::information(this, tr("Succès"), tr("Employé modifié."));
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
        QMessageBox::information(this, tr("Succès"), tr("Employé supprimé."));
        onChargerEmployes();
    } else {
        QMessageBox::warning(this, tr("Erreur"), err);
    }
}

void MainWindow::onTableSelectionChanged()
{
    if (ui->tableau->currentRow() < 0) return;
    QTableWidgetItem *it = ui->tableau->item(ui->tableau->currentRow(), 0);
    if (!it) return;
    ui->idline->setText(it->text());
}
