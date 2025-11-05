#include "connection.h"

Connection::Connection()
{

}

bool Connection::createconnect()
{bool test=false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("dhiaeddine");//inserer le nom de la source de données
    db.setUserName("system");//inserer nom de l'utilisateur
    db.setPassword("Semia2025-");//inserer mot de passe de cet utilisateur

    if (db.open())
        test=true;





    return  test;
}
