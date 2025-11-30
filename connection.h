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
    bool createTableAlertes();
    bool createOrPatchTableMaisons();
};

#endif // CONNECTION_H

