#ifndef SMSRECEIVER_H
#define SMSRECEIVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QDateTime>

class SmsReceiver : public QObject
{
    Q_OBJECT

public:
    explicit SmsReceiver(QObject *parent = nullptr);
    ~SmsReceiver();
    
    bool demarrerServeur(quint16 port = 8080);
    void arreterServeur();
    bool estActif() const;
    quint16 port() const;

signals:
    void smsRecu(const QString &from, const QString &to, const QString &message, const QDateTime &dateTime);
    void erreur(const QString &message);

private slots:
    void nouvelleConnexion();
    void donneesDisponibles();
    void connexionFermee();

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    quint16 m_port;
    
    void traiterRequeteHttp(QTcpSocket *socket, const QByteArray &data);
    void envoyerReponseHttp(QTcpSocket *socket, int code, const QString &message);
    QByteArray parserBodyHttp(const QByteArray &data);
};

#endif // SMSRECEIVER_H

