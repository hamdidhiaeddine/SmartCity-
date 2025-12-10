# Configuration Servo-Moteur pour Smart City

## 🔧 Matériel Requis

1. **Arduino Uno**
2. **Module RFID RC522** (lecteur de cartes RFID 13.56 MHz)
3. **Servo-moteur** (SG90 ou compatible)
4. **Câbles de connexion**
5. **Alimentation** (USB ou 5V externe si nécessaire)

## 📋 Schéma de Connexion

### RFID RC522 → Arduino Uno
| RC522 Pin | Arduino Pin |
|-----------|-------------|
| SDA       | 10          |
| SCK       | 13          |
| MOSI      | 11          |
| MISO      | 12          |
| IRQ       | Non connecté|
| GND       | GND         |
| RST       | 9           |
| 3.3V      | 3.3V        |

### Servo-Moteur → Arduino Uno
| Servo Pin | Arduino Pin |
|-----------|-------------|
| Signal (Orange/Jaune) | 6  |
| VCC (Rouge)          | 5V |
| GND (Marron/Noir)    | GND|

## 📚 Installation des Bibliothèques Arduino

Dans l'IDE Arduino :
1. **Sketch → Include Library → Manage Libraries**
2. Rechercher et installer :
   - `MFRC522` by GithubCommunity (pour RFID)
   - `Servo` (généralement déjà inclus)

## 📤 Téléversement du Code

1. Ouvrir `arduino_rfid_servo.ino` dans Arduino IDE
2. Sélectionner **Tools → Board → Arduino Uno**
3. Sélectionner le bon port COM (ex: COM3, COM4...)
4. Cliquer sur **Upload** (→)

## 🎯 Fonctionnement

### Lecture RFID
- L'Arduino lit automatiquement les cartes RFID approchées
- Le code est envoyé à Qt via USB/série
- Qt vérifie dans la base de données si le résident existe

### Contrôle du Servo
Quand un résident autorisé scanne sa carte :
1. **Qt envoie** la commande `OPEN` à l'Arduino
2. **Le servo tourne** à 90° (porte ouverte)
3. **Attente** de 3 secondes
4. **Le servo retourne** à 0° (porte fermée)

## 🔧 Configuration

### Modifier le délai d'ouverture
Dans `arduino_rfid_servo.ino`, ligne 31 :
```cpp
#define DELAI_OUVERTURE 3000  // 3000 ms = 3 secondes
```

### Modifier les angles du servo
```cpp
#define SERVO_FERME 0      // Position fermée
#define SERVO_OUVERT 90    // Position ouverte
```

## 🧪 Test du Système

### Test 1 : Vérifier la connexion
1. Ouvrir le **Moniteur Série** dans Arduino IDE (9600 bauds)
2. Vous devriez voir : `Systeme RFID + Servo pret`

### Test 2 : Test manuel du servo
Dans le moniteur série, taper : `OPEN` puis Enter
→ Le servo devrait tourner à 90° puis revenir à 0° après 3s

### Test 3 : Test RFID
1. Approcher une carte RFID du lecteur
2. Le code devrait s'afficher dans le moniteur série (ex: `A1B2C3D4`)

### Test 4 : Test complet avec Qt
1. Lancer l'application Qt
2. Scanner une carte RFID enregistrée (A1B2C3D4 ou E5F6G7H8)
3. Vérifier que :
   - Qt affiche "Accès Autorisé"
   - Le servo s'active automatiquement
   - Les logs Qt montrent "🚪 Ouverture de la porte..."

## 🐛 Dépannage

### Le servo ne bouge pas
- Vérifier que le servo est bien alimenté (5V)
- Vérifier la connexion du fil signal sur le pin 6
- Tester avec `OPEN` dans le moniteur série

### RFID ne détecte pas les cartes
- Vérifier que le module est alimenté en **3.3V** (pas 5V !)
- Vérifier toutes les connexions SPI
- Approcher la carte à moins de 3cm du lecteur

### Qt ne reçoit pas les données RFID
- Vérifier le port COM dans Qt (logs au démarrage)
- Fermer le moniteur série Arduino (un seul programme peut utiliser le port)
- Vérifier que l'Arduino est bien connecté automatiquement

## 📊 Codes RFID de Test

Dans la base de données :
- `A1B2C3D4` → Jean Dupont (Résident 1)
- `E5F6G7H8` → Marie Martin (Résident 2)

## 🔒 Sécurité

⚠️ **Important** : Ce système est à usage éducatif. Pour un usage réel :
- Ajouter un cryptage des communications
- Implémenter un système de logs d'accès
- Ajouter une authentification à deux facteurs
- Utiliser des cartes RFID sécurisées (Mifare DESFire)
