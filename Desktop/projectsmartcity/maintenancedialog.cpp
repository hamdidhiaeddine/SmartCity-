#include "maintenancedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>

namespace
{
    QSqlDatabase resolveDb()
    {
        if (QSqlDatabase::contains("qt_oracle")) {
            return QSqlDatabase::database("qt_oracle");
        }
        return QSqlDatabase::database();
    }
}

MaintenanceDialog::MaintenanceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Gestion de la Maintenance Prédictive"));
    setMinimumSize(1000, 650);
    setupUI();
    chargerJardins();
    chargerMaintenances();
}

MaintenanceDialog::~MaintenanceDialog()
{
}

void MaintenanceDialog::chargerJardins()
{
    m_jardinsMap.clear();
    m_jardinCombo->clear();
    m_filtreJardinCombo->clear();
    m_filtreJardinCombo->addItem(tr("Tous les jardins"), -1);
    
    QSqlDatabase db = resolveDb();
    if (!db.isOpen()) return;
    
    QSqlQuery query(db);
    if (query.exec("SELECT ID_JARDIN, NOM FROM GEST_JARDIN ORDER BY NOM")) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString nom = query.value(1).toString();
            m_jardinsMap[id] = nom;
            m_jardinCombo->addItem(QString("%1 - %2").arg(id).arg(nom), id);
            m_filtreJardinCombo->addItem(QString("%1 - %2").arg(id).arg(nom), id);
        }
    }
}

void MaintenanceDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Barre de filtre
    QHBoxLayout* filterLayout = new QHBoxLayout();
    QLabel* lblFiltre = new QLabel(tr("Filtrer par jardin:"));
    m_filtreJardinCombo = new QComboBox();
    m_btnFiltrer = new QPushButton(tr("Filtrer"));
    m_btnFiltrer->setStyleSheet("QPushButton { background-color: #6c757d; color: white; padding: 6px 12px; border-radius: 5px; }");
    connect(m_btnFiltrer, &QPushButton::clicked, this, &MaintenanceDialog::onFiltrerParJardinClicked);
    
    filterLayout->addWidget(lblFiltre);
    filterLayout->addWidget(m_filtreJardinCombo);
    filterLayout->addWidget(m_btnFiltrer);
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);
    
    // Tableau des maintenances
    m_tableWidget = new QTableWidget();
    m_tableWidget->setColumnCount(8);
    m_tableWidget->setHorizontalHeaderLabels({
        tr("ID"), tr("Jardin"), tr("Type"), tr("Description"),
        tr("Date Effectuée"), tr("Date Recommandée"), tr("Statut"), tr("Fréquence")
    });
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setStyleSheet("QTableWidget { color: black; } QTableWidget::item { color: black; }");
    connect(m_tableWidget, &QTableWidget::itemSelectionChanged, this, &MaintenanceDialog::onSelectionChanged);
    
    mainLayout->addWidget(m_tableWidget);
    
    // Formulaire
    QGroupBox* formGroup = new QGroupBox(tr("Détails de la Maintenance"));
    QFormLayout* formLayout = new QFormLayout(formGroup);
    
    // Style pour texte noir dans les champs
    QString inputStyle = "QLineEdit, QComboBox, QDateEdit { color: black; background-color: white; } "
                         "QLineEdit:read-only { background-color: #f0f0f0; }";
    
    m_idEdit = new QLineEdit();
    m_idEdit->setPlaceholderText(tr("ID automatique"));
    m_idEdit->setReadOnly(true);
    m_idEdit->setStyleSheet(inputStyle);
    formLayout->addRow(tr("ID Maintenance:"), m_idEdit);
    
    m_jardinCombo = new QComboBox();
    m_jardinCombo->setPlaceholderText(tr("Sélectionner un jardin"));
    m_jardinCombo->setStyleSheet(inputStyle);
    formLayout->addRow(tr("Jardin:"), m_jardinCombo);
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItems(Maintenance::typesMaintenanceDisponibles());
    m_typeCombo->setStyleSheet(inputStyle);
    formLayout->addRow(tr("Type:"), m_typeCombo);
    
    m_descriptionEdit = new QLineEdit();
    m_descriptionEdit->setPlaceholderText(tr("Description de la maintenance"));
    m_descriptionEdit->setStyleSheet(inputStyle);
    formLayout->addRow(tr("Description:"), m_descriptionEdit);
    
    m_dateEffectueeEdit = new QDateEdit();
    m_dateEffectueeEdit->setCalendarPopup(true);
    m_dateEffectueeEdit->setDate(QDate::currentDate());
    m_dateEffectueeEdit->setStyleSheet(inputStyle);
    formLayout->addRow(tr("Date Effectuée:"), m_dateEffectueeEdit);
    
    m_dateRecommandeeEdit = new QDateEdit();
    m_dateRecommandeeEdit->setCalendarPopup(true);
    m_dateRecommandeeEdit->setDate(QDate::currentDate().addDays(7));
    m_dateRecommandeeEdit->setStyleSheet(inputStyle);
    formLayout->addRow(tr("Date Recommandée:"), m_dateRecommandeeEdit);
    
    m_statutCombo = new QComboBox();
    m_statutCombo->addItems(Maintenance::statutsDisponibles());
    m_statutCombo->setStyleSheet(inputStyle);
    formLayout->addRow(tr("Statut:"), m_statutCombo);
    
    m_frequenceCombo = new QComboBox();
    m_frequenceCombo->addItems(Maintenance::frequencesDisponibles());
    m_frequenceCombo->setStyleSheet(inputStyle);
    formLayout->addRow(tr("Fréquence:"), m_frequenceCombo);
    
    mainLayout->addWidget(formGroup);
    
    // Boutons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    m_btnAjouter = new QPushButton(tr("Ajouter"));
    m_btnAjouter->setStyleSheet("QPushButton { background-color: #28a745; color: white; padding: 8px 16px; border-radius: 5px; }");
    connect(m_btnAjouter, &QPushButton::clicked, this, &MaintenanceDialog::onAjouterClicked);
    
    m_btnModifier = new QPushButton(tr("Modifier"));
    m_btnModifier->setStyleSheet("QPushButton { background-color: #fd7e14; color: white; padding: 8px 16px; border-radius: 5px; }");
    connect(m_btnModifier, &QPushButton::clicked, this, &MaintenanceDialog::onModifierClicked);
    
    m_btnSupprimer = new QPushButton(tr("Supprimer"));
    m_btnSupprimer->setStyleSheet("QPushButton { background-color: #dc3545; color: white; padding: 8px 16px; border-radius: 5px; }");
    connect(m_btnSupprimer, &QPushButton::clicked, this, &MaintenanceDialog::onSupprimerClicked);
    
    m_btnRafraichir = new QPushButton(tr("Rafraîchir"));
    m_btnRafraichir->setStyleSheet("QPushButton { background-color: #17a2b8; color: white; padding: 8px 16px; border-radius: 5px; }");
    connect(m_btnRafraichir, &QPushButton::clicked, this, &MaintenanceDialog::onRafraichirClicked);
    
    btnLayout->addWidget(m_btnAjouter);
    btnLayout->addWidget(m_btnModifier);
    btnLayout->addWidget(m_btnSupprimer);
    btnLayout->addWidget(m_btnRafraichir);
    btnLayout->addStretch();
    
    mainLayout->addLayout(btnLayout);
}

