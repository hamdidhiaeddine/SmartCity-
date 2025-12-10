#ifndef RECOMMANDATIONDIALOG_H
#define RECOMMANDATIONDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include "recommandationplante.h"

class RecommandationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecommandationDialog(QWidget *parent = nullptr);
    ~RecommandationDialog();

private slots:
    void onAjouterClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onRafraichirClicked();
    void onRechercherClicked();
    void onSelectionChanged();
    void onGenererAleatoireClicked();

private:
    void setupUI();
    void chargerRecommandations();
    void reinitialiserFormulaire();

    QTableWidget* m_tableWidget;
    QLineEdit* m_idEdit;
    QComboBox* m_typeSolCombo;
    QLineEdit* m_nomPlanteEdit;
    QTextEdit* m_recommandationEdit;
    QComboBox* m_compatibiliteCombo;
    QComboBox* m_saisonCombo;
    QLineEdit* m_rechercheEdit;
    
    QPushButton* m_btnAjouter;
    QPushButton* m_btnModifier;
    QPushButton* m_btnSupprimer;
    QPushButton* m_btnRafraichir;
    QPushButton* m_btnRechercher;
    QPushButton* m_btnGenererAleatoire;
};

#endif // RECOMMANDATIONDIALOG_H
