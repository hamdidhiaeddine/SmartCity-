# Système d'Alerte Arrosage Intelligent avec Capteur DHT11

## 📋 Description

Ce système utilise un capteur de température et d'humidité DHT11 pour surveiller les conditions environnementales et générer automatiquement des alertes d'arrosage pour les jardins.

## 🎯 Fonctionnement

### Principe de détection
- Le capteur DHT11 mesure la **température ambiante** et l'**humidité** toutes les 2 secondes
- Les données sont envoyées via port série à l'application Qt
- L'application compare la température ambiante avec la **température moyenne du sol** de chaque jardin stockée en base de données
- **Règle d'alerte** : Si `température_ambiante > température_sol_jardin`, le jardin nécessite un arrosage

### Logique métier
Lorsque la température de l'air est **supérieure** à celle du sol d'un jardin, cela indique que :
- Le sol est plus froid que l'air ambiant
- Le jardin risque un stress thermique
- L'évaporation sera importante
- **=> Arrosage recommandé**

## 🔧 Installation Matérielle

### Composants requis
- 1x Arduino Uno (ou compatible)
- 1x Capteur DHT11 (température + humidité)
- 1x Résistance 10kΩ (si module sans résistance intégrée)
- Câbles de connexion

### Schéma de câblage

#### Version avec module DHT11 (3 pins)
```
DHT11 Module          Arduino Uno
─────────────────     ───────────
+ (VCC)       ──────> 5V
OUT (DATA)    ──────> Pin Digital 2
- (GND)       ──────> GND
```

#### Version avec DHT11 brut (4 pins)
```
DHT11 (4 pins)        Arduino Uno
─────────────────     ───────────
Pin 1 (VCC)   ──────> 5V
Pin 2 (DATA)  ──────> Pin Digital 2 (+ résistance 10kΩ vers 5V)
Pin 3 (NC)    ──────> Non connecté
Pin 4 (GND)   ──────> GND
```

**Important** : Une résistance pull-up de 10kΩ entre DATA et VCC est nécessaire pour les capteurs sans module.

## 💻 Installation Logicielle

### 1. Bibliothèque Arduino
Dans l'IDE Arduino :
1. Menu : **Croquis** > **Inclure une bibliothèque** > **Gérer les bibliothèques**
2. Rechercher : `DHT sensor library`
3. Installer : **DHT sensor library by Adafruit**
4. Installer aussi : **Adafruit Unified Sensor** (dépendance automatique)

### 2. Téléversement du sketch
1. Ouvrir `arduino_dht11_temperature.ino` dans Arduino IDE
2. Sélectionner votre carte : **Outils** > **Type de carte** > **Arduino Uno**
3. Sélectionner le port COM : **Outils** > **Port** > **COMx**
4. Cliquer sur **Téléverser** (flèche →)

### 3. Compilation Qt
Les fichiers suivants ont été ajoutés au projet :
- `temperaturesensor.h` / `temperaturesensor.cpp` - Classe de gestion du capteur
- `arduino_dht11_temperature.ino` - Code Arduino
- Mis à jour : `mainwindow.h`, `mainwindow.cpp`, `projetsmartcity.pro`

Compilez normalement avec qmake/make.

## 📊 Base de données

### Table GEST_JARDIN
La table contient déjà la colonne `TEMPERATURE_MOYENNE_SOL` :

```sql
CREATE TABLE GEST_JARDIN (
    ID_JARDIN NUMBER PRIMARY KEY,
    EMPLACEMENT VARCHAR2(100),
    SUPERFICIE NUMBER(10,2),
    TYPE_SOL VARCHAR2(50),
    TEMPERATURE_MOYENNE_SOL NUMBER(5,2),  -- En °C
    TYPE_CHOIX VARCHAR2(50)
);
```

Exemple de données :
```sql
INSERT INTO GEST_JARDIN VALUES (1, 'Jardin A', 50.00, 'Argileux', 18.5, 'Potager');
INSERT INTO GEST_JARDIN VALUES (2, 'Jardin B', 75.00, 'Sableux', 22.0, 'Fleurs');
```

## 🚀 Utilisation

### Démarrage automatique
L'application détecte et connecte automatiquement le capteur DHT11 au démarrage si disponible.

### Connexion manuelle
Si la connexion automatique échoue :
```cpp
// Dans l'interface (si des boutons sont ajoutés)
Bouton "Connecter Capteur" → onConnecterCapteurTemperature()
Bouton "Déconnecter Capteur" → onDeconnecterCapteurTemperature()
```