void MaintenanceDialog::chargerMaintenances()
{
    QString error;
    QList<Maintenance> maintenances = Maintenance::fetchAll(&error);
    
    if (!error.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Impossible de charger les maintenances: %1").arg(error));
        return;
    }
    
    m_tableWidget->setRowCount(maintenances.size());
    
    for (int i = 0; i < maintenances.size(); ++i) {
        const Maintenance& m = maintenances[i];
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(m.id())));
        
        // Afficher le nom du jardin au lieu de l'ID
        QString jardinNom = m_jardinsMap.value(m.idJardin(), QString::number(m.idJardin()));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(jardinNom));
        m_tableWidget->item(i, 1)->setData(Qt::UserRole, m.idJardin());
        
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(m.typeMaintenance()));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(m.description()));
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(m.dateEffectuee().toString("dd/MM/yyyy")));
        m_tableWidget->setItem(i, 5, new QTableWidgetItem(m.dateRecommandee().toString("dd/MM/yyyy")));
        m_tableWidget->setItem(i, 6, new QTableWidgetItem(m.statut()));
        m_tableWidget->setItem(i, 7, new QTableWidgetItem(m.frequence()));
        
        // Colorer selon le statut
        QColor bgColor;
        if (m.statut() == "Terminé") bgColor = QColor(200, 255, 200);
        else if (m.statut() == "En cours") bgColor = QColor(255, 255, 200);
        else if (m.statut() == "Prévu") bgColor = QColor(200, 220, 255);
        else if (m.statut() == "Annulé") bgColor = QColor(255, 200, 200);
        else bgColor = QColor(255, 230, 200);
        
        for (int j = 0; j < 8; ++j) {
            if (m_tableWidget->item(i, j))
                m_tableWidget->item(i, j)->setBackground(bgColor);
        }
    }
}

void MaintenanceDialog::onFiltrerParJardinClicked()
{
    int idJardin = m_filtreJardinCombo->currentData().toInt();
    
    if (idJardin < 0) {
        // Tous les jardins
        chargerMaintenances();
        return;
    }
    
    QString error;
    QList<Maintenance> maintenances = Maintenance::fetchByJardin(idJardin, &error);
    
    if (!error.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Erreur de filtrage: %1").arg(error));
        return;
    }
    
    m_tableWidget->setRowCount(maintenances.size());
    
    for (int i = 0; i < maintenances.size(); ++i) {
        const Maintenance& m = maintenances[i];
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(m.id())));
        QString jardinNom = m_jardinsMap.value(m.idJardin(), QString::number(m.idJardin()));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(jardinNom));
        m_tableWidget->item(i, 1)->setData(Qt::UserRole, m.idJardin());
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(m.typeMaintenance()));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(m.description()));
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(m.dateEffectuee().toString("dd/MM/yyyy")));
        m_tableWidget->setItem(i, 5, new QTableWidgetItem(m.dateRecommandee().toString("dd/MM/yyyy")));
        m_tableWidget->setItem(i, 6, new QTableWidgetItem(m.statut()));
        m_tableWidget->setItem(i, 7, new QTableWidgetItem(m.frequence()));
    }
}

void MaintenanceDialog::onAjouterClicked()
{
    if (m_jardinCombo->currentIndex() < 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner un jardin"));
        return;
    }
    
    int id = Maintenance::getNextId();
    int idJardin = m_jardinCombo->currentData().toInt();
    
    Maintenance m(id, idJardin, m_typeCombo->currentText(), m_descriptionEdit->text(),
                  m_dateEffectueeEdit->date(), m_dateRecommandeeEdit->date(),
                  m_statutCombo->currentText(), m_frequenceCombo->currentText());
    
    QString error;
    if (m.ajouter(&error)) {
        QMessageBox::information(this, tr("Succès"), tr("Maintenance ajoutée avec succès (ID: %1)").arg(id));
        chargerMaintenances();
        reinitialiserFormulaire();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de l'ajout: %1").arg(error));
    }
}

