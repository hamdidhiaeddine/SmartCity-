# 📱 SYSTÈME SMS - SMARTCITY

## 📋 VUE D'ENSEMBLE

Votre application utilise **2 systèmes SMS** complémentaires :

### 🔵 **1. ENVOI de SMS** (via Twilio API)

- **Service** : Twilio (plateforme cloud)
- **Fonction** : Envoyer des notifications SMS aux résidents
- **Classe** : `SmsManager`

### 🔴 **2. RÉCEPTION de SMS** (via serveur local)

- **Service** : Serveur TCP local (port 8080)
- **Fonction** : Recevoir des SMS entrants via webhook Twilio
- **Classe** : `SmsReceiver`

---

## 🔵 ENVOI DE SMS - Comment ça marche ?

### 🏗️ Architecture

```
Application Qt
     ↓
SmsManager::envoyerSms()
     ↓
API Twilio (HTTPS)
     ↓
Réseau téléphonique
     ↓
📱 Téléphone du résident
```

### 🔑 Configuration Twilio

**Fichier :** `smsmanager.cpp` (lignes 14-16)

```cpp
QString SmsManager::m_accountSid = "AC069692f58baabea5aa96ccad4054734d";
QString SmsManager::m_authToken = "50d89558d346b48a4d29b1768428f0aa";
QString SmsManager::m_numeroTwilio = "+13022625021";
```

**🔒 Informations de compte :**

- **Account SID** : Identifiant unique de votre compte Twilio
- **Auth Token** : Mot de passe pour l'API
- **Numéro Twilio** : Votre numéro d'envoi (numéro US : +1 302 262 5021)

⚠️ **IMPORTANT :** Ces identifiants sont actuellement en clair dans le code !

- Pour la production, utilisez des variables d'environnement
- Ou un fichier de configuration crypté

---

### 📞 Format des numéros de téléphone

Le système normalise automatiquement les numéros :

#### ✅ Formats acceptés :

| Format original | Normalisé       | Pays                     |
| --------------- | --------------- | ------------------------ |
| `+33123456789`  | `+33123456789`  | France                   |
| `+21612345678`  | `+21612345678`  | Tunisie                  |
| `28123456`      | `+21628123456`  | Tunisie (auto-détection) |
| `0123456789`    | `+330123456789` | France (par défaut)      |
| `33123456789`   | `+33123456789`  | France                   |

#### ❌ Formats refusés :

- Numéros sans code pays : `12345678`
- Numéros trop courts : `+3312` (< 10 chiffres)
- Caractères invalides : `+33 12-34.56`

---

### 🎯 Méthodes disponibles

#### 1. **Envoi SMS générique**

```cpp
bool SmsManager::envoyerSms(const QString &numeroDestinataire,
                           const QString &message);
```

**Exemple :**

```cpp
bool ok = SmsManager::envoyerSms("+21612345678", "Bonjour!");
if (ok) {
    // SMS envoyé
} else {
    // Erreur d'envoi
}
```

#### 2. **Notification d'ajout**

```cpp
bool SmsManager::notifierAjoutResident(const QString &numeroResident,
                                      const QString &nomResident);
```

**Message envoyé :**

```
Bonjour Martin Dubois,

Votre inscription en tant que résident a été enregistrée avec succès.

Cordialement,
Équipe SMARTCITY
```

#### 3. **Notification de modification**

```cpp
bool SmsManager::notifierModificationResident(const QString &numeroResident,
                                             const QString &nomResident);
```

**Message envoyé :**

```
Bonjour Martin Dubois,

Vos informations de résident ont été mises à jour.

Cordialement,
Équipe SMARTCITY
```

#### 4. **Notification de suppression**

```cpp
bool SmsManager::notifierSuppressionResident(const QString &numeroResident,
                                            const QString &nomResident);
```

---

### 🔄 Intégration dans l'application

#### **Lors de l'ajout d'un résident** (mainwindow.cpp, ligne 430+)

