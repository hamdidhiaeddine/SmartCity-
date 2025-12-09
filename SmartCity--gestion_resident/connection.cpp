#include "connection.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

Connection::Connection()
{

}

bool Connection::createconnect()
{
    bool test = false;
    
    // Supprimer la connexion existante si elle existe
    if (QSqlDatabase::contains("qt_oracle")) {
        QSqlDatabase::removeDatabase("qt_oracle");
    }
    
    // Créer une connexion NOMMÉE "qt_oracle"
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "qt_oracle");
    
    // ========================================
    // OPTION 1 : Connexion via DSN (avec credentials explicites)
    // ========================================
    /*
    db.setDatabaseName("source_projet2A");
    db.setUserName("wala");
    db.setPassword("esprit18");
    */
    
    // ========================================
    // OPTION 2 : Connexion directe SANS DSN (plus fiable)
    // ========================================
    db.setDatabaseName("DRIVER={Oracle in instantclient_21_19};"
                       "DBQ=localhost:1521/XE;"
                       "UID=wala;"
                       "PWD=esprit18;");
    
    if (!db.open()) {
        qDebug() << "❌ Database connection failed:" << db.lastError().text();
        qDebug() << "💡 Vérifiez :";
        qDebug() << "   1. La source ODBC 'source_projet2A' est configurée (odbcad32.exe)";
        qDebug() << "   2. Le service Oracle est démarré";
        qDebug() << "   3. Les credentials sont corrects (wala/esprit18)";
        qDebug() << "   📖 Voir CONFIGURATION_ODBC.md pour aide détaillée";
        return false;
    }

    qDebug() << "✅ Database connected successfully.";
    test = true;

    // Force autocommit (so all INSERT/UPDATE/DELETE persist automatically)
    db.setConnectOptions("AUTOCOMMIT=ON");

    return test;
}

bool Connection::createTableAlertes()
{
    if (!QSqlDatabase::contains("qt_oracle")) {
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
        "    ID NUMBER PRIMARY KEY,"
        "    ID_MAISON NUMBER NOT NULL,"
        "    ZONE VARCHAR2(200),"
        "    NIVEAU NUMBER,"
        "    STATUT VARCHAR2(50) DEFAULT 'En attente',"
        "    DATE_ALERTE DATE DEFAULT SYSDATE,"
        "    CONSTRAINT FK_ALERTE_MAISON FOREIGN KEY (ID_MAISON) REFERENCES GEST_MAISON(ID) ON DELETE CASCADE"
        ")";

    if (!query.exec(createTableSQL)) {
        qDebug() << "Erreur création table:" << query.lastError().text();
        return false;
    }

    // Créer la séquence
    if (!query.exec("CREATE SEQUENCE SEQ_ALERTES START WITH 1 INCREMENT BY 1 NOCACHE NOCYCLE")) {
        qDebug() << "Erreur création séquence:" << query.lastError().text();
    }

    // Créer le trigger
    QString createTriggerSQL = 
        "CREATE OR REPLACE TRIGGER TRG_ALERTE_ID "
        "BEFORE INSERT ON GEST_ALERTES "
        "FOR EACH ROW "
        "BEGIN "
        "    IF :NEW.ID IS NULL THEN "
        "        SELECT SEQ_ALERTES.NEXTVAL INTO :NEW.ID FROM DUAL; "
        "    END IF; "
        "END;";

    if (!query.exec(createTriggerSQL)) {
        qDebug() << "Erreur création trigger:" << query.lastError().text();
    }

    qDebug() << "✅ Table GEST_ALERTES créée avec succès";
    return true;
}

bool Connection::isConnected()
{
    if (!QSqlDatabase::contains("qt_oracle"))
        return false;

    QSqlDatabase db = QSqlDatabase::database("qt_oracle");
    return db.isValid() && db.isOpen();
}

bool Connection::reconnect()
{
    if (isConnected())
        return true;

    return createconnect();
}