void MaintenanceDialog::onModifierClicked()
{
    int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner une maintenance"));
        return;
    }
    
    int oldId = m_tableWidget->item(row, 0)->text().toInt();
    int newId = oldId; // Garder le même ID
    
    if (m_jardinCombo->currentIndex() < 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner un jardin"));
        return;
    }
    
    int idJardin = m_jardinCombo->currentData().toInt();
    
    Maintenance m(newId, idJardin, m_typeCombo->currentText(), m_descriptionEdit->text(),
                  m_dateEffectueeEdit->date(), m_dateRecommandeeEdit->date(),
                  m_statutCombo->currentText(), m_frequenceCombo->currentText());
    
    QString error;
    if (m.modifier(oldId, &error)) {
        QMessageBox::information(this, tr("Succès"), tr("Maintenance modifiée avec succès"));
        chargerMaintenances();
        reinitialiserFormulaire();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de la modification: %1").arg(error));
    }
}

void MaintenanceDialog::onSupprimerClicked()
{
    int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner une maintenance"));
        return;
    }
    
    int id = m_tableWidget->item(row, 0)->text().toInt();
    QString type = m_tableWidget->item(row, 2)->text();
    
    if (QMessageBox::question(this, tr("Confirmation"), 
        tr("Supprimer la maintenance '%1' (ID: %2) ?").arg(type).arg(id)) != QMessageBox::Yes) {
        return;
    }
    
    QString error;
    if (Maintenance::supprimer(id, &error)) {
        QMessageBox::information(this, tr("Succès"), tr("Maintenance supprimée"));
        chargerMaintenances();
        reinitialiserFormulaire();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de la suppression: %1").arg(error));
    }
}

void MaintenanceDialog::onRafraichirClicked()
{
    chargerJardins();
    chargerMaintenances();
    reinitialiserFormulaire();
}

void MaintenanceDialog::onSelectionChanged()
{
    int row = m_tableWidget->currentRow();
    if (row < 0) return;
    
    m_idEdit->setText(m_tableWidget->item(row, 0)->text());
    
    // Sélectionner le jardin dans le combo
    int idJardin = m_tableWidget->item(row, 1)->data(Qt::UserRole).toInt();
    for (int i = 0; i < m_jardinCombo->count(); ++i) {
        if (m_jardinCombo->itemData(i).toInt() == idJardin) {
            m_jardinCombo->setCurrentIndex(i);
            break;
        }
    }
    
    int typeIdx = m_typeCombo->findText(m_tableWidget->item(row, 2)->text());
    if (typeIdx >= 0) m_typeCombo->setCurrentIndex(typeIdx);
    
    m_descriptionEdit->setText(m_tableWidget->item(row, 3)->text());
    
    QDate dateEff = QDate::fromString(m_tableWidget->item(row, 4)->text(), "dd/MM/yyyy");
    if (dateEff.isValid()) m_dateEffectueeEdit->setDate(dateEff);
    
    QDate dateRec = QDate::fromString(m_tableWidget->item(row, 5)->text(), "dd/MM/yyyy");
    if (dateRec.isValid()) m_dateRecommandeeEdit->setDate(dateRec);
    
    int statutIdx = m_statutCombo->findText(m_tableWidget->item(row, 6)->text());
    if (statutIdx >= 0) m_statutCombo->setCurrentIndex(statutIdx);
    
    int freqIdx = m_frequenceCombo->findText(m_tableWidget->item(row, 7)->text());
    if (freqIdx >= 0) m_frequenceCombo->setCurrentIndex(freqIdx);
}

void MaintenanceDialog::reinitialiserFormulaire()
{
    m_idEdit->clear();
    m_jardinCombo->setCurrentIndex(-1);
    m_typeCombo->setCurrentIndex(0);
    m_descriptionEdit->clear();
    m_dateEffectueeEdit->setDate(QDate::currentDate());
    m_dateRecommandeeEdit->setDate(QDate::currentDate().addDays(7));
    m_statutCombo->setCurrentIndex(0);
    m_frequenceCombo->setCurrentIndex(1); // Hebdomadaire par défaut
}
