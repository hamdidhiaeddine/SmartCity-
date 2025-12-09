#include "smsmanager.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QRegularExpression>

// Configuration Twilio
QString SmsManager::m_accountSid = "ACcfcecf0193b7ec6e5cab80b90c94947e";
QString SmsManager::m_authToken = "347fac7a47ea6216574b950e237b6e1b";
QString SmsManager::m_numeroTwilio = "+12708131762";

SmsManager::SmsManager(QObject *parent)
    : QObject(parent)
{
}

QString SmsManager::construireUrlApi()
{
    return QString("https://api.twilio.com/2010-04-01/Accounts/%1/Messages.json").arg(m_accountSid);
}

QString SmsManager::construireAuthHeader()
{
    QString credentials = QString("%1:%2").arg(m_accountSid, m_authToken);
    QByteArray credentialsBytes = credentials.toUtf8();
    QByteArray base64Data = credentialsBytes.toBase64();
    return QString("Basic %1").arg(QString::fromLatin1(base64Data));
}

bool SmsManager::envoyerSms(const QString &numeroDestinataire, const QString &message)
{
    // Vérifier que le numéro n'est pas vide
    if (numeroDestinataire.isEmpty()) {
        qWarning() << "Numéro de téléphone vide, impossible d'envoyer le SMS";
        return false;
    }
    
    // Normaliser le numéro de téléphone
    QString numeroNormalise = numeroDestinataire.trimmed();
    
    // Supprimer tous les espaces, tirets, points, parenthèses
    numeroNormalise.remove(QRegularExpression("[\\s\\-\\.\\(\\)]"));
    
    // Si le numéro commence déjà par +, vérifier qu'il est valide
    if (numeroNormalise.startsWith("+")) {
        QString digitsOnly = numeroNormalise.mid(1);
        
        // Vérifier que ce sont uniquement des chiffres
        if (!digitsOnly.contains(QRegularExpression("^\\d+$"))) {
            qWarning() << "Le numéro contient des caractères non numériques:" << numeroNormalise;
            return false;
        }
        
        // Détecter et corriger les numéros locaux tunisiens mal formatés AVANT la validation
        // Les numéros tunisiens commencent souvent par 28, 29, 50, 90, etc.
        // Si le numéro commence par +28 ou +29 avec 8 chiffres, c'est probablement un numéro tunisien
        if ((numeroNormalise.startsWith("+28") || numeroNormalise.startsWith("+29")) && 
            digitsOnly.length() == 8) {
            qWarning() << "Numéro tunisien mal formaté détecté:" << numeroNormalise;
            qWarning() << "Correction automatique: ajout du code pays tunisien (+216)";
            numeroNormalise = "+216" + digitsOnly;
            digitsOnly = numeroNormalise.mid(1); // Mettre à jour digitsOnly après correction
            qDebug() << "Numéro corrigé:" << numeroNormalise;
        }
        
        // Vérifier la longueur minimale (au moins 10 chiffres pour un numéro international)
        // Cette vérification se fait APRÈS la correction automatique
        if (digitsOnly.length() < 10) {
            qWarning() << "Numéro international trop court:" << numeroNormalise;
            qWarning() << "Le numéro doit contenir au moins 10 chiffres après le +";
            qWarning() << "Numéro reçu:" << numeroDestinataire;
            qWarning() << "Numéro après normalisation:" << numeroNormalise;
            return false;
        }
        
        // Détecter d'autres numéros locaux mal formatés (commencent par +2X avec moins de 11 chiffres)
        // mais qui ne sont pas des numéros tunisiens corrigibles
        if ((numeroNormalise.startsWith("+21") || numeroNormalise.startsWith("+22") || 
             numeroNormalise.startsWith("+23") || numeroNormalise.startsWith("+24") ||
             numeroNormalise.startsWith("+25") || numeroNormalise.startsWith("+26") ||
             numeroNormalise.startsWith("+27")) &&
            digitsOnly.length() < 11 && !numeroNormalise.startsWith("+216") &&
            !numeroNormalise.startsWith("+212") && !numeroNormalise.startsWith("+213")) {
            qWarning() << "Numéro invalide détecté:" << numeroNormalise;
            qWarning() << "Ce numéro semble être un numéro local mal formaté.";
            qWarning() << "Les numéros internationaux doivent contenir le code pays complet.";
            qWarning() << "Veuillez vérifier et corriger le numéro dans la base de données.";
            return false;
        }
        
        // Détecter d'autres numéros locaux mal formatés (commencent par +2X avec moins de 11 chiffres)
        if ((numeroNormalise.startsWith("+21") || numeroNormalise.startsWith("+22") || 
             numeroNormalise.startsWith("+23") || numeroNormalise.startsWith("+24") ||
             numeroNormalise.startsWith("+25") || numeroNormalise.startsWith("+26") ||
             numeroNormalise.startsWith("+27") || numeroNormalise.startsWith("+29")) &&
            digitsOnly.length() < 11) {
            qWarning() << "Numéro invalide détecté:" << numeroNormalise;
            qWarning() << "Ce numéro semble être un numéro local mal formaté.";
            qWarning() << "Les numéros internationaux doivent contenir le code pays complet.";
            qWarning() << "Veuillez vérifier et corriger le numéro dans la base de données.";
            return false;
        }
    } else {
        // Si le numéro ne commence pas par +, essayer de le normaliser
        // Supprimer les zéros en début si présents
        while (numeroNormalise.startsWith("0") && numeroNormalise.length() > 1) {
            numeroNormalise = numeroNormalise.mid(1);
        }
        
        // Si le numéro commence par un code pays connu, ajouter le +
        // Codes pays courants : 33 (France), 216 (Tunisie), 212 (Maroc), 213 (Algérie), etc.
        if (numeroNormalise.startsWith("33") && numeroNormalise.length() >= 11) {
            // Numéro français
            numeroNormalise = "+" + numeroNormalise;
        } else if (numeroNormalise.startsWith("216") && numeroNormalise.length() >= 11) {
            // Numéro tunisien
            numeroNormalise = "+" + numeroNormalise;
        } else if (numeroNormalise.startsWith("212") && numeroNormalise.length() >= 11) {
            // Numéro marocain
            numeroNormalise = "+" + numeroNormalise;
        } else if (numeroNormalise.startsWith("213") && numeroNormalise.length() >= 11) {
            // Numéro algérien
            numeroNormalise = "+" + numeroNormalise;
        } else if (numeroNormalise.length() >= 10 && numeroNormalise.contains(QRegularExpression("^\\d+$"))) {
            // Si c'est un numéro local (10 chiffres), supposer que c'est français
            // ATTENTION: Vous devrez peut-être adapter cela selon votre contexte
            qWarning() << "Numéro local détecté:" << numeroNormalise;
            qWarning() << "Conversion en format international français (+33)";
            numeroNormalise = "+33" + numeroNormalise;
        } else {
            qWarning() << "Format de numéro invalide:" << numeroDestinataire;
            qWarning() << "Le numéro doit être au format international (ex: +33123456789)";
            return false;
        }
    }
    
    // Validation finale : le numéro doit commencer par + et contenir uniquement des chiffres après
    if (!numeroNormalise.startsWith("+") || numeroNormalise.length() < 11) {
        qWarning() << "Numéro normalisé invalide:" << numeroNormalise;
        return false;
    }
    
    QString digitsAfterPlus = numeroNormalise.mid(1);
    if (!digitsAfterPlus.contains(QRegularExpression("^\\d+$")) || digitsAfterPlus.length() < 10) {
        qWarning() << "Numéro normalisé invalide (doit contenir au moins 10 chiffres):" << numeroNormalise;
        return false;
    }
    
    qDebug() << "Numéro original:" << numeroDestinataire;
    qDebug() << "Numéro normalisé:" << numeroNormalise;
    qDebug() << "Message:" << message;
    
    QNetworkAccessManager manager;
    QNetworkRequest request;
    
    // Construire l'URL de l'API Twilio
    QString url = construireUrlApi();
    qDebug() << "URL Twilio:" << url;
    request.setUrl(QUrl(url));
    
    // Ajouter les en-têtes d'authentification (Twilio utilise Basic Auth)
    QString authHeader = construireAuthHeader();
    request.setRawHeader("Authorization", authHeader.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    
    // Construire les données POST au format application/x-www-form-urlencoded
    QUrlQuery postData;
    postData.addQueryItem("From", m_numeroTwilio);
    postData.addQueryItem("To", numeroNormalise);
    postData.addQueryItem("Body", message);
    
    QByteArray data = postData.toString(QUrl::FullyEncoded).toUtf8();
    
    qDebug() << "Données POST:" << data;
    qDebug() << "Auth Header:" << authHeader;
    
    // Envoyer la requête POST
    QNetworkReply *reply = manager.post(request, data);
    
    // Attendre la réponse (synchrone pour simplifier)
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    
    // Vérifier la réponse
    int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray response = reply->readAll();
    
    qDebug() << "Code HTTP:" << httpCode;
    qDebug() << "Réponse:" << response;
    
    if (reply->error() == QNetworkReply::NoError && (httpCode == 200 || httpCode == 201)) {
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (!doc.isNull()) {
            QJsonObject obj = doc.object();
            QString status = obj["status"].toString();
            qDebug() << "SMS envoyé avec succès! Status:" << status;
            qDebug() << "SID:" << obj["sid"].toString();
        } else {
            qDebug() << "SMS envoyé avec succès! Réponse:" << response;
        }
        reply->deleteLater();
        return true;
    } else {
        qWarning() << "═══════════════════════════════════════════════════════════";
        qWarning() << "✗ ERREUR LORS DE L'ENVOI DU SMS";
        qWarning() << "═══════════════════════════════════════════════════════════";
        qWarning() << "Erreur réseau:" << reply->errorString();
        qWarning() << "Code HTTP:" << httpCode;
        qWarning() << "Numéro destinataire:" << numeroNormalise;
        qWarning() << "Numéro Twilio (From):" << m_numeroTwilio;
        qWarning() << "Account SID:" << m_accountSid;
        qWarning() << "";
        
        // Vérifier les erreurs réseau spécifiques
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "DÉTAILS DE L'ERREUR RÉSEAU:";
            qWarning() << "  Type:" << reply->error();
            qWarning() << "  Message:" << reply->errorString();
            
            if (reply->error() == QNetworkReply::SslHandshakeFailedError) {
                qWarning() << "";
                qWarning() << "⚠️ ERREUR SSL DÉTECTÉE!";
                qWarning() << "  → OpenSSL n'est pas correctement configuré";
                qWarning() << "  → Consultez: FIX_SSL_PROBLEME.md";
                qWarning() << "  → Ou exécutez: installer_ssl.bat";
            } else if (reply->error() == QNetworkReply::ConnectionRefusedError) {
                qWarning() << "";
                qWarning() << "⚠️ CONNEXION REFUSÉE!";
                qWarning() << "  → Vérifiez votre connexion Internet";
                qWarning() << "  → Vérifiez que le pare-feu n'bloque pas l'application";
            } else if (reply->error() == QNetworkReply::TimeoutError) {
                qWarning() << "";
                qWarning() << "⚠️ TIMEOUT!";
                qWarning() << "  → La connexion a expiré";
                qWarning() << "  → Vérifiez votre connexion Internet";
            }
        }
        
        qWarning() << "";
        qWarning() << "RÉPONSE DU SERVEUR:";
        qWarning() << response;
        qWarning() << "";
        
        // Parser la réponse d'erreur JSON si possible
        QJsonDocument errorDoc = QJsonDocument::fromJson(response);
        QString errorMessageTwilio;
        int errorCodeTwilio = 0;
        
        if (!errorDoc.isNull()) {
            QJsonObject errorObj = errorDoc.object();
            errorMessageTwilio = errorObj["message"].toString();
            errorCodeTwilio = errorObj["code"].toInt();
            qWarning() << "═══════════════════════════════════════════════════════════";
            qWarning() << "ERREUR TWILIO:";
            qWarning() << "═══════════════════════════════════════════════════════════";
            qWarning() << "Code d'erreur:" << errorCodeTwilio;
            qWarning() << "Message:" << errorMessageTwilio;
            qWarning() << "";
            
            // Messages d'aide spécifiques selon le code d'erreur
            if (errorCodeTwilio == 21211) {
                qWarning() << "🔴 ERREUR 21211: Numéro de téléphone invalide";
                qWarning() << "  → Le numéro doit être au format international valide";
                qWarning() << "  → Format attendu: +216XXXXXXXX (Tunisie)";
                qWarning() << "  → Numéro reçu:" << numeroNormalise;
                qWarning() << "  → Vérifiez le numéro dans la base de données";
            } else if (errorCodeTwilio == 21608) {
                qWarning() << "🔴 ERREUR 21608: Numéro non vérifié";
                qWarning() << "  → Pour les comptes d'essai Twilio, vous devez vérifier les numéros";
                qWarning() << "  → Allez sur: https://console.twilio.com/us1/develop/phone-numbers/manage/verified";
                qWarning() << "  → Ajoutez le numéro:" << numeroNormalise;
            } else if (errorCodeTwilio == 21614) {
                qWarning() << "🔴 ERREUR 21614: Numéro 'From' invalide";
                qWarning() << "  → Le numéro Twilio configuré n'est pas valide";
                qWarning() << "  → Numéro configuré:" << m_numeroTwilio;
                qWarning() << "  → Vérifiez dans votre compte Twilio";
            } else if (errorCodeTwilio == 20003) {
                qWarning() << "🔴 ERREUR 20003: Authentification échouée";
                qWarning() << "  → Account SID ou Auth Token incorrect";
                qWarning() << "  → Vérifiez vos credentials dans smsmanager.cpp";
            } else if (errorCodeTwilio == 21408) {
                qWarning() << "🔴 ERREUR 21408: Permission refusée";
                qWarning() << "  → Votre compte Twilio n'a pas les permissions nécessaires";
                qWarning() << "  → Vérifiez votre compte Twilio";
            }
        } else {
            qWarning() << "═══════════════════════════════════════════════════════════";
            qWarning() << "⚠️ IMPOSSIBLE DE PARSER LA RÉPONSE JSON";
            qWarning() << "═══════════════════════════════════════════════════════════";
            qWarning() << "Causes possibles:";
            qWarning() << "  1. Problème de connexion réseau";
            qWarning() << "  2. Problème SSL (OpenSSL manquant)";
            qWarning() << "  3. Serveur Twilio inaccessible";
            qWarning() << "  4. Réponse non-JSON reçue";
        }
        
        qWarning() << "";
        qWarning() << "═══════════════════════════════════════════════════════════";
        qWarning() << "SOLUTIONS POSSIBLES:";
        qWarning() << "═══════════════════════════════════════════════════════════";
        qWarning() << "1. Vérifiez votre connexion Internet";
        qWarning() << "2. Vérifiez vos credentials Twilio (Account SID / Auth Token)";
        qWarning() << "3. Vérifiez que le numéro est vérifié (compte d'essai)";
        qWarning() << "4. Vérifiez OpenSSL (voir FIX_SSL_PROBLEME.md)";
        qWarning() << "5. Consultez la console Twilio pour plus de détails";
        qWarning() << "═══════════════════════════════════════════════════════════";
        
        reply->deleteLater();
        return false;
    }
}

bool SmsManager::notifierAjoutResident(const QString &numeroResident, const QString &nomResident)
{
    QString message = QString("Bonjour %1,\n\nVotre inscription en tant que résident a été enregistrée avec succès.\n\nCordialement,\nÉquipe SMARTCITY")
                      .arg(nomResident);
    
    return envoyerSms(numeroResident, message);
}

bool SmsManager::notifierModificationResident(const QString &numeroResident, const QString &nomResident)
{
    QString message = QString("Bonjour %1,\n\nVos informations de résident ont été mises à jour.\n\nCordialement,\nÉquipe SMARTCITY")
                      .arg(nomResident);
    
    return envoyerSms(numeroResident, message);
}

bool SmsManager::notifierSuppressionResident(const QString &numeroResident, const QString &nomResident)
{
    QString message = QString("Bonjour %1,\n\nVotre dossier de résident a été supprimé de notre système.\n\nCordialement,\nÉquipe SMARTCITY")
                      .arg(nomResident);
    
    return envoyerSms(numeroResident, message);
}

