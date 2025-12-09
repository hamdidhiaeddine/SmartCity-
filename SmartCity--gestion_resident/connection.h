#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class Connection
{
public:
    Connection();
    bool createconnect();
    bool createTableAlertes();
    bool isConnected();
    bool reconnect();
};

#endif // CONNECTION_H
