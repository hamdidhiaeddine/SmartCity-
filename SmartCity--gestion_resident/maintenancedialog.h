#ifndef MAINTENANCEDIALOG_H
#define MAINTENANCEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include "maintenance.h"

class MaintenanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaintenanceDialog(QWidget *parent = nullptr);
    ~MaintenanceDialog();

private slots:
    void onAjouterClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onRafraichirClicked();
    void onSelectionChanged();
    void onFiltrerParJardinClicked();

private:
    void setupUI();
    void chargerMaintenances();
    void chargerJardins();
    void reinitialiserFormulaire();

    QTableWidget* m_tableWidget;
    QLineEdit* m_idEdit;
    QComboBox* m_jardinCombo;
    QComboBox* m_typeCombo;
    QLineEdit* m_descriptionEdit;
    QDateEdit* m_dateEffectueeEdit;
    QDateEdit* m_dateRecommandeeEdit;
    QComboBox* m_statutCombo;
    QComboBox* m_frequenceCombo;
    QComboBox* m_filtreJardinCombo;
    
    QPushButton* m_btnAjouter;
    QPushButton* m_btnModifier;
    QPushButton* m_btnSupprimer;
    QPushButton* m_btnRafraichir;
    QPushButton* m_btnFiltrer;
    
    QMap<int, QString> m_jardinsMap; // ID -> Nom
};

#endif // MAINTENANCEDIALOG_H
