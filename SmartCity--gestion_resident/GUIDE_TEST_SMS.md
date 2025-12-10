# 📱 GUIDE RAPIDE - ENVOI SMS À +216510912755

## 🎯 3 FAÇONS D'ENVOYER UN SMS DE TEST

---

### 1️⃣ **AU DÉMARRAGE DE L'APPLICATION** (Automatique)

**Étapes :**

1. Exécutez l'application Qt (Ctrl+R)
2. Attendez la connexion à la base de données
3. Une popup apparaît : **"Voulez-vous envoyer un SMS de test à +216510912755 ?"**
4. Cliquez **OUI**
5. ✅ SMS envoyé !

**Message envoyé :**

```
Bonjour !

Ceci est un SMS de test depuis l'application SmartCity.

Date/Heure: 25/11/2025 17:30:45

Cordialement,
Équipe SmartCity
```

---

### 2️⃣ **VIA LE BOUTON "ENVOYER SMS"** (Interface)

**Étapes :**

1. Lancez l'application
2. Allez dans **"Gestion des résidents"**
3. Cliquez sur le bouton **"Envoyer SMS"** (même sans sélectionner de résident)
4. Une popup apparaît avec 2 options :
   - **OUI** : Envoyer un SMS de TEST à +216510912755
   - **NON** : Envoyer au résident sélectionné
5. Choisissez **OUI**
6. ✅ SMS envoyé !

---

### 3️⃣ **EN AJOUTANT UN RÉSIDENT** (Automatique)

**Option A : Créer un résident de test avec votre numéro**

1. Dans l'interface "Gestion des résidents"
2. Remplissez le formulaire :
   - **ID** : 9999
   - **Nom** : TEST
   - **Prénom** : SMS
   - **Date naissance** : 01/01/2000
   - **Adresse** : Tunisie
   - **Téléphone** : **+216510912755** ← VOTRE NUMÉRO
   - **Email** : test.sms@smartcity.tn
   - **Statut** : Actif
   - **Situation** : Célibataire
3. Cliquez **"Ajouter"**
4. ✅ SMS automatique envoyé !

**Message envoyé :**

```
Bonjour TEST SMS,

Votre inscription comme résident (ID: 9999) a été enregistrée.

Cordialement,
SmartCity
```

**Option B : Via SQL**

```cmd
cd c:\Users\Raouf\Desktop\ayooo
sqlplus soumaya/esprit18@Source_Projet2A @test_sms_personnel.sql
```

Puis décommentez les lignes INSERT dans le script.

---

## 🔧 CONFIGURATION ACTUELLE

**Compte Twilio :**

```
Account SID: AC069692f58baabea5aa96ccad4054734d
Auth Token: 50d89558d346b48a4d29b1768428f0aa
Numéro émetteur: +13022625021 (USA)
```

**Numéro destinataire :**

```
+216510912755 (Tunisie)
```

---

## ⚠️ VÉRIFICATIONS AVANT LE TEST

### ✅ Checklist :