### Affichage des données
Les données sont affichées en temps réel dans l'interface (si widgets ajoutés) :
- `labelTemperature` : Température actuelle en °C
- `labelHumidite` : Humidité actuelle en %

### Alertes d'arrosage
Toutes les **5 secondes**, le système vérifie automatiquement :
1. Récupère la température du capteur
2. Compare avec tous les jardins en base
3. Si `temp_capteur > temp_sol_jardin` :
   - Affiche une **boîte de dialogue** avec la liste des jardins
   - Affiche dans la **console** (qDebug)

Exemple d'alerte :
```
💦 ALERTE ARROSAGE

La température ambiante est élevée.
2 jardin(s) nécessite(nt) un arrosage :

• Jardin #1 (Jardin A) - Sol: 18.5°C < Ambiant: 25.3°C
• Jardin #3 (Jardin C) - Sol: 20.0°C < Ambiant: 25.3°C
```

## 📝 Format de communication série

### Arduino → Qt
```
25.30 °C
65.00 %
26.10 °C
64.50 %
...
```

- Température suivie de " °C" et saut de ligne
- Humidité suivie de " %" et saut de ligne
- Cycle toutes les 2 secondes

### Paramètres série
- **Baud rate** : 9600
- **Data bits** : 8
- **Parity** : None
- **Stop bits** : 1
- **Flow control** : None

## 🐛 Dépannage

### Capteur ne se connecte pas
- Vérifier que le port COM est disponible (pas utilisé par Arduino RFID)
- Vérifier le câblage (DATA sur pin 2)
- Vérifier que le sketch est bien téléversé

### Valeurs NaN ou aberrantes
- Vérifier la résistance pull-up (10kΩ entre DATA et VCC)
- Attendre 2-3 secondes après le démarrage (stabilisation)
- Vérifier l'alimentation (5V stable)

### Pas d'alerte d'arrosage
- Vérifier que la table GEST_JARDIN contient des données
- Vérifier que `TEMPERATURE_MOYENNE_SOL` est remplie
- La température du capteur doit être **supérieure** à celle des jardins

### Plusieurs Arduino (RFID + DHT11)
Si vous utilisez 2 Arduino :
1. **Arduino 1** (RFID) : sur COMx
2. **Arduino 2** (DHT11) : sur COMy (différent)

L'application détectera automatiquement le second port libre.

## 📈 Exemple de scénario

### Situation
- **Jardin A** : Sol argileux, température moyenne sol = **18.5°C**
- **Jardin B** : Sol sableux, température moyenne sol = **22.0°C**
- **Capteur DHT11** : Température ambiante = **25.3°C**

### Résultat
```
💦 ALERTE : 2 jardins à arroser
- Jardin A : 18.5°C < 25.3°C → ARROSER
- Jardin B : 22.0°C < 25.3°C → ARROSER
```

Si température descend à **21.0°C** :
```
💦 ALERTE : 1 jardin à arroser  
- Jardin A : 18.5°C < 21.0°C → ARROSER
- Jardin B : 22.0°C > 21.0°C → OK (pas d'arrosage)
```

## 🔗 Intégration avec les autres modules

Ce système s'intègre parfaitement avec :
- **Module Jardin** : Utilise `GEST_JARDIN` et `TEMPERATURE_MOYENNE_SOL`
- **Module Maintenance** : Les alertes peuvent déclencher des tâches de maintenance
- **Module Recommandation** : Suggestion de plantes selon température

## ✅ Checklist d'installation

- [ ] Bibliothèque DHT installée dans Arduino IDE
- [ ] Capteur DHT11 câblé correctement (avec résistance si nécessaire)
- [ ] Sketch `arduino_dht11_temperature.ino` téléversé
- [ ] Arduino connecté à un port COM libre
- [ ] Table `GEST_JARDIN` contient des données avec `TEMPERATURE_MOYENNE_SOL`
- [ ] Application Qt compilée avec les nouveaux fichiers
- [ ] Test de réception des données (console qDebug)
- [ ] Test d'alerte d'arrosage avec température > sol

## 📞 Support

En cas de problème :
1. Vérifier le **Moniteur Série Arduino** (9600 bauds) : les données doivent s'afficher
2. Vérifier la **console Qt** (qDebug) : les températures doivent être loggées
3. Vérifier la **base de données** : `SELECT * FROM GEST_JARDIN;`

Bonne irrigation intelligente ! 💧🌱
