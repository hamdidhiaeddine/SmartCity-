#include "connection.h"

Connection::Connection()
{

}

bool Connection::createconnect()
{bool test=false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName("localhost");
    db.setDatabaseName("source_projet2A");//inserer le nom de la source de données
    db.setUserName("system");//inserer nom de l'utilisateur
    db.setPassword("123456");//inserer mot de passe de cet utilisateur
    if (!db.open()) {
        qDebug() << "❌ Database connection failed:" << db.lastError().text();
        return false;
    }

    qDebug() << "✅ Database connected successfully.";

    // 👇 Force autocommit (so all INSERT/UPDATE/DELETE persist automatically)
    db.setConnectOptions("AUTOCOMMIT=ON");

    return true;
    /*if (db.open())
        test=true;





    */return  test;
}