- [ ] **Connexion Internet active** (pour API Twilio)
- [ ] **Compte Twilio actif** (vérifier sur https://console.twilio.com)
- [ ] **Numéro vérifié** (pour compte d'essai) :
  - Allez sur https://console.twilio.com/verified-caller-ids
  - Vérifiez que **+216510912755** est dans la liste
  - Sinon, cliquez "Add a new Caller ID" et suivez les instructions

### 🔍 Vérification du numéro :

**1. Dans Twilio Console :**

```
1. https://console.twilio.com/verified-caller-ids
2. Cherchez +216510912755
3. Status doit être "Verified" ✅
```

**2. Si non vérifié :**

```
1. Cliquez "Add a new Caller ID"
2. Entrez +216510912755
3. Recevez le code par SMS/appel
4. Validez le code
```

---

## 📊 MESSAGES DE DEBUG ATTENDUS

**Dans la console Qt :**

### ✅ **Succès :**

```
📱 Test d'envoi SMS à: +216510912755
Message: Bonjour !

Ceci est un SMS de test depuis l'application SmartCity.
...

Numéro original: +216510912755
Numéro normalisé: +216510912755
URL Twilio: https://api.twilio.com/2010-04-01/Accounts/.../Messages.json
Code HTTP: 201
SMS envoyé avec succès! Status: queued
SID: SM1234567890abcdef
✓ SMS de test envoyé avec succès
```

### ❌ **Erreur - Numéro non vérifié :**

```
Code HTTP: 400
Code d'erreur Twilio: 21608
Message d'erreur Twilio: The number is not a verified outgoing caller number
ERREUR: Le numéro n'est pas vérifié dans votre compte Twilio
Pour les comptes d'essai Twilio, vous devez vérifier les numéros de destination
```

**Solution :** Vérifiez le numéro dans Twilio Console (voir ci-dessus)

### ❌ **Erreur - Pas de crédit :**

```
Code d'erreur Twilio: 20003
Message: Insufficient credit
```

**Solution :** Rechargez votre compte Twilio

---

## 🚀 TEST RAPIDE - PAS À PAS

### **Option la plus rapide (30 secondes) :**

1. **Ouvrez Qt Creator**
2. **Recompilez** : Ctrl+Shift+B
3. **Exécutez** : Ctrl+R
4. **Attendez** la popup "connection successful"
5. **Cliquez OUI** pour le SMS de test
6. **Vérifiez votre téléphone** 📱

---

## 📞 RÉSULTAT ATTENDU SUR VOTRE TÉLÉPHONE

**Expéditeur :** +13022625021  
**Message :**

```
Bonjour !

Ceci est un SMS de test depuis l'application SmartCity.

Date/Heure: 25/11/2025 17:30:45

Cordialement,
Équipe SmartCity
```

**Délai de réception :**

- ⚡ Généralement 5-30 secondes
- 🌍 Peut aller jusqu'à 2-3 minutes (international)

---

## 🐛 PROBLÈMES COURANTS

### 1. **"Driver not loaded"**

**Cause :** Connexion DB avant chargement ODBC  
**Solution :** Déjà corrigé ! Recompilez.

### 2. **"Numéro non vérifié" (Code 21608)**

**Solution :**

```
1. https://console.twilio.com/verified-caller-ids
2. Add a new Caller ID
3. +216510912755
4. Entrez le code reçu par SMS
```

### 3. **"Pas de crédit"**

**Solution :**

- Compte d'essai : crédit gratuit limité
- Vérifiez : https://console.twilio.com/billing/usage

### 4. **SMS non reçu après 5 minutes**

**Vérifications :**

- Console Qt : Code HTTP = 201 ? ✅
- Twilio logs : https://console.twilio.com/monitor/logs/sms
- Numéro correct : +216510912755 ✅
- Téléphone allumé et réseau disponible 📱

---

## 🎉 COMMANDES UTILES

### **Vérifier les résidents en base :**

```sql
sqlplus soumaya/esprit18@Source_Projet2A
SELECT ID, NOM, PRENOM, TELEPHONE
FROM GEST_RESIDENT
WHERE TELEPHONE = '+216510912755';
```

### **Créer le résident de test :**

```sql
sqlplus soumaya/esprit18@Source_Projet2A @test_sms_personnel.sql
-- Puis décommentez l'INSERT dans le script
```

### **Vérifier l'historique des SMS (Twilio) :**

1. https://console.twilio.com/monitor/logs/sms
2. Cherchez le numéro : +216510912755
3. Vérifiez le statut : "delivered" ✅

---

## ✅ SUCCÈS !

**Si vous voyez :**

```
✓ SMS de test envoyé avec succès
```

**Et recevez le SMS sur votre téléphone → 🎉 TOUT FONCTIONNE !**

Vous pouvez maintenant :

- ✅ Envoyer des SMS aux résidents lors de l'ajout
- ✅ Envoyer des notifications personnalisées
- ✅ Utiliser le bouton "Envoyer SMS" dans l'interface

---

**Bon test ! 📱🚀**
