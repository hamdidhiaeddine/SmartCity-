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
    db.setDatabaseName("source_projet2A"); // Nom de la source de données ODBC
    db.setUserName("hiba");             // Nom de l'utilisateur Oracle
    db.setPassword("esprit18");            // Mot de passe

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
        qDebug() << "  1. La source ODBC 'Source_Projet2A' existe";
        qDebug() << "  2. L'utilisateur 'hiba' existe dans Oracle";
        qDebug() << "  3. Le service Oracle est démarré";
        qDebug() << "  4. Les tables sont créées pour l'utilisateur 'hiba'";
        qDebug() << "  5. Les triggers d'historique sont créés (exécutez CORRIGER_HISTORIQUE.sql)";
    }

    return test;
}
