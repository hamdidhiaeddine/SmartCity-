# 🔍 Diagnostic - Erreur d'envoi SMS

## ❌ Problème
L'application affiche: **"Échec de l'envoi du SMS de test"**

## 🔎 Causes possibles

### 1. **Problème SSL/OpenSSL** (Le plus fréquent)
- Qt ne peut pas établir une connexion HTTPS sécurisée vers Twilio
- **Symptôme**: Erreur "SslHandshakeFailedError" dans la console
- **Solution**: Voir `FIX_SSL_PROBLEME.md` ou exécuter `installer_ssl.bat`

### 2. **Credentials Twilio invalides**
- Account SID ou Auth Token incorrect
- **Vérification**: Consultez votre console Twilio
- **Fichier**: `smsmanager.cpp` lignes 16-18

### 3. **Numéro non vérifié** (Compte d'essai)
- Pour les comptes Twilio d'essai, les numéros doivent être vérifiés
- **Erreur Twilio**: Code 21608
- **Solution**: Vérifiez le numéro sur https://console.twilio.com

### 4. **Numéro invalide**
- Format de numéro incorrect
- **Erreur Twilio**: Code 21211
- **Format attendu**: +216XXXXXXXX (format international)

### 5. **Problème de connexion réseau**
- Pas d'accès Internet
- Pare-feu bloque l'application
- **Vérification**: Testez votre connexion

## ✅ Solutions étape par étape

### Étape 1: Vérifier la console
Regardez la console de l'application pour voir les messages d'erreur détaillés.

### Étape 2: Vérifier OpenSSL
Si vous voyez "SslHandshakeFailedError":
```bash
# Exécutez le script d'installation SSL
installer_ssl.bat
```

Ou suivez les instructions dans `FIX_SSL_PROBLEME.md`

### Étape 3: Vérifier les credentials Twilio
Dans `smsmanager.cpp`, vérifiez:
```cpp
QString SmsManager::m_accountSid = "AC069692f58baabea5aa96ccad4054734d";
QString SmsManager::m_authToken = "50d89558d346b48a4d29b1768428f0aa";
QString SmsManager::m_numeroTwilio = "+13022625021";
```

**Vérifiez dans votre console Twilio** que ces valeurs sont correctes.

### Étape 4: Vérifier le numéro de test
Le numéro de test est: `+21628500392`

**Pour un compte d'essai Twilio**, vous devez:
1. Aller sur: https://console.twilio.com/us1/develop/phone-numbers/manage/verified
2. Cliquer sur "Add a new number"
3. Ajouter: `+21628500392`
4. Entrer le code de vérification reçu par SMS

### Étape 5: Tester la connexion
```bash
# Testez avec curl (si disponible)
curl -X POST https://api.twilio.com/2010-04-01/Accounts/AC069692f58baabea5aa96ccad4054734d/Messages.json \
  --data-urlencode "From=+13022625021" \
  --data-urlencode "To=+21628500392" \
  --data-urlencode "Body=Test" \
  -u AC069692f58baabea5aa96ccad4054734d:50d89558d346b48a4d29b1768428f0aa
```

## 📋 Checklist de diagnostic

- [ ] La console affiche des messages d'erreur détaillés
- [ ] OpenSSL est installé et configuré
- [ ] Les credentials Twilio sont corrects
- [ ] Le numéro de test est vérifié (compte d'essai)
- [ ] La connexion Internet fonctionne
- [ ] Le pare-feu n'bloque pas l'application
- [ ] Le numéro est au format international (+216...)

## 🔧 Messages d'erreur courants

### "SslHandshakeFailedError"
→ **Solution**: Installer OpenSSL (voir `FIX_SSL_PROBLEME.md`)

### "Code HTTP: 0"
→ **Solution**: Problème de connexion réseau ou SSL

### "Code 21211: Invalid 'To' Phone Number"
→ **Solution**: Vérifier le format du numéro (+216XXXXXXXX)

### "Code 21608: The number +216... is unverified"
→ **Solution**: Vérifier le numéro dans la console Twilio

### "Code 20003: Authenticate"
→ **Solution**: Vérifier Account SID et Auth Token

## 💡 Après correction

1. Recompilez l'application
2. Relancez le test SMS
3. Vérifiez la console pour les nouveaux messages détaillés
4. Si ça fonctionne, vous verrez "Code HTTP: 201" et "SMS envoyé avec succès!"

## 📝 Fichiers utiles

- `FIX_SSL_PROBLEME.md` - Solution pour les problèmes SSL
- `installer_ssl.bat` - Script d'installation OpenSSL
- `GUIDE_TEST_SMS.md` - Guide complet de test SMS
- `README_SMS_COMPLET.md` - Documentation complète SMS




