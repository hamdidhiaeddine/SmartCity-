#include "connection.h"
#include <QDebug>

Connection::Connection()
{

}

bool Connection::createconnect()
{
    bool test = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    
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
