# ⚠️ CONFLIT: Arduino RFID vs Capteur DHT11

## 🔴 Problème Détecté

Les données du capteur DHT11 (température et humidité) sont interceptées par le système RFID qui les traite comme des cartes RFID inconnues :

```
RFID scanné: "23.80 °C"  ❌ Interprété comme carte RFID
RFID scanné: "55.00 %"   ❌ Interprété comme carte RFID
```

## 🎯 Cause du Problème

**Un seul Arduino ne peut pas gérer simultanément le RFID ET le DHT11** car ils partagent le même port série (COM). Toutes les données série sont envoyées au même flux.

## ✅ Solutions Possibles

### Solution 1: Utiliser 2 Arduino (RECOMMANDÉ)

**Matériel nécessaire:**
- Arduino #1: RFID RC522 + Servo
- Arduino #2: Capteur DHT11

**Configuration:**
```
Arduino #1 (RFID)          Ordinateur
├─ RFID RC522      ──────> COM3 (par exemple)
├─ Servo SG90
└─ LCD I2C (optionnel)

Arduino #2 (DHT11)         Ordinateur  
└─ DHT11           ──────> COM4 (par exemple)
```

**Code Arduino #1** (RFID): Utilisez `arduino_rfid_reader.ino` existant

**Code Arduino #2** (DHT11): Utilisez `arduino_dht11_temperature.ino`

**Configuration Qt:**
- L'application détectera automatiquement les 2 ports
- Arduino RFID sur le premier port trouvé
- Capteur DHT11 sur le second port

### Solution 2: Désactiver temporairement le RFID

Si vous n'avez qu'un seul Arduino et voulez tester le capteur température:

**Dans `mainwindow.cpp`, ligne ~115, commentez:**
```cpp
// Initialiser Arduino RFID
/*
arduinoRFID = new ArduinoRFID(this);
connect(arduinoRFID, &ArduinoRFID::rfidScanned, this, &MainWindow::onRFIDScanned);
// ... reste du code RFID
if (arduinoRFID->connectArduino()) {
    qDebug() << "✅ Arduino connecté automatiquement au démarrage";
}
*/
```

Puis téléversez `arduino_dht11_temperature.ino` sur votre Arduino.

### Solution 3: Désactiver temporairement le DHT11

Si vous voulez utiliser uniquement le RFID (configuration actuelle):

**Dans `mainwindow.cpp`, ligne ~140, commentez:**
```cpp
// Initialiser le capteur de température DHT11
/*
m_temperatureSensor = new TemperatureSensor(this);
connect(m_temperatureSensor, &TemperatureSensor::temperatureRecue, ...);
// ... reste du code capteur
*/
```

### Solution 4: Système hybride avec multiplexage (AVANCÉ)

Modifier le code Arduino pour envoyer des données préfixées:

**Arduino (RFID + DHT11):**
```cpp
// RFID détecté
Serial.println("RFID:1234567890");

// Température
Serial.println("TEMP:23.5");
Serial.println("HUM:55.0");
```

**Qt - Analyser le préfixe:**
```cpp
if (data.startsWith("RFID:")) {
    // Traiter comme RFID
} else if (data.startsWith("TEMP:")) {
    // Traiter comme température
}
```

## 🚀 Configuration Recommandée pour Production

### Scénario A: Gestion complète (RFID + Température)

**Matériel:**
- 2x Arduino Uno
- 1x RFID RC522
- 1x Servo SG90
- 1x Capteur DHT11
- 1x LCD I2C 16x2 (optionnel)

**Branchement:**

**Arduino #1 (COM3):**
```
RFID RC522:
  SDA  → Pin 10
  SCK  → Pin 13
  MOSI → Pin 11
  MISO → Pin 12
  RST  → Pin 9
  GND  → GND
  3.3V → 3.3V

Servo SG90:
  Signal → Pin 6
  VCC    → 5V
  GND    → GND

LCD I2C (optionnel):
  SDA → A4
  SCL → A5
  VCC → 5V
  GND → GND
```

**Arduino #2 (COM4):**
```
DHT11:
  VCC  → 5V
  DATA → Pin 2 (+ résistance 10kΩ vers 5V)
  GND  → GND
```

### Scénario B: Système minimal (RFID uniquement)

Utilisez la configuration actuelle avec `arduino_rfid_reader.ino` uniquement.

## 📝 État Actuel du Code

Le code est configuré pour **désactiver automatiquement le capteur DHT11** si le RFID est déjà connecté, évitant ainsi le conflit.

**Console au démarrage:**
```
========================================
🔌 INITIALISATION ARDUINO RFID
========================================
✅ Arduino connecté automatiquement sur COM3

========================================
🌡️ INITIALISATION CAPTEUR DHT11
========================================
⚠️ Arduino RFID déjà connecté - Capteur DHT11 désactivé
💡 Pour utiliser le capteur température:
   1. Utilisez un second Arduino sur un autre port COM
   2. OU commentez l'initialisation du RFID ci-dessus
========================================
```

## 🔧 Changement de Configuration

### Pour passer de RFID à DHT11:

1. **Débrancher Arduino**
2. **Commenter le code RFID dans mainwindow.cpp** (lignes ~115-130)
3. **Téléverser** `arduino_dht11_temperature.ino`
4. **Rebrancher Arduino**
5. **Recompiler et exécuter** l'application Qt
6. Vous verrez les alertes d'arrosage pour les jardins

### Pour utiliser les 2 systèmes simultanément:

1. **Acheter un second Arduino Uno** (~10€)
2. **Brancher Arduino #1** avec RFID sur COM3
3. **Brancher Arduino #2** avec DHT11 sur COM4
4. **Téléverser** `arduino_rfid_reader.ino` sur Arduino #1
5. **Téléverser** `arduino_dht11_temperature.ino` sur Arduino #2
6. **Lancer l'application** Qt

L'application détectera automatiquement les 2 Arduinos et fonctionnera avec les 2 systèmes.

## ✅ Résumé

| Système | Arduino | Port | Sketch | État Actuel |
|---------|---------|------|--------|-------------|
| **RFID** | Arduino #1 | COM3 | arduino_rfid_reader.ino | ✅ ACTIF |
| **DHT11** | Arduino #2 | COM4 | arduino_dht11_temperature.ino | ⚠️ DÉSACTIVÉ (même Arduino que RFID) |

**Pour activer le DHT11:**
- Utiliser un second Arduino OU
- Désactiver temporairement le RFID

Bon développement ! 🚀