```cpp
void MainWindow::onAjouterResident()
{
    // ... validation et insertion en base ...

    if (Resident::ajouter(resident)) {
        // ✅ Ajout réussi

        // 📱 Envoi du SMS de confirmation
        QString messageSms = QString(
            "Bonjour %1,\n\n"
            "Votre inscription comme résident (ID: %2) a été enregistrée.\n\n"
            "Cordialement,\nSmartCity"
        ).arg(resident.nomComplet(), resident.id());

        if (SmsManager::envoyerSms(resident.telephone(), messageSms)) {
            qDebug() << "✓ SMS envoyé à" << resident.telephone();
        } else {
            qWarning() << "✗ Échec envoi SMS";
        }

        rafraichirResidents();
    }
}
```

#### **Envoi manuel depuis l'interface** (bouton "Envoyer SMS")

```cpp
void MainWindow::onEnvoyerSms()
{
    // 1. Récupérer le résident sélectionné dans le tableau
    const int row = ui->tableau_5->currentRow();
    QString telephone = ui->tableau_5->item(row, 4)->text();

    // 2. Demander le message à l'utilisateur
    QString message = QInputDialog::getMultiLineText(...);

    // 3. Envoyer le SMS
    bool success = SmsManager::envoyerSms(telephone, message);

    // 4. Afficher le résultat
    if (success) {
        QMessageBox::information(this, "SMS envoyé", "✓");
    } else {
        QMessageBox::warning(this, "Erreur", "✗");
    }
}
```

---

### 🌐 Communication avec Twilio

#### Requête HTTP POST

**URL :**

```
https://api.twilio.com/2010-04-01/Accounts/AC069692f58baabea5aa96ccad4054734d/Messages.json
```

**Headers :**

```
Authorization: Basic <base64(AccountSID:AuthToken)>
Content-Type: application/x-www-form-urlencoded
```

**Données POST :**

```
From=+13022625021
To=+21612345678
Body=Bonjour, votre inscription a été enregistrée.
```

**Réponse (succès, HTTP 201) :**

```json
{
  "sid": "SMxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
  "status": "queued",
  "from": "+13022625021",
  "to": "+21612345678",
  "body": "Bonjour, votre inscription a été enregistrée.",
  "date_created": "Mon, 25 Nov 2025 16:00:00 +0000"
}
```

**Réponse (erreur, HTTP 400) :**

```json
{
  "code": 21211,
  "message": "The 'To' number +2812345678 is not a valid phone number.",
  "more_info": "https://www.twilio.com/docs/errors/21211"
}
```

---

### 🐛 Codes d'erreur Twilio

| Code  | Erreur                   | Cause                 | Solution                               |
| ----- | ------------------------ | --------------------- | -------------------------------------- |
| 21211 | Numéro invalide          | Format incorrect      | Vérifier le format international       |
| 21608 | Numéro non vérifié       | Compte d'essai        | Vérifier le numéro dans Twilio Console |
| 21614 | Numéro FROM invalide     | Mauvais numéro Twilio | Vérifier `m_numeroTwilio`              |
| 20003 | Authentification échouée | Mauvais SID/Token     | Vérifier les identifiants              |
| 21606 | Pays non autorisé        | Géo-restrictions      | Activer le pays dans Twilio            |

---

## 🔴 RÉCEPTION DE SMS - Comment ça marche ?

### 🏗️ Architecture

```
📱 Téléphone envoie SMS
     ↓
Réseau téléphonique
     ↓
Twilio reçoit le SMS
     ↓
Webhook HTTP POST
     ↓
Votre serveur TCP (port 8080)
     ↓
SmsReceiver::traiterRequeteHttp()
     ↓
Signal smsRecu() → Interface Qt
```

### 🖥️ Serveur TCP Local

**Démarrage** (mainwindow.cpp, ligne 60+) :

```cpp
MainWindow::MainWindow(QWidget *parent)
{
    // Créer le récepteur
    m_smsReceiver = new SmsReceiver(this);

    // Connecter le signal
    connect(m_smsReceiver, &SmsReceiver::smsRecu,
            this, &MainWindow::onSmsRecu);

    // Démarrer le serveur
    if (m_smsReceiver->demarrerServeur(8080)) {
        qDebug() << "✓ Serveur SMS démarré sur port 8080";
    } else {
        qWarning() << "✗ Impossible de démarrer le serveur";
    }
}
```

### 🌐 Configuration Webhook Twilio

**Vous DEVEZ configurer le webhook dans Twilio Console :**

