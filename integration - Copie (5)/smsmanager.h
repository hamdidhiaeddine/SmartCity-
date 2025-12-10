#ifndef SMSMANAGER_H
#define SMSMANAGER_H

#include <QString>
#include <QObject>

class SmsManager : public QObject
{
    Q_OBJECT

public:
    explicit SmsManager(QObject *parent = nullptr);
    
    // Envoyer un SMS
    static bool envoyerSms(const QString &numeroDestinataire, const QString &message);
    
    // Envoyer un SMS de notification pour l'ajout d'un résident
    static bool notifierAjoutResident(const QString &numeroResident, const QString &nomResident);
    
    // Envoyer un SMS de notification pour la modification d'un résident
    static bool notifierModificationResident(const QString &numeroResident, const QString &nomResident);
    
    // Envoyer un SMS de notification pour la suppression d'un résident
    static bool notifierSuppressionResident(const QString &numeroResident, const QString &nomResident);

private:
    static QString m_accountSid;
    static QString m_authToken;
    static QString m_numeroTwilio;
    
    static QString construireUrlApi();
    static QString construireAuthHeader();
};

#endif // SMSMANAGER_H

