#include "vehicule.h"
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>

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

Vehicule::Vehicule() {}

Vehicule::Vehicule(QString immat, QString marque, QString modele, QString type, QString etat, QString service, QDate date_maint)
{
    this->immat = immat;
    this->marque = marque;
    this->modele = modele;
    this->type = type;
    this->etat = etat;
    this->service = service;
    this->date_maint = date_maint;
}


// AJOUTER
bool Vehicule::ajouter()
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qCritical() << "Vehicule::ajouter - Connexion base de données invalide";
        return false;
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            qCritical() << "Vehicule::ajouter - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return false;
        }
    }
    
    db.transaction();
    
    // Vérifier si l'immatriculation existe déjà (contrainte unique)
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT 1 FROM GEST_VEHICULE WHERE IMMATRICULATION = :IMMAT");
    checkQuery.bindValue(":IMMAT", immat);
    if (checkQuery.exec() && checkQuery.next()) {
        db.rollback();
        qDebug() << "❌ Erreur: L'immatriculation" << immat << "existe déjà !";
        return false;
    }
    
    // Obtenir le prochain ID disponible
    int nextId = 1;
    QSqlQuery idQuery(db);
    
    // Calculer MAX(ID) + 1
    if (idQuery.exec("SELECT NVL(MAX(ID), 0) + 1 FROM GEST_VEHICULE") && idQuery.next()) {
        nextId = idQuery.value(0).toInt();
    } else {
        // Si échec, essayer avec une séquence si elle existe
        QSqlQuery seqQuery(db);
        if (seqQuery.exec("SELECT SEQ_VEHICULE.NEXTVAL FROM DUAL") && seqQuery.next()) {
            nextId = seqQuery.value(0).toInt();
        }
    }
    
    // Vérifier que l'ID n'existe pas déjà et trouver un ID disponible
    QSqlQuery checkIdQuery(db);
    checkIdQuery.prepare("SELECT 1 FROM GEST_VEHICULE WHERE ID = :ID");
    int attempts = 0;
    while (attempts < 1000) {
        checkIdQuery.bindValue(":ID", nextId);
        if (checkIdQuery.exec() && checkIdQuery.next()) {
            // ID existe déjà, incrémenter
            nextId++;
            attempts++;
        } else {
            // ID disponible
            break;
        }
    }
    
    if (attempts >= 1000) {
        db.rollback();
        qCritical() << "Vehicule::ajouter - Impossible de trouver un ID disponible après 1000 tentatives !";
        return false;
    }
    
    // Insérer avec ID comme clé primaire et DATE_MAINTENANCE
    QSqlQuery query(db);
    query.prepare("INSERT INTO GEST_VEHICULE "
                  "(ID, IMMATRICULATION, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINTENANCE) "
                  "VALUES (:ID, :IMMAT, :MARQUE, :MODELE, :TYPE, :ETAT, :SERVICE, :DATE_MAINT)");
    
    query.bindValue(":ID", nextId);
    query.bindValue(":IMMAT", immat);
    query.bindValue(":MARQUE", marque);
    query.bindValue(":MODELE", modele);
    query.bindValue(":TYPE", type);
    query.bindValue(":ETAT", etat);
    query.bindValue(":SERVICE", service);
    query.bindValue(":DATE_MAINT", date_maint);

    if (!query.exec()) {
        QString error = query.lastError().text();
        QString nativeCode = query.lastError().nativeErrorCode();
        db.rollback();
        qDebug() << "❌ SQL Error (ajouter):" << error;
        qDebug() << "   Code erreur:" << nativeCode;
        qDebug() << "   Immatriculation:" << immat;
        qDebug() << "   ID tenté:" << nextId;
        
        // Si erreur de contrainte unique sur IMMATRICULATION (ORA-00001)
        if (error.contains("unique constraint", Qt::CaseInsensitive) || nativeCode == "1") {
            if (error.contains("IMMATRICULATION", Qt::CaseInsensitive)) {
                qDebug() << "   → L'immatriculation existe déjà !";
            } else if (error.contains("ID", Qt::CaseInsensitive) || error.contains("GEST_VEHICULE_PK", Qt::CaseInsensitive)) {
                qDebug() << "   → L'ID existe déjà !";
            }
        }
        // Si erreur de colonne inexistante (ORA-00904)
        else if (error.contains("invalid identifier", Qt::CaseInsensitive) || nativeCode == "904") {
            qDebug() << "   → Colonne inexistante dans la table GEST_VEHICULE";
            qDebug() << "   → Vérifiez que la table a les colonnes: ID, IMMATRICULATION, DATE_MAINTENANCE";
        }
        
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        qCritical() << "Vehicule::ajouter - Erreur commit:" << error;
        return false;
    }
    
    id_vehi = nextId;
    qDebug() << "✅ Véhicule ajouté avec succès. ID:" << nextId << "Immatriculation:" << immat;
    return true;
}

