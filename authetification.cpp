#include "authetification.h"
#include "ui_authentification.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRect>
#include <QPixmap>

AUTHETIFICATION::AUTHETIFICATION(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Authentification");
    
    // Configurer le background avec l'image depuis les ressources
    QLabel *backgroundLabel = findChild<QLabel*>("label");
    if (backgroundLabel) {
        // Charger l'image depuis les ressources
        QPixmap bgPixmap(":/images/bg.png");
        if (!bgPixmap.isNull()) {
            // Configurer le label pour afficher l'image
            backgroundLabel->setPixmap(bgPixmap);
            backgroundLabel->setScaledContents(true);
            // Assurer que le label couvre toute la fenêtre
            backgroundLabel->setGeometry(0, 0, width(), height());
            // Mettre le background en arrière-plan (derrière tous les autres widgets)
            backgroundLabel->lower();
        } else {
            // Si l'image n'est pas trouvée, utiliser un fond de couleur
            backgroundLabel->setStyleSheet("background-color: qlineargradient(spread:repeat, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(9, 130, 230 ,10) stop:1 rgba(255, 255, 255, 255));");
        }
    }
    
    // Charger l'image utilisateur/mot de passe depuis les ressources
    QLabel *userIconLabel = findChild<QLabel*>("label_3");
    if (userIconLabel) {
        // Charger l'image phot.png depuis les ressources
        QPixmap userPixmap(":/images/phot.png");
        if (!userPixmap.isNull()) {
            userIconLabel->setPixmap(userPixmap);
            userIconLabel->setScaledContents(true);
        }
    }
    
    // Charger le logo depuis les ressources
    QLabel *logoLabel = findChild<QLabel*>("label_2");
    if (logoLabel) {
        // Le logo est déjà chargé via Lg.png dans mainwindow, mais on peut aussi charger depuis ressources si nécessaire
        // Pour l'instant, on utilise le logo depuis les ressources si disponible
        QPixmap logoPixmap(":/images/Lg.png");
        if (!logoPixmap.isNull()) {
            logoLabel->setPixmap(logoPixmap);
            logoLabel->setScaledContents(true);
        }
    }
    
    // Connecter les signaux pour la validation en temps réel
    QLineEdit *lineEdit = findChild<QLineEdit*>("lineEdit");
    QLineEdit *lineEdit_2 = findChild<QLineEdit*>("lineEdit_2");
    QPushButton *btnConnexion = findChild<QPushButton*>("add_employee_btn_3");
    
    if (lineEdit) {
        connect(lineEdit, &QLineEdit::textChanged, this, &AUTHETIFICATION::onIdentifiantChanged);
        // Supprimer le placeholder texte par défaut
        if (lineEdit->text() == "identifiant") {
            lineEdit->clear();
            lineEdit->setPlaceholderText("Identifiant");
        }
        lineEdit->setEchoMode(QLineEdit::Normal);
    }
    
    if (lineEdit_2) {
        connect(lineEdit_2, &QLineEdit::textChanged, this, &AUTHETIFICATION::onMotDePasseChanged);
        // Supprimer le placeholder texte par défaut
        if (lineEdit_2->text() == "Mot de passe ") {
            lineEdit_2->clear();
            lineEdit_2->setPlaceholderText("Mot de passe");
        }
        lineEdit_2->setEchoMode(QLineEdit::Password);
    }
    
    if (btnConnexion) {
        connect(btnConnexion, &QPushButton::clicked, this, &AUTHETIFICATION::onConnexion);
    }
}

AUTHETIFICATION::~AUTHETIFICATION()
{
    delete ui;
}

