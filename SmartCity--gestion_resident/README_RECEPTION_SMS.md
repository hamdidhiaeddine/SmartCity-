# Réception de SMS via Twilio

## 📋 Description

L'application peut maintenant **recevoir des SMS** via Twilio. Lorsqu'un SMS est envoyé au numéro Twilio configuré, l'application le reçoit automatiquement et affiche une notification.

## 🔧 Fonctionnement

1. **Serveur HTTP intégré** : L'application démarre un serveur HTTP sur le port **8080** au démarrage
2. **Webhook Twilio** : Twilio envoie les SMS reçus à votre application via HTTP POST
3. **Notification** : Une boîte de dialogue s'affiche automatiquement lorsqu'un SMS est reçu

## ⚙️ Configuration

### Étape 1 : Obtenir votre adresse IP publique

Pour que Twilio puisse envoyer les SMS à votre application, vous devez exposer votre serveur sur Internet.

**Option A : Serveur local avec ngrok (pour les tests)**
```bash
# Installer ngrok depuis https://ngrok.com
ngrok http 8080
# Vous obtiendrez une URL comme : https://abc123.ngrok.io
```

**Option B : Serveur dédié**
- Utilisez un serveur avec une adresse IP publique
- Configurez le pare-feu pour autoriser le port 8080

### Étape 2 : Configurer le webhook dans Twilio

1. Connectez-vous à votre compte Twilio : https://console.twilio.com
2. Allez dans **Phone Numbers** > **Manage** > **Active numbers**
3. Cliquez sur votre numéro Twilio (`+13022625021`)
4. Dans la section **Messaging**, configurez le **Webhook URL** :
   ```
   http://VOTRE_IP:8080/sms
   ```
   ou avec ngrok :
   ```
   https://abc123.ngrok.io/sms
   ```
5. Sélectionnez **HTTP POST** comme méthode
6. Cliquez sur **Save**

### Étape 3 : Tester la réception

1. Envoyez un SMS au numéro Twilio (`+13022625021`) depuis votre téléphone
2. L'application devrait afficher une notification avec le message reçu

## 📱 Format des SMS reçus

Lorsqu'un SMS est reçu, l'application affiche :
- **De** : Numéro de l'expéditeur
- **Vers** : Numéro Twilio (destinataire)
- **Message** : Contenu du SMS
- **Date** : Date et heure de réception

## 🔍 Logs de débogage

Les SMS reçus sont également loggés dans la console avec le format :
```
=== SMS REÇU ===
De: +33123456789
Vers: +13022625021
Message: Bonjour, ceci est un test
SID: SM1234567890abcdef
Date: Mon, 01 Jan 2024 12:00:00 +0000
```

## 🛠️ Dépannage

### Le serveur ne démarre pas

**Erreur : "Address already in use"**
- Le port 8080 est déjà utilisé par une autre application
- Changez le port dans `mainwindow.cpp` : `m_smsReceiver->demarrerServeur(8081);`

### Les SMS ne sont pas reçus

1. **Vérifiez que le serveur est démarré** :
   - Regardez les logs au démarrage de l'application
   - Vous devriez voir : "Récepteur SMS démarré avec succès sur le port 8080"

2. **Vérifiez le webhook Twilio** :
   - Allez dans la console Twilio
   - Vérifiez que l'URL du webhook est correcte
   - Testez l'URL avec un outil comme Postman ou curl

3. **Vérifiez le pare-feu** :
   - Le port 8080 doit être ouvert
   - Si vous utilisez ngrok, vérifiez que le tunnel est actif

4. **Vérifiez les logs Twilio** :
   - Allez dans **Monitor** > **Logs** > **Messaging** dans la console Twilio
   - Vérifiez les tentatives de webhook et les erreurs éventuelles

### Test manuel du webhook

Vous pouvez tester le webhook avec curl :
```bash
curl -X POST http://localhost:8080/sms \
  -d "From=+33123456789" \
  -d "To=+13022625021" \
  -d "Body=Test message" \
  -d "MessageSid=SM1234567890"
```

## 📊 Améliorations futures

- [ ] Stocker les SMS reçus dans la base de données
- [ ] Interface pour afficher l'historique des SMS reçus
- [ ] Réponses automatiques aux SMS
- [ ] Filtrage des SMS par résident
- [ ] Export des SMS reçus en PDF

## 🔐 Sécurité

⚠️ **Important** : Pour la production, ajoutez :
- Authentification du webhook (vérification de la signature Twilio)
- HTTPS au lieu de HTTP
- Validation des données reçues
- Limitation du taux de requêtes

## 📝 Notes

- Le serveur écoute sur toutes les interfaces réseau (`0.0.0.0`)
- Le port par défaut est 8080 (modifiable dans le code)
- Les SMS sont traités de manière synchrone
- L'application répond avec du TwiML à Twilio (format XML)

