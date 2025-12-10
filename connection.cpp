#include "connection.h"
#include <QDebug>
#include <QSqlError>

Connection::Connection()
{

}

bool Connection::createconnect()
{
    bool test = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    
    // Connexion directe avec le bon driver Oracle 64-bit
    db.setDatabaseName("Driver={Oracle in instantclient_21_19};Dbq=localhost:1521/xe;Uid=wala;Pwd=esprit18;");
    
    // Alternative avec DSN (si la source ODBC est bien configurée)
    // db.setDatabaseName("source_projet2A");
    // db.setUserName("wala");
    // db.setPassword("esprit18");

    if (db.open()) {
        qDebug() << "✓ Connexion à la base de données réussie !";
        qDebug() << "  - Utilisateur :" << db.userName();
        qDebug() << "  - Base :" << db.databaseName();
        test = true;
    } else {
        qDebug() << "✗ ERREUR DE CONNEXION À LA BASE DE DONNÉES :";
        qDebug() << "  - Message :" << db.lastError().text();
        qDebug() << "  - Type :" << db.lastError().type();
        qDebug() << "  - Code :" << db.lastError().nativeErrorCode();
        qDebug() << "";
        qDebug() << "Vérifiez :";
        qDebug() << "  1. La source ODBC 'source_projet2A' existe (ou utilisez connexion directe)";
        qDebug() << "  2. L'utilisateur 'wala' existe dans Oracle avec le mot de passe 'esprit18'";
        qDebug() << "  3. Le service Oracle est démarré (OracleServiceXE)";
        qDebug() << "  4. Les tables sont créées pour l'utilisateur 'wala'";
        qDebug() << "  5. Le driver Oracle ODBC est installé";
    }

    return test;
}