void AUTHETIFICATION::onConnexion()
{
    QLineEdit *lineEdit = findChild<QLineEdit*>("lineEdit");
    QLineEdit *lineEdit_2 = findChild<QLineEdit*>("lineEdit_2");
    
    if (!lineEdit || !lineEdit_2) {
        QMessageBox::warning(this, "Erreur", "Champs de saisie introuvables");
        return;
    }
    
    AuthInput input;
    input.identifiant = lineEdit->text();
    input.motDePasse = lineEdit_2->text();
    
    AuthValidationResult result = InputController::validateAuth(input);
    
    if (!result.ok) {
        // Afficher les erreurs
        QStringList erreurs;
        if (result.fieldErrors.contains("identifiant")) {
            erreurs << "Identifiant: " + result.fieldErrors["identifiant"];
            afficherErreur("identifiant", result.fieldErrors["identifiant"]);
        } else {
            effacerErreur("identifiant");
        }
        
        if (result.fieldErrors.contains("motDePasse")) {
            erreurs << "Mot de passe: " + result.fieldErrors["motDePasse"];
            afficherErreur("motDePasse", result.fieldErrors["motDePasse"]);
        } else {
            effacerErreur("motDePasse");
        }
        
        QMessageBox::warning(this, "Erreur de validation", 
                           "Veuillez corriger les erreurs suivantes:\n\n" + erreurs.join("\n"));
        return;
    }
    
    // Si la validation réussit, effacer les erreurs
    effacerErreur("identifiant");
    effacerErreur("motDePasse");
    
    // Ici, vous pouvez ajouter la logique de connexion réelle
    // (vérification des identifiants dans la base de données, etc.)
    // Pour l'instant, on accepte juste le dialogue si la validation est OK
    // Le message de succès sera affiché dans main.cpp après accept()
    
    accept(); // Ferme le dialogue avec succès (retourne QDialog::Accepted)
}

void AUTHETIFICATION::onIdentifiantChanged()
{
    QLineEdit *lineEdit = findChild<QLineEdit*>("lineEdit");
    if (!lineEdit) return;
    
    QString identifiant = lineEdit->text();
    QString error;
    
    if (InputController::validateIdentifiant(identifiant, error)) {
        effacerErreur("identifiant");
    } else if (!identifiant.isEmpty()) {
        // Afficher l'erreur seulement si le champ n'est pas vide
        afficherErreur("identifiant", error);
    } else {
        effacerErreur("identifiant");
    }
}

void AUTHETIFICATION::onMotDePasseChanged()
{
    QLineEdit *lineEdit_2 = findChild<QLineEdit*>("lineEdit_2");
    if (!lineEdit_2) return;
    
    QString motDePasse = lineEdit_2->text();
    QString error;
    
    if (InputController::validateMotDePasse(motDePasse, error)) {
        effacerErreur("motDePasse");
    } else if (!motDePasse.isEmpty()) {
        // Afficher l'erreur seulement si le champ n'est pas vide
        afficherErreur("motDePasse", error);
    } else {
        effacerErreur("motDePasse");
    }
}

void AUTHETIFICATION::validerChamps()
{
    onIdentifiantChanged();
    onMotDePasseChanged();
}

void AUTHETIFICATION::afficherErreur(const QString &champ, const QString &message)
{
    // Créer ou mettre à jour un label d'erreur pour le champ
    QLabel *errorLabel = findChild<QLabel*>(champ + "_error");
    
    if (!errorLabel) {
        // Créer un nouveau label d'erreur
        errorLabel = new QLabel(this);
        errorLabel->setObjectName(champ + "_error");
        errorLabel->setStyleSheet("color: red; font-size: 10px;");
        
        // Positionner le label sous le champ correspondant
        QLineEdit *lineEdit = nullptr;
        if (champ == "identifiant") {
            lineEdit = findChild<QLineEdit*>("lineEdit");
        } else if (champ == "motDePasse") {
            lineEdit = findChild<QLineEdit*>("lineEdit_2");
        }
        
        if (lineEdit) {
            QRect geom = lineEdit->geometry();
            errorLabel->setGeometry(geom.x(), geom.y() + geom.height() + 5, geom.width(), 15);
        }
    }
    
    errorLabel->setText(message);
    errorLabel->show();
}

void AUTHETIFICATION::effacerErreur(const QString &champ)
{
    QLabel *errorLabel = findChild<QLabel*>(champ + "_error");
    if (errorLabel) {
        errorLabel->hide();
        errorLabel->clear();
    }
}
