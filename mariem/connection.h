#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>

class Connection
{
public:
    Connection();
    bool createconnect();
    bool isConnected();
    bool reconnect();
    bool createTableAlertes();  // Créer automatiquement la table GEST_ALERTES
};

#endif // CONNECTION_H
