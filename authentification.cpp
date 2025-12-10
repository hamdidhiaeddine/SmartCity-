#include "authentification.h"
#include "ui_authentification.h"
#include "facialrecognition.h"
#include "facecapturedialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

Authentification::Authentification(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Authentification)
    , m_authentifie(false)
{
    ui->setupUi(this);
    
    // Connexion des boutons
    connect(ui->add_employee_btn_3, &QPushButton::clicked, this, &Authentification::onConnexionClicked);
    connect(ui->add_employee_btn_4, &QPushButton::clicked, this, &Authentification::onInscriptionClicked);
    connect(ui->motdepasseoublier, &QLabel::linkActivated, this, &Authentification::onMotDePasseOublieClicked);
    connect(ui->motdepasseoublier_2, &QLabel::linkActivated, this, &Authentification::onInscriptionClicked);
    
    // Connexion du bouton reconnaissance faciale
    if (ui->connexionVisageBtn) {
        connect(ui->connexionVisageBtn, &QPushButton::clicked, this, &Authentification::onConnexionVisage);
    }
    
    // Configuration des champs
    ui->lineEdit->setPlaceholderText("Identifiant");
    ui->lineEdit_2->setPlaceholderText("Mot de passe");
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
}

Authentification::~Authentification()
{
    delete ui;
}

QString Authentification::getIdentifiant() const
{
    return ui->lineEdit->text();
}

QString Authentification::getMotDePasse() const
{
    return ui->lineEdit_2->text();
}

void Authentification::onConnexionClicked()
{
    QString identifiant = ui->lineEdit->text().trimmed();
    QString motDePasse = ui->lineEdit_2->text().trimmed();
    
    if (identifiant.isEmpty() || motDePasse.isEmpty()) {
        QMessageBox::warning(this, "Champs vides", 
                           "Veuillez remplir l'identifiant et le mot de passe.");
        return;
    }
    
    if (validerConnexion(identifiant, motDePasse)) {
        m_authentifie = true;
        accept(); // Ferme le dialog avec succès
    } else {
        QMessageBox::critical(this, "Échec de connexion", 
                            "Identifiant ou mot de passe incorrect.");
        ui->lineEdit_2->clear();
        ui->lineEdit_2->setFocus();
    }
}

void Authentification::onInscriptionClicked()
{
    // Utiliser ce bouton pour la reconnaissance faciale au lieu de l'inscription
    if (validerConnexionVisage()) {
        m_authentifie = true;
        accept(); // Ferme le dialog avec succès
    }
}

void Authentification::onMotDePasseOublieClicked()
{
    QMessageBox::information(this, "Mot de passe oublié", 
                           "Fonctionnalité de récupération à venir.\n"
                           "Contactez l'administrateur pour réinitialiser votre mot de passe.");
}

bool Authentification::validerConnexion(const QString &identifiant, const QString &motDePasse)
{
    // Vérification dans la table EMPLOYES (nom + password)
    QSqlQuery queryEmploye;
    queryEmploye.prepare("SELECT ID_EMPLOYE, NOM, PRENOM, EMAIL, POSTE "
                  "FROM EMPLOYES "
                  "WHERE UPPER(NOM) = UPPER(:nom) AND PASSWORD = :password");
    queryEmploye.bindValue(":nom", identifiant);
    queryEmploye.bindValue(":password", motDePasse);
    
    if (queryEmploye.exec() && queryEmploye.next()) {
        int userId = queryEmploye.value("ID_EMPLOYE").toInt();
        QString nom = queryEmploye.value("NOM").toString();
        QString prenom = queryEmploye.value("PRENOM").toString();
        QString email = queryEmploye.value("EMAIL").toString();
        QString poste = queryEmploye.value("POSTE").toString();
        
        qDebug() << "✓ Connexion réussie (Employé):";
        qDebug() << "  - ID:" << userId;
        qDebug() << "  - Nom:" << nom << prenom;
        qDebug() << "  - Email:" << email;
        qDebug() << "  - Poste:" << poste;
        
        return true;
    }
    
    // Fallback: Vérification dans la table USERS
    QSqlQuery query;
    query.prepare("SELECT ID_USER, NOM, PRENOM, ROLE, ACTIF "
                  "FROM USERS "
                  "WHERE LOGIN = :login AND MOT_DE_PASSE = :password");
    query.bindValue(":login", identifiant);
    query.bindValue(":password", motDePasse);
    
    if (!query.exec()) {
        qWarning() << "Erreur SQL lors de l'authentification:" << query.lastError().text();
        
        // Fallback sur les comptes par défaut si la table n'existe pas
        if (identifiant == "admin" && motDePasse == "admin") {
            qDebug() << "Connexion admin réussie (mode fallback)";
            return true;
        }
        if (identifiant == "hiba" && motDePasse == "esprit18") {
            qDebug() << "Connexion hiba réussie (mode fallback)";
            return true;
        }
        
        return false;
    }
    
    if (query.next()) {
        int actif = query.value("ACTIF").toInt();
        
        if (actif == 0) {
            qWarning() << "Compte désactivé pour:" << identifiant;
            return false;
        }
        
        int userId = query.value("ID_USER").toInt();
        QString nom = query.value("NOM").toString();
        QString prenom = query.value("PRENOM").toString();
        QString role = query.value("ROLE").toString();
        
        qDebug() << "✓ Connexion réussie (User):";
        qDebug() << "  - ID:" << userId;
        qDebug() << "  - Nom:" << nom << prenom;
        qDebug() << "  - Rôle:" << role;
        
        return true;
    }
    
    qDebug() << "Échec authentification - Identifiant ou mot de passe incorrect:" << identifiant;
    return false;
}

void Authentification::onConnexionVisage()
{
    // Appeler directement la validation par visage
    if (validerConnexionVisage()) {
        m_authentifie = true;
        accept(); // Ferme le dialog avec succès
    }
}

bool Authentification::validerConnexionVisage()
{
    // Ouvrir le dialogue de capture
    FaceCaptureDialog dialog(this);
    dialog.setWindowTitle("Authentification par Visage");
    
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }
    
    QImage capturedImage = dialog.getCapturedImage();
    
    if (capturedImage.isNull()) {
        QMessageBox::warning(this, "Erreur", "Aucune image capturée.");
        return false;
    }
    
    QString errorText;
    int employeId = FacialRecognition::authenticateByFace(capturedImage, errorText);
    
    if (employeId > 0) {
        // Récupérer les informations de l'employé
        QSqlQuery query;
        query.prepare("SELECT NOM, PRENOM, POSTE FROM EMPLOYES WHERE ID_EMPLOYE = :id");
        query.bindValue(":id", employeId);
        
        if (query.exec() && query.next()) {
            QString nom = query.value(0).toString();
            QString prenom = query.value(1).toString();
            QString poste = query.value(2).toString();
            
            qDebug() << "✓ Connexion réussie par visage:";
            qDebug() << "  - ID:" << employeId;
            qDebug() << "  - Nom:" << nom << prenom;
            qDebug() << "  - Poste:" << poste;
            
            QMessageBox::information(this, "Connexion Réussie", 
                QString("Bienvenue %1 %2 !\n\nPoste: %3")
                .arg(nom, prenom, poste));
            
            return true;
        }
    }
    
    QMessageBox::warning(this, "Échec", 
        "Authentification échouée:\n" + errorText);
    return false;
}
