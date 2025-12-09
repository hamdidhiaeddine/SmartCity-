#include "recommandationdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>

RecommandationDialog::RecommandationDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Recommandations de Plantes - Biodiversité"));
    setMinimumSize(950, 600);
    setupUI();
    chargerRecommandations();
}

RecommandationDialog::~RecommandationDialog()
{
}

void RecommandationDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Style pour texte noir dans les champs
    QString inputStyle = "QLineEdit { color: black; background-color: white; }";
    
    // Barre de recherche avec génération automatique
    QHBoxLayout* searchLayout = new QHBoxLayout();
    QLabel* lblRecherche = new QLabel(tr("Rechercher par type de sol:"));
    m_rechercheEdit = new QLineEdit();
    m_rechercheEdit->setPlaceholderText(tr("Ex: Argileux, Sableux, Limoneux..."));
    m_rechercheEdit->setStyleSheet(inputStyle);
    m_btnRechercher = new QPushButton(tr("Rechercher"));
    m_btnRechercher->setStyleSheet("QPushButton { background-color: #007bff; color: white; padding: 6px 12px; border-radius: 5px; }");
    connect(m_btnRechercher, &QPushButton::clicked, this, &RecommandationDialog::onRechercherClicked);
    
    m_btnGenererAleatoire = new QPushButton(tr("🎲 Générer Recommandations"));
    m_btnGenererAleatoire->setStyleSheet("QPushButton { background-color: #6f42c1; color: white; padding: 6px 12px; border-radius: 5px; font-weight: bold; }");
    m_btnGenererAleatoire->setToolTip(tr("Générer automatiquement des recommandations de plantes selon le type de sol"));
    connect(m_btnGenererAleatoire, &QPushButton::clicked, this, &RecommandationDialog::onGenererAleatoireClicked);
    
    searchLayout->addWidget(lblRecherche);
    searchLayout->addWidget(m_rechercheEdit);
    searchLayout->addWidget(m_btnRechercher);
    searchLayout->addWidget(m_btnGenererAleatoire);
    mainLayout->addLayout(searchLayout);
    
    // Tableau des recommandations - 6 colonnes
    m_tableWidget = new QTableWidget();
    m_tableWidget->setColumnCount(6);
    m_tableWidget->setHorizontalHeaderLabels({
        tr("ID"), tr("Type Sol"), tr("Nom Plante"), 
        tr("Recommandation"), tr("Compatibilité"), tr("Saison")
    });
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setStyleSheet("QTableWidget { color: black; } QTableWidget::item { color: black; }");
    connect(m_tableWidget, &QTableWidget::itemSelectionChanged, this, &RecommandationDialog::onSelectionChanged);
    
    mainLayout->addWidget(m_tableWidget);
    
    // Formulaire avec tous les champs
    QGroupBox* formGroup = new QGroupBox(tr("Détails de la Recommandation"));
    QFormLayout* formLayout = new QFormLayout(formGroup);
    
    // Réutiliser le même style pour texte noir
    QString formInputStyle = "QLineEdit, QComboBox, QTextEdit { color: black; background-color: white; } "
                         "QLineEdit:read-only { background-color: #f0f0f0; }";
    
    m_idEdit = new QLineEdit();
    m_idEdit->setPlaceholderText(tr("ID automatique"));
    m_idEdit->setReadOnly(true);
    m_idEdit->setStyleSheet(formInputStyle);
    formLayout->addRow(tr("ID Plante:"), m_idEdit);
    
    m_typeSolCombo = new QComboBox();
    m_typeSolCombo->addItems(RecommandationPlante::typesDesolDisponibles());
    m_typeSolCombo->setEditable(true);
    m_typeSolCombo->setStyleSheet(formInputStyle);
    formLayout->addRow(tr("Type de Sol:"), m_typeSolCombo);
    
    m_nomPlanteEdit = new QLineEdit();
    m_nomPlanteEdit->setPlaceholderText(tr("Nom de la plante (Rose, Lavande, etc.)"));
    m_nomPlanteEdit->setStyleSheet(formInputStyle);
    formLayout->addRow(tr("Nom Plante:"), m_nomPlanteEdit);
    
    m_recommandationEdit = new QTextEdit();
    m_recommandationEdit->setPlaceholderText(tr("Description de la recommandation pour cette plante et ce type de sol"));
    m_recommandationEdit->setMaximumHeight(60);
    m_recommandationEdit->setStyleSheet(formInputStyle);
    formLayout->addRow(tr("Recommandation:"), m_recommandationEdit);
    
    m_compatibiliteCombo = new QComboBox();
    m_compatibiliteCombo->addItems(RecommandationPlante::compatibilitesDisponibles());
    m_compatibiliteCombo->setStyleSheet(formInputStyle);
    formLayout->addRow(tr("Compatibilité:"), m_compatibiliteCombo);
    
    m_saisonCombo = new QComboBox();
    m_saisonCombo->addItems(RecommandationPlante::saisonsDisponibles());
    m_saisonCombo->setStyleSheet(formInputStyle);
    formLayout->addRow(tr("Saison:"), m_saisonCombo);
    
    mainLayout->addWidget(formGroup);
    
    // Boutons CRUD
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    m_btnAjouter = new QPushButton(tr("Ajouter"));
    m_btnAjouter->setStyleSheet("QPushButton { background-color: #28a745; color: white; padding: 8px 16px; border-radius: 5px; }");
    connect(m_btnAjouter, &QPushButton::clicked, this, &RecommandationDialog::onAjouterClicked);
    
    m_btnModifier = new QPushButton(tr("Modifier"));
    m_btnModifier->setStyleSheet("QPushButton { background-color: #fd7e14; color: white; padding: 8px 16px; border-radius: 5px; }");
    connect(m_btnModifier, &QPushButton::clicked, this, &RecommandationDialog::onModifierClicked);
    
    m_btnSupprimer = new QPushButton(tr("Supprimer"));
    m_btnSupprimer->setStyleSheet("QPushButton { background-color: #dc3545; color: white; padding: 8px 16px; border-radius: 5px; }");
    connect(m_btnSupprimer, &QPushButton::clicked, this, &RecommandationDialog::onSupprimerClicked);
    
    m_btnRafraichir = new QPushButton(tr("Rafraîchir"));
    m_btnRafraichir->setStyleSheet("QPushButton { background-color: #17a2b8; color: white; padding: 8px 16px; border-radius: 5px; }");
    connect(m_btnRafraichir, &QPushButton::clicked, this, &RecommandationDialog::onRafraichirClicked);
    
    btnLayout->addWidget(m_btnAjouter);
    btnLayout->addWidget(m_btnModifier);
    btnLayout->addWidget(m_btnSupprimer);
    btnLayout->addWidget(m_btnRafraichir);
    btnLayout->addStretch();
    
    mainLayout->addLayout(btnLayout);
}