1. Allez sur https://console.twilio.com
2. Sélectionnez votre numéro Twilio
3. Dans "Messaging" → "Webhook when a message comes in"
4. Entrez : `http://VOTRE_IP_PUBLIQUE:8080/sms`
5. Méthode : **POST**

⚠️ **IMPORTANT :**

- Votre IP doit être **publique** et **accessible** depuis Internet
- Ouvrez le **port 8080** dans votre firewall/routeur
- Utilisez **ngrok** pour tester en local :
  ```bash
  ngrok http 8080
  # URL générée : https://abc123.ngrok.io
  # Webhook Twilio : https://abc123.ngrok.io/sms
  ```

---

### 📥 Traitement des SMS reçus

**Requête Twilio vers votre serveur :**

```http
POST /sms HTTP/1.1
Host: votre-ip:8080
Content-Type: application/x-www-form-urlencoded

MessageSid=SM123456789&
From=+21612345678&
To=+13022625021&
Body=Bonjour%20SmartCity&
DateSent=Mon,%2025%20Nov%202025%2016:00:00%20+0000
```

**Code de traitement** (smsreceiver.cpp) :

```cpp
void SmsReceiver::traiterRequeteHttp(QTcpSocket *socket, const QByteArray &data)
{
    // 1. Extraire le body de la requête HTTP
    QByteArray body = parserBodyHttp(data);

    // 2. Parser les paramètres (URL-encoded)
    QUrlQuery query(QString::fromUtf8(body));
    QString from = query.queryItemValue("From");      // +21612345678
    QString to = query.queryItemValue("To");          // +13022625021
    QString message = query.queryItemValue("Body");   // Bonjour SmartCity

    // 3. Parser la date
    QDateTime dateTime = QDateTime::fromString(dateStr, ...);

    // 4. Répondre à Twilio (TwiML vide)
    QString twiml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                    "<Response>\n"
                    "</Response>\n";
    envoyerReponseHttp(socket, 200, twiml);

    // 5. Émettre le signal
    emit smsRecu(from, to, message, dateTime);
}
```

**Gestionnaire du signal** (mainwindow.cpp) :

```cpp
void MainWindow::onSmsRecu(const QString &from,
                          const QString &to,
                          const QString &message,
                          const QDateTime &dateTime)
{
    qDebug() << "📱 SMS REÇU !";
    qDebug() << "De:" << from;
    qDebug() << "Message:" << message;
    qDebug() << "Date:" << dateTime.toString();

    // Afficher dans l'interface
    QMessageBox::information(this,
        "SMS Reçu",
        QString("De: %1\n\nMessage: %2").arg(from, message)
    );

    // Vous pouvez :
    // - Enregistrer en base de données
    // - Ajouter à une liste de SMS
    // - Déclencher une action automatique
}
```

---

## 🛠️ DÉBOGAGE

### 📊 Messages de debug

**Lors de l'envoi :**

```
Numéro original: +21628123456
Numéro normalisé: +21628123456
Message: Bonjour, votre inscription a été enregistrée.
URL Twilio: https://api.twilio.com/2010-04-01/Accounts/.../Messages.json
Données POST: From=...&To=...&Body=...
Code HTTP: 201
SMS envoyé avec succès! Status: queued
SID: SM1234567890abcdef
```

**En cas d'erreur :**

```
✗ Erreur lors de l'envoi du SMS
Code HTTP: 400
Code d'erreur Twilio: 21211
Message d'erreur Twilio: The 'To' number is not valid
ERREUR: Numéro de téléphone invalide
Le numéro doit être au format international valide (ex: +216XXXXXXXX)
```

**Lors de la réception :**

```
Serveur SMS démarré sur le port 8080
URL webhook Twilio: http://VOTRE_IP:8080/sms
Nouvelle connexion depuis 54.173.119.234
=== SMS REÇU ===
De: +21612345678
Vers: +13022625021
Message: Bonjour SmartCity
SID: SM9876543210fedcba
Date: Mon, 25 Nov 2025 16:00:00 +0000
```

---

## ⚠️ PROBLÈMES COURANTS

### 1️⃣ "The bound address is already in use"

**Cause :** Le port 8080 est déjà utilisé