// AFFICHER
QSqlQuery Vehicule::afficher()
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qWarning() << "Vehicule::afficher - Connexion base de données invalide";
        return QSqlQuery(db);
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            qWarning() << "Vehicule::afficher - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return QSqlQuery(db);
        }
    }
    
    QSqlQuery query(db);
    query.prepare("SELECT ID, IMMATRICULATION, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINTENANCE FROM GEST_VEHICULE ORDER BY ID DESC");
    
    if (!query.exec()) {
        qWarning() << "Vehicule::afficher - Erreur:" << query.lastError().text();
        qWarning() << "   Code erreur:" << query.lastError().nativeErrorCode();
    }
    return query;
}

// MODIFIER
bool Vehicule::modifier(QString oldImmat)
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qCritical() << "Vehicule::modifier - Connexion base de données invalide";
        return false;
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            qCritical() << "Vehicule::modifier - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return false;
        }
    }
    
    db.transaction();
    
    // Vérifier que l'ancienne immatriculation existe et récupérer son ID
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT ID FROM GEST_VEHICULE WHERE IMMATRICULATION = :OLD_IMMAT");
    checkQuery.bindValue(":OLD_IMMAT", oldImmat);
    
    int vehicleId = 0;
    if (!checkQuery.exec()) {
        QString error = checkQuery.lastError().text();
        db.rollback();
        qDebug() << "❌ Erreur lors de la vérification de l'immatriculation:" << error;
        qDebug() << "   Immatriculation recherchée:" << oldImmat;
        return false;
    }
    
    if (!checkQuery.next()) {
        db.rollback();
        qDebug() << "❌ Erreur: L'immatriculation" << oldImmat << "n'existe pas !";
        return false;
    }
    
    vehicleId = checkQuery.value(0).toInt();
    if (vehicleId <= 0) {
        db.rollback();
        qDebug() << "❌ Erreur: ID invalide récupéré pour l'immatriculation" << oldImmat;
        return false;
    }
    
    // Si la nouvelle immatriculation est différente, vérifier qu'elle n'existe pas déjà
    if (immat != oldImmat) {
        QSqlQuery checkNewQuery(db);
        checkNewQuery.prepare("SELECT 1 FROM GEST_VEHICULE WHERE IMMATRICULATION = :IMMAT");
        checkNewQuery.bindValue(":IMMAT", immat);
        if (checkNewQuery.exec() && checkNewQuery.next()) {
            db.rollback();
            qDebug() << "❌ Erreur: L'immatriculation" << immat << "existe déjà !";
            return false;
        }
    }
    
    // Mettre à jour en utilisant ID comme clé primaire
    QSqlQuery query(db);
    query.prepare("UPDATE GEST_VEHICULE SET "
                  "IMMATRICULATION = :IMMAT, MARQUE = :MARQUE, MODELE = :MODELE, TYPE = :TYPE, "
                  "ETAT = :ETAT, SERVICE = :SERVICE, DATE_MAINTENANCE = :DATE_MAINT "
                  "WHERE ID = :ID");
    
    query.bindValue(":ID", vehicleId);
    query.bindValue(":IMMAT", immat);
    query.bindValue(":MARQUE", marque);
    query.bindValue(":MODELE", modele);
    query.bindValue(":TYPE", type);
    query.bindValue(":ETAT", etat);
    query.bindValue(":SERVICE", service);
    query.bindValue(":DATE_MAINT", date_maint);

    if (!query.exec()) {
        QString error = query.lastError().text();
        QString nativeCode = query.lastError().nativeErrorCode();
        db.rollback();
        qDebug() << "❌ SQL Error (modifier):" << error;
        qDebug() << "   Code erreur:" << nativeCode;
        qDebug() << "   Ancienne immatriculation:" << oldImmat;
        qDebug() << "   Nouvelle immatriculation:" << immat;
        qDebug() << "   ID véhicule:" << vehicleId;
        
        // Si erreur de contrainte unique (ORA-00001)
        if (error.contains("unique constraint", Qt::CaseInsensitive) || nativeCode == "1") {
            qDebug() << "   → La nouvelle immatriculation existe déjà !";
        }
        
        return false;
    }

    // Vérifier que des lignes ont été affectées
    int rowsAffected = query.numRowsAffected();
    if (rowsAffected == 0) {
        db.rollback();
        qDebug() << "⚠️ Aucune ligne modifiée pour ID:" << vehicleId;
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        qCritical() << "Vehicule::modifier - Erreur commit:" << error;
        return false;
    }

    qDebug() << "✅ Véhicule modifié. ID:" << vehicleId << oldImmat << "→" << immat << "(" << rowsAffected << "ligne(s) affectée(s))";
    return true;
}