void RecommandationDialog::chargerRecommandations()
{
    QString error;
    QList<RecommandationPlante> recommandations = RecommandationPlante::fetchAll(&error);
    
    if (!error.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Impossible de charger les recommandations: %1").arg(error));
        return;
    }
    
    m_tableWidget->setRowCount(recommandations.size());
    
    for (int i = 0; i < recommandations.size(); ++i) {
        const RecommandationPlante& rp = recommandations[i];
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(rp.idPlante())));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(rp.typeSol()));
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(rp.nomPlante()));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(rp.recommandation()));
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(rp.compatibilite()));
        m_tableWidget->setItem(i, 5, new QTableWidgetItem(rp.saison()));
        
        // Colorer selon compatibilité
        QColor bgColor;
        if (rp.compatibilite() == "Excellente") bgColor = QColor(200, 255, 200);
        else if (rp.compatibilite() == "Bonne") bgColor = QColor(220, 255, 220);
        else if (rp.compatibilite() == "Moyenne") bgColor = QColor(255, 255, 200);
        else bgColor = QColor(255, 220, 220);
        
        for (int j = 0; j < 6; ++j) {
            if (m_tableWidget->item(i, j))
                m_tableWidget->item(i, j)->setBackground(bgColor);
        }
    }
}

void RecommandationDialog::onGenererAleatoireClicked()
{
    // Demander le type de sol
    QStringList typesSol = RecommandationPlante::typesDesolDisponibles();
    bool ok;
    QString typeSol = QInputDialog::getItem(this, tr("Générer Recommandations"),
                                             tr("Choisir le type de sol:"),
                                             typesSol, 0, false, &ok);
    if (!ok || typeSol.isEmpty())
        return;
    
    // Demander le nombre de recommandations
    int count = QInputDialog::getInt(this, tr("Nombre de Recommandations"),
                                      tr("Combien de recommandations générer?"),
                                      3, 1, 6, 1, &ok);
    if (!ok)
        return;
    
    // Générer les recommandations
    QList<RecommandationPlante> recommandations = RecommandationPlante::genererRecommandationsAleatoires(typeSol, count);
    
    int ajoutees = 0;
    QString errors;
    
    for (const RecommandationPlante& rp : recommandations) {
        QString err;
        if (rp.ajouter(&err)) {
            ajoutees++;
        } else {
            errors += err + "\n";
        }
    }
    
    if (ajoutees > 0) {
        QMessageBox::information(this, tr("Succès"), 
            tr("%1 recommandation(s) générée(s) et ajoutée(s) pour le sol %2!")
            .arg(ajoutees).arg(typeSol));
        chargerRecommandations();
    } else {
        QMessageBox::warning(this, tr("Erreur"), 
            tr("Aucune recommandation n'a pu être ajoutée.\n%1").arg(errors));
    }
}

