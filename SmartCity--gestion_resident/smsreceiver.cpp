#include "smsreceiver.h"

#include <QDebug>
#include <QUrlQuery>
#include <QRegularExpression>

SmsReceiver::SmsReceiver(QObject *parent)
    : QObject(parent),
      m_server(new QTcpServer(this)),
      m_port(8080)
{
    connect(m_server, &QTcpServer::newConnection, this, &SmsReceiver::nouvelleConnexion);
}

SmsReceiver::~SmsReceiver()
{
    arreterServeur();
}

bool SmsReceiver::demarrerServeur(quint16 port)
{
    if (m_server->isListening()) {
        qWarning() << "Le serveur est déjà en cours d'exécution";
        return false;
    }
    
    m_port = port;
    
    if (!m_server->listen(QHostAddress::Any, port)) {
        qWarning() << "Impossible de démarrer le serveur sur le port" << port;
        qWarning() << "Erreur:" << m_server->errorString();
        emit erreur(QString("Impossible de démarrer le serveur: %1").arg(m_server->errorString()));
        return false;
    }
    
    qDebug() << "Serveur SMS démarré sur le port" << port;
    qDebug() << "URL webhook Twilio: http://VOTRE_IP:" << port << "/sms";
    return true;
}

void SmsReceiver::arreterServeur()
{
    if (m_server->isListening()) {
        m_server->close();
        qDebug() << "Serveur SMS arrêté";
    }
    
    // Fermer toutes les connexions client
    for (QTcpSocket *client : m_clients) {
        client->close();
        client->deleteLater();
    }
    m_clients.clear();
}

bool SmsReceiver::estActif() const
{
    return m_server->isListening();
}

quint16 SmsReceiver::port() const
{
    return m_port;
}

void SmsReceiver::nouvelleConnexion()
{
    QTcpSocket *client = m_server->nextPendingConnection();
    if (!client) {
        return;
    }
    
    m_clients.append(client);
    connect(client, &QTcpSocket::readyRead, this, &SmsReceiver::donneesDisponibles);
    connect(client, &QTcpSocket::disconnected, this, &SmsReceiver::connexionFermee);
    
    qDebug() << "Nouvelle connexion depuis" << client->peerAddress().toString();
}

void SmsReceiver::donneesDisponibles()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) {
        return;
    }
    
    QByteArray data = client->readAll();
    traiterRequeteHttp(client, data);
}

void SmsReceiver::connexionFermee()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) {
        return;
    }
    
    m_clients.removeAll(client);
    client->deleteLater();
}

void SmsReceiver::traiterRequeteHttp(QTcpSocket *socket, const QByteArray &data)
{
    QString requete = QString::fromUtf8(data);
    
    // Vérifier que c'est une requête POST vers /sms
    if (!requete.contains("POST /sms") && !requete.contains("POST /")) {
        envoyerReponseHttp(socket, 404, "Not Found");
        return;
    }
    
    // Parser le body de la requête (format application/x-www-form-urlencoded)
    QByteArray body = parserBodyHttp(data);
    
    if (body.isEmpty()) {
        envoyerReponseHttp(socket, 400, "Bad Request - Body vide");
        return;
    }
    
    // Parser les paramètres Twilio
    QUrlQuery query(QString::fromUtf8(body));
    
    QString from = query.queryItemValue("From");
    QString to = query.queryItemValue("To");
    QString message = query.queryItemValue("Body");
    QString messageSid = query.queryItemValue("MessageSid");
    QString dateStr = query.queryItemValue("DateSent");
    
    qDebug() << "=== SMS REÇU ===";
    qDebug() << "De:" << from;
    qDebug() << "Vers:" << to;
    qDebug() << "Message:" << message;
    qDebug() << "SID:" << messageSid;
    qDebug() << "Date:" << dateStr;
    
    if (from.isEmpty() || message.isEmpty()) {
        qWarning() << "Paramètres manquants dans la requête";
        envoyerReponseHttp(socket, 400, "Bad Request - Paramètres manquants");
        return;
    }
    
    // Parser la date
    QDateTime dateTime;
    if (!dateStr.isEmpty()) {
        // Format Twilio: "Mon, 01 Jan 2024 12:00:00 +0000"
        dateTime = QDateTime::fromString(dateStr, "ddd, dd MMM yyyy HH:mm:ss +0000");
        if (!dateTime.isValid()) {
            dateTime = QDateTime::currentDateTime();
        }
    } else {
        dateTime = QDateTime::currentDateTime();
    }
    
    // Envoyer la réponse TwiML à Twilio
    QString twiml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                    "<Response>\n"
                    "</Response>\n";
    
    envoyerReponseHttp(socket, 200, twiml);
    
    // Émettre le signal avec les informations du SMS
    emit smsRecu(from, to, message, dateTime);
}

QByteArray SmsReceiver::parserBodyHttp(const QByteArray &data)
{
    // Chercher la ligne vide qui sépare les headers du body
    int bodyStart = data.indexOf("\r\n\r\n");
    if (bodyStart == -1) {
        bodyStart = data.indexOf("\n\n");
    }
    
    if (bodyStart == -1) {
        return QByteArray();
    }
    
    // Extraire le body (après les headers)
    QByteArray body = data.mid(bodyStart + 4); // +4 pour "\r\n\r\n" ou "\n\n"
    
    return body.trimmed();
}

void SmsReceiver::envoyerReponseHttp(QTcpSocket *socket, int code, const QString &message)
{
    QString reponse = QString("HTTP/1.1 %1 OK\r\n"
                              "Content-Type: text/xml; charset=utf-8\r\n"
                              "Content-Length: %2\r\n"
                              "Connection: close\r\n"
                              "\r\n"
                              "%3")
                      .arg(code)
                      .arg(message.toUtf8().length())
                      .arg(message);
    
    socket->write(reponse.toUtf8());
    socket->flush();
    socket->waitForBytesWritten(3000);
}

