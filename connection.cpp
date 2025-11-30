#include "connection.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

Connection::Connection()
{
}

bool Connection::createconnect()
{
    // Supprimer proprement toute connexion existante
    if (QSqlDatabase::contains("qt_oracle")) {
        QSqlDatabase dbOld = QSqlDatabase::database("qt_oracle");
        if (dbOld.isOpen())
            dbOld.close();
        QSqlDatabase::removeDatabase("qt_oracle");
    }

    // Créer une nouvelle connexion NOMMÉE
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "qt_oracle");

    // Chaîne complète Oracle (OBLIGATOIRE avec ODBC)
    db.setDatabaseName(
        "DRIVER={Oracle in XE};"
        "DBQ=XE;"
        "UID=mariem;"
        "PWD=test1234;"
        );

    // Tester la connexion
    if (db.open()) {
        qDebug() << "=== Database connection successful ===";
        return true;
    } else {
        qDebug() << "=== Database connection FAILED ===";
        qDebug() << "Error:" << db.lastError().text();
        return false;
    }
}

bool Connection::isConnected()
{
    if (!QSqlDatabase::contains("qt_oracle"))
        return false;

    // ATTENTION : connexion NOMMÉE
    QSqlDatabase db = QSqlDatabase::database("qt_oracle");
    return db.isValid() && db.isOpen();
}

bool Connection::reconnect()
{
    if (isConnected())
        return true;

    return createconnect();
}

bool Connection::createTableAlertes()
{
    if (!isConnected()) {
        qDebug() << "Pas de connexion à la base de données";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database("qt_oracle");
    QSqlQuery query(db);

    // Vérifier si la table existe déjà
    query.prepare("SELECT COUNT(*) FROM USER_TABLES WHERE TABLE_NAME = 'GEST_ALERTES'");
    if (query.exec() && query.next()) {
        if (query.value(0).toInt() > 0) {
            qDebug() << "Table GEST_ALERTES existe déjà";
            return true;
        }
    }

    qDebug() << "Création de la table GEST_ALERTES...";

    // Créer la table
    QString createTableSQL =
        "CREATE TABLE GEST_ALERTES ("
        "    ID NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,"
        "    ID_MAISON NUMBER NOT NULL,"
        "    ZONE VARCHAR2(200),"
        "    NIVEAU NUMBER,"
        "    STATUT VARCHAR2(50) DEFAULT 'En attente',"
        "    DATE_ALERTE DATE DEFAULT SYSDATE"
        ")";

    if (!query.exec(createTableSQL)) {
        // Si IDENTITY n'est pas supporté, utiliser la version avec séquence
        qDebug() << "Tentative avec séquence (ancienne version Oracle)...";

        if (!query.exec("CREATE TABLE GEST_ALERTES ("
                        "    ID NUMBER PRIMARY KEY,"
                        "    ID_MAISON NUMBER NOT NULL,"
                        "    ZONE VARCHAR2(200),"
                        "    NIVEAU NUMBER,"
                        "    STATUT VARCHAR2(50) DEFAULT 'En attente',"
                        "    DATE_ALERTE DATE DEFAULT SYSDATE"
                        ")")) {
            qDebug() << "Erreur création table:" << query.lastError().text();
            return false;
        }

        // Créer la séquence
        query.exec("CREATE SEQUENCE GEST_ALERTES_SEQ START WITH 1 INCREMENT BY 1");

        // Créer le trigger
        query.exec("CREATE OR REPLACE TRIGGER GEST_ALERTES_TRG "
                   "BEFORE INSERT ON GEST_ALERTES "
                   "FOR EACH ROW "
                   "BEGIN "
                   "    IF :NEW.ID IS NULL THEN "
                   "        SELECT GEST_ALERTES_SEQ.NEXTVAL INTO :NEW.ID FROM DUAL; "
                   "    END IF; "
                   "END");
    }

    // Créer les index
    query.exec("CREATE INDEX IDX_ALERTE_MAISON ON GEST_ALERTES(ID_MAISON)");
    query.exec("CREATE INDEX IDX_ALERTE_STATUT ON GEST_ALERTES(STATUT)");

    qDebug() << "✓ Table GEST_ALERTES créée avec succès!";
    return true;
}

bool Connection::createOrPatchTableMaisons()
{
    if (!isConnected()) {
        return false;
    }
    QSqlDatabase db = QSqlDatabase::database("qt_oracle");
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM USER_TABLES WHERE TABLE_NAME = 'GEST_MAISON'");
    if (query.exec() && query.next()) {
        if (query.value(0).toInt() == 0) {
            QString createSql = "CREATE TABLE GEST_MAISON (ID NUMBER PRIMARY KEY, ADRESSE VARCHAR2(200), SECURITE NUMBER, STATUS VARCHAR2(50), TYPE VARCHAR2(50), NBRDESPIECES NUMBER)";
            if (!query.exec(createSql)) {
                return false;
            }
            return true;
        }
    }
    auto ensureColumn = [&](const QString &name, const QString &type){
        QSqlQuery qc(db);
        qc.prepare("SELECT COUNT(*) FROM USER_TAB_COLUMNS WHERE TABLE_NAME='GEST_MAISON' AND COLUMN_NAME=:c");
        qc.bindValue(":c", name);
        if (qc.exec() && qc.next()) {
            if (qc.value(0).toInt() == 0) {
                QString alterSql = QString("ALTER TABLE GEST_MAISON ADD %1 %2").arg(name, type);
                QSqlQuery qa(db);
                if (!qa.exec(alterSql)) {
                    return false;
                }
            }
        }
        return true;
    };
    if (!ensureColumn("ADRESSE", "VARCHAR2(200)")) return false;
    if (!ensureColumn("SECURITE", "NUMBER")) return false;
    if (!ensureColumn("STATUS", "VARCHAR2(50)")) return false;
    if (!ensureColumn("TYPE", "VARCHAR2(50)")) return false;
    if (!ensureColumn("NBRDESPIECES", "NUMBER")) return false;
    return true;
}

