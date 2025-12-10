# Fonctionnalité SMS via Twilio

## 📱 Description

Cette fonctionnalité permet d'envoyer automatiquement des SMS de notification aux résidents lors des opérations CRUD (ajout, modification, suppression).

## 🔑 Configuration

Les identifiants Twilio sont configurés dans `smsmanager.cpp` :
- **Account SID** : AC069692f58baabea5aa96ccad4054734d
- **Auth Token** : 50d89558d346b48a4d29b1768428f0aa
- **Numéro Twilio** : +13022625021

## 🚀 Utilisation

### Envoi automatique

Les SMS sont envoyés automatiquement lors de :
1. **Ajout d'un résident** : Notification d'inscription
2. **Modification d'un résident** : Notification de mise à jour
3. **Suppression d'un résident** : Notification de suppression

### Format des numéros

Le système normalise automatiquement les numéros de téléphone :
- Les numéros commençant par `0` sont convertis en format international (+33 pour la France)
- Les numéros sans `+` reçoivent automatiquement le préfixe `+`

**Exemples :**
- `0123456789` → `+33123456789`
- `123456789` → `+123456789`
- `+33123456789` → `+33123456789` (déjà correct)

## 📝 Messages envoyés

### Ajout de résident
```
Bonjour [Nom Prénom],

Votre inscription en tant que résident a été enregistrée avec succès.

Cordialement,
Équipe SMARTCITY
```

### Modification de résident
```
Bonjour [Nom Prénom],

Vos informations de résident ont été mises à jour.

Cordialement,
Équipe SMARTCITY
```

### Suppression de résident
```
Bonjour [Nom Prénom],

Votre dossier de résident a été supprimé de notre système.

Cordialement,
Équipe SMARTCITY
```

## 🔧 Utilisation programmatique

```cpp
#include "smsmanager.h"

// Envoyer un SMS personnalisé
SmsManager::envoyerSms("+33123456789", "Votre message ici");

// Utiliser les fonctions de notification
SmsManager::notifierAjoutResident("+33123456789", "Dupont Jean");
```

## ⚠️ Notes importantes

1. **Coûts** : Chaque SMS envoyé via Twilio peut avoir un coût. Vérifiez votre plan Twilio.
2. **Format des numéros** : Assurez-vous que les numéros sont au format international valide.
3. **Erreurs** : Les erreurs d'envoi sont loggées dans la console de débogage.
4. **Réseau** : Une connexion Internet est requise pour envoyer les SMS.

## 🐛 Dépannage

Si les SMS ne sont pas envoyés :
1. Vérifiez votre connexion Internet
2. Vérifiez que les identifiants Twilio sont corrects
3. Vérifiez les logs de débogage dans la console
4. Vérifiez que le numéro Twilio est actif dans votre compte
5. Vérifiez le format du numéro de téléphone du destinataire

## 📊 Logs

Les logs de débogage affichent :
- Le numéro de destination
- Le statut de l'envoi
- Les erreurs éventuelles
- Le SID du message (si succès)

