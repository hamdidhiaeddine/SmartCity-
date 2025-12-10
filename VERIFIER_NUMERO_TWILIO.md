# 📱 VÉRIFIER VOTRE NUMÉRO SUR TWILIO

## ❌ Problème Actuel

```
Code d'erreur Twilio: 21211
Message: "Invalid 'To' Phone Number: 21651091XXXX"
```

**Cause :** Votre compte Twilio est un compte **Trial (gratuit)**.
Les comptes Trial ne peuvent envoyer des SMS qu'à des numéros **vérifiés**.

---

## ✅ SOLUTION : Vérifier +216510912755 sur Twilio

### **Étape 1 : Connexion à Twilio Console**

1. Allez sur : https://console.twilio.com/
2. Connectez-vous avec votre compte Twilio

---

### **Étape 2 : Vérifier votre numéro**

1. Dans le menu de gauche, cliquez sur **"Phone Numbers"**
2. Cliquez sur **"Verified Caller IDs"**
3. Cliquez sur le bouton rouge **"+ Add a new Caller ID"**

4. **Entrez votre numéro :** `+216510912755`
5. Choisissez la méthode de vérification : **SMS** ou **Call**
6. Cliquez sur **"Verify"**

7. Vous recevrez un **code de vérification** par SMS ou appel
8. Entrez le code dans la console Twilio
9. Cliquez sur **"Submit"**

---

### **Étape 3 : Vérification réussie**

Vous verrez votre numéro dans la liste **"Verified Caller IDs"** :

- ✅ +216510912755 (Verified)

---

### **Étape 4 : Tester à nouveau**

1. Relancez votre application Qt
2. Cliquez **OUI** pour envoyer le SMS de test
3. Vous devriez voir :
   ```
   Code HTTP: 201
   SMS envoyé avec succès! Status: queued
   ✓ SMS de test envoyé avec succès
   ```

---

## 🆓 COMPTE TRIAL VS COMPTE PAYANT

### **Compte Trial (Gratuit) - Limitations :**

- ✅ Crédit gratuit : ~15-16 USD
- ❌ SMS uniquement vers numéros vérifiés
- ❌ SMS contient "[Sent from your Twilio trial account]"
- ❌ Limite de numéros vérifiés : 10 numéros max

### **Compte Payant (Upgrade) - Avantages :**

- ✅ Envoyer SMS à n'importe quel numéro
- ✅ Pas de message "trial account"
- ✅ Nombre illimité de destinataires
- 💰 **Coût :** ~10-15 USD minimum pour activer

---

## 🔄 Upgrader votre compte (Optionnel)

Si vous voulez envoyer des SMS à n'importe quel numéro sans vérification :

1. Allez sur : https://console.twilio.com/billing
2. Cliquez sur **"Upgrade Your Account"**
3. Ajoutez une carte de crédit
4. Ajoutez un crédit minimum (10-20 USD)

**Coût par SMS Tunisie :** ~0.05-0.10 USD par SMS

---

## 📋 ALTERNATIVE : Tester avec un autre numéro

Si vous ne pouvez pas vérifier +216510912755, vous pouvez :

### **Option 1 : Utiliser un autre numéro (déjà vérifié)**

Si vous avez déjà vérifié un autre numéro lors de l'inscription Twilio

### **Option 2 : Utiliser un numéro virtuel gratuit**

Services comme :

- https://receive-sms-online.info/
- https://sms-online.co/receive-free-sms

**Attention :** Ces numéros sont publics, à utiliser uniquement pour les tests !

---

## 🎯 RÉSUMÉ - QUE FAIRE MAINTENANT ?

### **Solution Immédiate (Gratuit) :**

1. Connectez-vous à https://console.twilio.com/
2. Allez dans **Phone Numbers → Verified Caller IDs**
3. Cliquez **"+ Add a new Caller ID"**
4. Entrez **+216510912755**
5. Vérifiez avec le code SMS reçu
6. Testez à nouveau l'envoi

**Temps requis :** 2-3 minutes

---

### **Solution Long Terme (Payant) :**

1. Upgradez votre compte Twilio
2. Ajoutez 10-20 USD de crédit
3. Envoyez des SMS à n'importe quel numéro

**Coût :** 10-20 USD + 0.05 USD par SMS

---

## 🔍 Vérifier l'état de votre compte

**Console Twilio → Account → Trial Information**

Vous verrez :

```
Trial Account Balance: $15.50 (example)
Verified Phone Numbers: 0/10
```

Après vérification :

```
Verified Phone Numbers: 1/10
  ✅ +216510912755 (Verified)
```

---

## ✅ TEST FINAL

Une fois le numéro vérifié, relancez l'application et vous devriez voir :

```
Code HTTP: 201
Réponse: {"sid":"SM1234...","status":"queued",...}
SMS envoyé avec succès! Status: queued
SID: SM1234567890abcdef
✓ SMS de test envoyé avec succès
```

Et recevoir le SMS sur +216510912755 dans 5-30 secondes ! 📲

---

**Lien direct pour vérifier :** https://console.twilio.com/us1/develop/phone-numbers/manage/verified