void RecommandationDialog::onAjouterClicked()
{
    int id = RecommandationPlante::getNextId();
    
    QString typeSol = m_typeSolCombo->currentText().trimmed();
    if (typeSol.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Le type de sol est obligatoire"));
        return;
    }
    
    QString nomPlante = m_nomPlanteEdit->text().trimmed();
    QString recommandation = m_recommandationEdit->toPlainText().trimmed();
    if (recommandation.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("La recommandation est obligatoire"));
        return;
    }
    
    QString compatibilite = m_compatibiliteCombo->currentText();
    QString saison = m_saisonCombo->currentText();
    
    RecommandationPlante rp(id, typeSol, recommandation, nomPlante, compatibilite, saison);
    
    QString error;
    if (rp.ajouter(&error)) {
        QMessageBox::information(this, tr("Succès"), tr("Recommandation ajoutée avec succès (ID: %1)").arg(id));
        chargerRecommandations();
        reinitialiserFormulaire();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de l'ajout: %1").arg(error));
    }
}

void RecommandationDialog::onModifierClicked()
{
    int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner une recommandation"));
        return;
    }
    
    int oldId = m_tableWidget->item(row, 0)->text().toInt();
    int newId = oldId; // Garder le même ID
    
    QString typeSol = m_typeSolCombo->currentText().trimmed();
    QString nomPlante = m_nomPlanteEdit->text().trimmed();
    QString recommandation = m_recommandationEdit->toPlainText().trimmed();
    QString compatibilite = m_compatibiliteCombo->currentText();
    QString saison = m_saisonCombo->currentText();
    
    RecommandationPlante rp(newId, typeSol, recommandation, nomPlante, compatibilite, saison);
    
    QString error;
    if (rp.modifier(oldId, &error)) {
        QMessageBox::information(this, tr("Succès"), tr("Recommandation modifiée avec succès"));
        chargerRecommandations();
        reinitialiserFormulaire();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de la modification: %1").arg(error));
    }
}

void RecommandationDialog::onSupprimerClicked()
{
    int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner une recommandation"));
        return;
    }
    
    int id = m_tableWidget->item(row, 0)->text().toInt();
    QString nomPlante = m_tableWidget->item(row, 2)->text();
    
    if (QMessageBox::question(this, tr("Confirmation"), 
        tr("Supprimer la recommandation '%1' (ID: %2) ?").arg(nomPlante).arg(id)) != QMessageBox::Yes) {
        return;
    }
    
    QString error;
    if (RecommandationPlante::supprimer(id, &error)) {
        QMessageBox::information(this, tr("Succès"), tr("Recommandation supprimée"));
        chargerRecommandations();
        reinitialiserFormulaire();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de la suppression: %1").arg(error));
    }
}

void RecommandationDialog::onRafraichirClicked()
{
    chargerRecommandations();
    reinitialiserFormulaire();
}

void RecommandationDialog::onRechercherClicked()
{
    QString typeSol = m_rechercheEdit->text().trimmed();
    if (typeSol.isEmpty()) {
        chargerRecommandations();
        return;
    }
    
    QString error;
    QList<RecommandationPlante> recommandations = RecommandationPlante::fetchByTypeSol(typeSol, &error);
    
    if (!error.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Erreur de recherche: %1").arg(error));
        return;
    }
    
    m_tableWidget->setRowCount(recommandations.size());
    
    for (int i = 0; i < recommandations.size(); ++i) {
        const RecommandationPlante& rp = recommandations[i];
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(rp.idPlante())));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(rp.typeSol()));
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(rp.nomPlante()));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(rp.recommandation()));
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(rp.compatibilite()));
        m_tableWidget->setItem(i, 5, new QTableWidgetItem(rp.saison()));
    }
    
    if (recommandations.isEmpty()) {
        QMessageBox::information(this, tr("Recherche"), 
            tr("Aucune recommandation trouvée pour le type de sol '%1'").arg(typeSol));
    }
}

void RecommandationDialog::onSelectionChanged()
{
    int row = m_tableWidget->currentRow();
    if (row < 0) return;
    
    m_idEdit->setText(m_tableWidget->item(row, 0)->text());
    
    // Type de sol
    QString typeSol = m_tableWidget->item(row, 1)->text();
    int solIndex = m_typeSolCombo->findText(typeSol);
    if (solIndex >= 0) m_typeSolCombo->setCurrentIndex(solIndex);
    else m_typeSolCombo->setCurrentText(typeSol);
    
    // Nom plante
    m_nomPlanteEdit->setText(m_tableWidget->item(row, 2)->text());
    
    // Recommandation
    m_recommandationEdit->setText(m_tableWidget->item(row, 3)->text());
    
    // Compatibilité
    QString compat = m_tableWidget->item(row, 4)->text();
    int compatIndex = m_compatibiliteCombo->findText(compat);
    if (compatIndex >= 0) m_compatibiliteCombo->setCurrentIndex(compatIndex);
    
    // Saison
    QString saison = m_tableWidget->item(row, 5)->text();
    int saisonIndex = m_saisonCombo->findText(saison);
    if (saisonIndex >= 0) m_saisonCombo->setCurrentIndex(saisonIndex);
}

void RecommandationDialog::reinitialiserFormulaire()
{
    m_idEdit->clear();
    m_typeSolCombo->setCurrentIndex(0);
    m_nomPlanteEdit->clear();
    m_recommandationEdit->clear();
    m_compatibiliteCombo->setCurrentIndex(2); // Moyenne
    m_saisonCombo->setCurrentIndex(0); // Printemps
}
