#include "authentification.h"
#include "ui_authentification.h"
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
    connect(ui->bypass, &QPushButton::clicked, this, &Authentification::onBypassClicked);
    
    // Configuration des champs
    ui->lineEdit->setPlaceholderText("Identifiant");
    ui->lineEdit_2->setPlaceholderText("Mot de passe");
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
}

void Authentification::onBypassClicked()
{
    qDebug() << "Authentification bypassée via le bouton 'bypass'";
    m_authentifie = true;
    accept();
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
    QMessageBox::information(this, "Inscription", 
                           "Fonctionnalité d'inscription à venir.\n"
                           "Contactez l'administrateur pour créer un compte.");
}

void Authentification::onMotDePasseOublieClicked()
{
    QMessageBox::information(this, "Mot de passe oublié", 
                           "Fonctionnalité de récupération à venir.\n"
                           "Contactez l'administrateur pour réinitialiser votre mot de passe.");
}

bool Authentification::validerConnexion(const QString &identifiant, const QString &motDePasse)
{
    // Vérification dans la table USERS
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
        
        qDebug() << "✓ Connexion réussie:";
        qDebug() << "  - ID:" << userId;
        qDebug() << "  - Nom:" << nom << prenom;
        qDebug() << "  - Rôle:" << role;
        
        return true;
    }
    
    qDebug() << "Échec authentification - Identifiant ou mot de passe incorrect:" << identifiant;
    return false;
}
