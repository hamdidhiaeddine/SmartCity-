# Intégration Arduino RFID - Système de Contrôle d'Accès

## Vue d'ensemble
Ce système permet de scanner des badges RFID pour identifier les résidents et afficher leurs informations ainsi que leur maison assignée.

## Matériel Nécessaire

### Composants
- **Arduino Uno** (ou compatible)
- **Module RFID RC522** (lecteur RFID 13.56 MHz)
- **Badges/Cartes RFID** (compatibles ISO 14443A)
- **Câbles de connexion**
- **Câble USB** (Arduino vers PC)

### Connexions RC522 → Arduino Uno
```
RC522 Pin    →    Arduino Pin
─────────────────────────────
SDA/SS       →    10
SCK          →    13
MOSI         →    11
MISO         →    12
IRQ          →    Non connecté
GND          →    GND
RST          →    9
3.3V         →    3.3V
```

⚠️ **ATTENTION**: Le RC522 fonctionne en 3.3V, ne pas connecter à 5V !

## Installation

### 1. Configuration de la Base de Données

Exécutez le script SQL :
```bash
sqlplus wala/esprit18@localhost:1521/XE @add_rfid_column.sql
```

Ou manuellement :
```sql
ALTER TABLE GEST_RESIDENT ADD (RFID_CODE VARCHAR2(50));
CREATE INDEX idx_rfid_code ON GEST_RESIDENT(RFID_CODE);
COMMIT;
```

### 2. Programmation de l'Arduino

1. Ouvrez **Arduino IDE**
2. Installez la bibliothèque **MFRC522** :
   - Outils → Gérer les bibliothèques
   - Recherchez "MFRC522"
   - Installez "MFRC522 by GithubCommunity"
3. Ouvrez le fichier `arduino_rfid_reader.ino`
4. Sélectionnez votre carte : Outils → Type de carte → Arduino Uno
5. Sélectionnez le port COM : Outils → Port → COM3 (ou votre port)
6. Téléversez le code : Sketch → Téléverser

### 3. Test du Lecteur RFID

1. Ouvrez le moniteur série : Outils → Moniteur série
2. Réglez le débit à **9600 bauds**
3. Approchez un badge RFID du lecteur
4. Vous devriez voir le code RFID s'afficher (ex: "A1B2C3D4E5F6")

## Utilisation dans l'Application Qt

### Connexion de l'Arduino

1. Branchez l'Arduino en USB
2. Lancez l'application Smart City
3. Allez dans l'onglet **Gestion Maisons**
4. Cliquez sur **🔌 Connecter Arduino**
5. Si plusieurs ports sont disponibles, sélectionnez le bon port COM
6. Message de confirmation : "✓ Arduino connecté avec succès !"

### Enregistrement des Codes RFID

**Option 1 : Manuellement dans la base**
```sql
UPDATE GEST_RESIDENT SET RFID_CODE = 'A1B2C3D4' WHERE ID_RES = 1;
```

**Option 2 : Via l'application**
1. Scannez un badge RFID (l'app affichera "RFID inconnu")
2. Notez le code affiché
3. Dans la gestion des résidents, modifiez le résident
4. Ajoutez le code RFID dans le champ approprié
5. Sauvegardez

### Scan d'un Badge RFID

1. Arduino connecté et actif
2. Approchez le badge du lecteur RC522
3. **Accès Autorisé** : Une fenêtre s'affiche avec :
   - ✅ Nom et prénom du résident
   - 📧 Email
   - 📞 Téléphone
   - 🏠 Adresse de la maison assignée
   - 📐 Type et superficie de la maison

4. **Accès Refusé** : Si le code RFID n'est pas dans la base
   - ❌ Message d'erreur avec le code scanné

## Flux de Fonctionnement

```
┌─────────────────┐
│  Badge RFID     │
│  (13.56 MHz)    │
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│  Lecteur RC522  │ ← Lecture de l'UID
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│  Arduino Uno    │ ← Conversion en HEX
└────────┬────────┘
         │
         ↓ Serial (9600 bauds)
┌─────────────────┐
│  Port COM (USB) │
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│  Qt Application │ ← Réception du code RFID
│  (QSerialPort)  │
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│  Base Oracle    │ ← Recherche du résident
│  GEST_RESIDENT  │
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│  Affichage      │ ← Message d'accès
│  (QMessageBox)  │
└─────────────────┘
```

## Fonctionnalités

### Classe ArduinoRFID
- **connectArduino()** : Connexion au port série
- **disconnectArduino()** : Déconnexion
- **autoDetectArduino()** : Détection automatique du port
- **Signal rfidScanned()** : Émis quand un badge est scanné

### MainWindow Integration
- **onRFIDScanned()** : Traite le code RFID et affiche les infos
- **onConnecterArduino()** : Interface de connexion
- **onDeconnecterArduino()** : Déconnexion de l'Arduino

## Codes RFID de Test

Le script SQL ajoute 3 codes de test :
```
Résident 1 : A1B2C3D4
Résident 2 : E5F6G7H8
Résident 3 : I9J0K1L2
```

## Dépannage

### Arduino non détecté
- Vérifiez le câble USB
- Vérifiez que les drivers CH340/FTDI sont installés
- Essayez un autre port USB
- Redémarrez l'Arduino (bouton RESET)

### Badge non lu
- Vérifiez les connexions du RC522
- Vérifiez l'alimentation 3.3V
- Badge trop éloigné (max 3-5 cm)
- Badge incompatible (doit être ISO 14443A)

### Code RFID non reconnu
- Vérifiez que le code est bien enregistré dans la base
- Vérifiez la requête SQL : `SELECT * FROM GEST_RESIDENT WHERE RFID_CODE IS NOT NULL`
- Le code doit être en MAJUSCULES

### Erreur de port COM
```
Erreur: Impossible de se connecter à l'Arduino
```
- Fermez l'Arduino IDE (libère le port série)
- Vérifiez dans Gestionnaire de périphériques → Ports (COM et LPT)
- Essayez de sélectionner manuellement le port

## Exemple de Log

```
Lecteur RFID initialisé
En attente de badge...
A1B2C3D4E5F6
```

Application Qt :
```
🔍 RFID scanné: A1B2C3D4E5F6
✓ Accès autorisé pour: Dupont Jean
```

## Améliorations Possibles

1. **LED d'état** :
   - Verte : Accès autorisé
   - Rouge : Accès refusé
   
2. **Buzzer** :
   - Bip court : Badge reconnu
   - Bip long : Badge refusé

3. **LCD 16x2** :
   - Affichage du nom du résident
   - Statut de l'accès

4. **Historique des accès** :
   - Logger chaque scan dans une table ACCES_LOG
   - Horodatage des entrées/sorties

5. **Mode enregistrement** :
   - Scanner un badge pour l'associer automatiquement à un résident

## Sécurité

⚠️ **Limitations du système RFID RC522** :
- Les badges RFID 13.56 MHz peuvent être clonés
- Pas de chiffrement sur les badges basiques
- Portée limitée (3-5 cm) est une sécurité

**Recommandations** :
- Utilisez des badges MIFARE Classic ou DESFire pour plus de sécurité
- Combinez avec un code PIN pour double authentification
- Logs d'accès pour traçabilité
- Mise à jour régulière de la base de données

## Support

En cas de problème :
1. Vérifiez les logs de debug (qDebug dans la console)
2. Testez l'Arduino indépendamment (moniteur série)
3. Vérifiez la base de données (codes RFID enregistrés)