**Solutions :**

```cmd
# Vérifier quel processus utilise le port
netstat -ano | findstr :8080

# Tuer le processus
taskkill /PID <numéro_pid> /F

# Ou changer de port dans le code
m_smsReceiver->demarrerServeur(8081);  // Port différent
```

### 2️⃣ "database not open" lors de l'envoi

**Cause :** SMS envoyé avant la connexion DB

**Solution :** Déjà corrigée ! La connexion s'établit maintenant avant.

### 3️⃣ Numéros tunisiens mal formatés

**Problème :** `+28123456` au lieu de `+21628123456`

**Solution automatique :** Le code détecte et corrige automatiquement !

```cpp
if (numeroNormalise.startsWith("+28") && digitsOnly.length() == 8) {
    numeroNormalise = "+216" + digitsOnly;  // Ajout du code pays
}
```

### 4️⃣ SMS non reçus

**Checklist :**

- ☐ Webhook configuré dans Twilio Console
- ☐ IP publique accessible depuis Internet
- ☐ Port 8080 ouvert dans le firewall
- ☐ Serveur démarré (`✓ Serveur SMS démarré`)
- ☐ Utiliser ngrok pour tester en local

---

## 💰 COÛTS TWILIO

**Compte d'essai :**

- ✅ Gratuit
- ⚠️ Numéros destinataires doivent être vérifiés
- 📱 Crédit initial (variable selon promo)

**Tarifs (2025) :**

- 📤 Envoi SMS US → Tunisie : ~0.10 USD/SMS
- 📥 Réception SMS : Gratuit (si numéro Twilio capable)
- 🌍 Prix varient selon pays

**Vérifier un numéro (compte d'essai) :**

1. https://console.twilio.com/verified-caller-ids
2. Cliquer "Add a new Caller ID"
3. Entrer le numéro : `+21612345678`
4. Recevoir code de vérification par SMS
5. Valider le code

---

## 🎯 UTILISATION DANS L'APPLICATION

### Scénarios d'envoi automatique :

1. **Ajout résident** → SMS "Inscription confirmée"
2. **Modification résident** → SMS "Informations mises à jour"
3. **Suppression résident** → SMS "Dossier supprimé"

### Scénarios d'envoi manuel :

1. Sélectionner un résident dans le tableau
2. Cliquer sur **"Envoyer SMS"**
3. Saisir le message personnalisé
4. Cliquer OK → SMS envoyé

### Réception de SMS :

- Les SMS entrants sont affichés dans une popup
- **À IMPLÉMENTER :** Stockage en base de données
- **À IMPLÉMENTER :** Historique des SMS reçus

---

## 📈 AMÉLIORATIONS POSSIBLES

### 🔒 Sécurité

```cpp
// Stocker les identifiants dans un fichier de config
QSettings settings("config.ini", QSettings::IniFormat);
QString accountSid = settings.value("twilio/account_sid").toString();
QString authToken = settings.value("twilio/auth_token").toString();
```

### 💾 Historique SMS

```sql
CREATE TABLE HIST_SMS (
    ID NUMBER PRIMARY KEY,
    ID_RESIDENT NUMBER,
    TYPE VARCHAR2(20),  -- 'ENVOI' ou 'RECEPTION'
    NUMERO VARCHAR2(20),
    MESSAGE VARCHAR2(1000),
    DATE_ENVOI DATE,
    STATUT VARCHAR2(20)  -- 'ENVOYE', 'ERREUR', 'RECU'
);
```

### 📊 Statistiques

- Nombre de SMS envoyés
- Coût total des SMS
- Taux de succès/échec
- SMS reçus par période

---

## 🎉 CONCLUSION

Votre système SMS est **complet et fonctionnel** !

**Points forts :**
✅ Normalisation automatique des numéros  
✅ Gestion des erreurs détaillée  
✅ Auto-correction numéros tunisiens  
✅ Réception SMS via webhook  
✅ Intégration dans le CRUD résidents

**À faire :**
⚠️ Sécuriser les identifiants Twilio  
⚠️ Configurer le webhook pour la réception  
⚠️ Ouvrir le port 8080 ou utiliser ngrok  
⚠️ Vérifier les numéros destinataires (compte d'essai)