// SUPPRIMER
bool Vehicule::supprimer(QString immat)
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qCritical() << "Vehicule::supprimer - Connexion base de données invalide";
        return false;
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            qCritical() << "Vehicule::supprimer - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return false;
        }
    }
    
    db.transaction();
    
    // Récupérer l'ID du véhicule à partir de l'immatriculation
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT ID FROM GEST_VEHICULE WHERE IMMATRICULATION = :IMMAT");
    checkQuery.bindValue(":IMMAT", immat);
    
    int vehicleId = 0;
    if (!checkQuery.exec() || !checkQuery.next()) {
        db.rollback();
        qDebug() << "❌ Erreur: L'immatriculation" << immat << "n'existe pas !";
        return false;
    }
    
    vehicleId = checkQuery.value(0).toInt();
    if (vehicleId <= 0) {
        db.rollback();
        qDebug() << "❌ Erreur: ID invalide récupéré pour l'immatriculation" << immat;
        return false;
    }
    
    // Supprimer en utilisant ID comme clé primaire
    QSqlQuery query(db);
    query.prepare("DELETE FROM GEST_VEHICULE WHERE ID = :ID");
    query.bindValue(":ID", vehicleId);

    if (!query.exec()) {
        QString error = query.lastError().text();
        db.rollback();
        qDebug() << "❌ SQL Error (supprimer):" << error;
        qDebug() << "   Code erreur:" << query.lastError().nativeErrorCode();
        return false;
    }
    
    // Vérifier que des lignes ont été affectées
    int rowsAffected = query.numRowsAffected();
    if (rowsAffected == 0) {
        db.rollback();
        qDebug() << "⚠️ Aucune ligne supprimée pour ID:" << vehicleId;
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        qCritical() << "Vehicule::supprimer - Erreur commit:" << error;
        return false;
    }

    qDebug() << "✅ Véhicule supprimé. ID:" << vehicleId << "Immatriculation:" << immat << "(" << rowsAffected << "ligne(s) affectée(s))";
    return true;
}
