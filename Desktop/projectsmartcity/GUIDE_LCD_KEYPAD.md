# Guide d'intégration Arduino LCD + Keypad 4x4

## 📋 Vue d'ensemble

Ce guide explique comment utiliser le système de saisie de matricules via un clavier 4x4 connecté à un Arduino avec écran LCD I2C pour rechercher automatiquement des véhicules dans votre application Qt Smart City.

---

## 🔌 Matériel requis

1. **Arduino Uno** (ou compatible)
2. **LCD I2C 16x2** (adresse 0x27)
3. **Clavier matriciel 4x4**
4. **Câbles de connexion**

---

## 📐 Schéma de connexion

### LCD I2C (4 fils)
```
LCD I2C    →    Arduino
─────────────────────────
VCC        →    5V
GND        →    GND
SDA        →    A4
SCL        →    A5
```

### Keypad 4x4 (8 fils)
```
Keypad     →    Arduino
─────────────────────────
Rangée 1   →    Pin 9
Rangée 2   →    Pin 8
Rangée 3   →    Pin 7
Rangée 4   →    Pin 6
Colonne 1  →    Pin 5
Colonne 2  →    Pin 4
Colonne 3  →    Pin 3
Colonne 4  →    Pin 2
```

### Disposition des touches
```
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ T │
├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ N │
├───┼───┼───┼───┤
│ 7 │ 8 │ 9 │ - │
├───┼───┼───┼───┤
│ * │ 0 │ # │ D │
└───┴───┴───┴───┘

* = Effacer
# = Envoyer/Valider
```

---

## 📚 Installation des bibliothèques Arduino

Ouvrez l'IDE Arduino et installez les bibliothèques suivantes :

### 1. LiquidCrystal_I2C
```
Croquis → Inclure une bibliothèque → Gérer les bibliothèques
Rechercher: "LiquidCrystal I2C"
Installer: "LiquidCrystal_I2C" par Frank de Brabander
```

### 2. Keypad
```
Croquis → Inclure une bibliothèque → Gérer les bibliothèques
Rechercher: "Keypad"
Installer: "Keypad" par Mark Stanley
```

---

## 🔧 Configuration Arduino

1. **Ouvrir le sketch**
   - Fichier: `arduino_lcd_keypad.ino`

2. **Vérifier l'adresse I2C** (si nécessaire)
   ```cpp
   // Par défaut: 0x27
   LiquidCrystal_I2C lcd(0x27, 16, 2);
   
   // Si votre LCD utilise 0x3F:
   LiquidCrystal_I2C lcd(0x3F, 16, 2);
   ```

3. **Téléverser le sketch**
   ```
   Outils → Port → Sélectionner le port COM de l'Arduino
   Téléverser (Ctrl+U)
   ```

---

## 🖥️ Configuration Qt

### Fichiers ajoutés au projet

```
arduinolcd.h           - Classe de gestion du LCD + Keypad
arduinolcd.cpp         - Implémentation
arduino_lcd_keypad.ino - Sketch Arduino
```

### Modifications apportées

**projetsmartcity.pro**
```qmake
SOURCES += \
    ... \
    arduinolcd.cpp

HEADERS += \
    ... \
    arduinolcd.h
```

**mainwindow.h**
```cpp
#include "arduinolcd.h"

private:
    ArduinoLCD *arduinoLCD;    // Gestionnaire LCD + Keypad

private slots:
    void onConnecterLCD();
    void onDeconnecterLCD();
    void onToucheKeypadPressee(const QString &touche);
    void onDonneesKeypadEnvoyees(const QString &donnees);
```

**mainwindow.cpp**
```cpp
// Initialisation dans le constructeur
arduinoLCD = new ArduinoLCD(this);
connect(arduinoLCD, &ArduinoLCD::touchePressee, this, &MainWindow::onToucheKeypadPressee);
connect(arduinoLCD, &ArduinoLCD::donneesEnvoyees, this, &MainWindow::onDonneesKeypadEnvoyees);
```

---

## 🎮 Utilisation

### 1. Connexion à l'Arduino

1. Branchez l'Arduino avec le LCD et le Keypad
2. Dans l'application Qt, cliquez sur **"Connecter LCD"** (bouton `btnConnecterLCD`)
3. L'application détecte automatiquement le port COM
4. Un message confirme la connexion

### 2. Saisir une matricule avec le clavier 4x4

```
Exemple: Saisir "123TN456"

Appuyez sur: 1 → 2 → 3 → T → N → 4 → 5 → 6 → #
             │                                 │
             └─────────────┬───────────────────┘
                           │
                    Valider avec #
```

**Actions des touches spéciales:**
- `*` : Efface tout et recommence
- `#` : Valide et lance automatiquement la recherche

### 3. Recherche automatique

Lorsque vous appuyez sur `#`:
1. La matricule s'affiche dans `lineEditMatriculeLCD`
2. Le bouton `btnRechercherMatriculeLCD` est automatiquement cliqué
3. Le véhicule est recherché dans la base de données
4. Les informations s'affichent sur le LCD I2C

---

## 🔍 Fonctionnement détaillé

### Flux de données

```
┌─────────────┐
│  Keypad 4x4 │
└──────┬──────┘
       │ Touche pressée
       ▼
┌─────────────────┐
│  Arduino (C++)  │
│  - Lit touche   │
│  - Affiche LCD  │
│  - Envoie série │
└──────┬──────────┘
       │ USB Série
       ▼
┌──────────────────────┐
│  Qt (ArduinoLCD)     │
│  - Reçoit touche     │
│  - Ajoute au champ   │
│  - Détecte #         │
└──────┬───────────────┘
       │ Signal donneesEnvoyees
       ▼
┌──────────────────────────┐
│  MainWindow              │
│  - Rempli lineEdit       │
│  - Click btnRechercher   │
└──────┬───────────────────┘
       │
       ▼
┌──────────────────────────┐
│  Recherche BD            │
│  - SELECT vehicule       │
│  - Affiche résultats     │
│  - Envoie au LCD         │
└──────────────────────────┘
```

### Signaux Qt émis

**ArduinoLCD::touchePressee(QString)**
- Émis à chaque touche pressée (sauf * et #)
- Utilisé pour mettre à jour `lineEditMatriculeLCD` en temps réel

**ArduinoLCD::donneesEnvoyees(QString)**
- Émis quand # est pressé
- Déclenche automatiquement `btnRechercherMatriculeLCD->click()`

---

## 🧪 Test et débogage

### Test de connexion

1. Ouvrez le **Moniteur série Arduino** (9600 bauds)
2. Vous devriez voir:
   ```
   Systeme LCD + Keypad Pret.
   Commandes :
     LCD|ligne1|ligne2
     CLEAR
     TEST
   ```

3. Testez une touche → Vous verrez:
   ```
   Touche : 5
   ```

4. Testez la validation (#):
   ```
   ENVOI: 123TN456
   ```

### Commandes de test depuis Qt

Vous pouvez envoyer des commandes manuellement:

```cpp
// Afficher du texte
arduinoLCD->afficherTexte("Ligne 1", "Ligne 2");

// Effacer l'écran
arduinoLCD->effacer();

// Test
arduinoLCD->tester();
```

### Logs Qt

Activez les logs pour voir les messages:

```
✅ Arduino LCD connecté sur COM5
🔢 Touche saisie: 1 → Matricule: 1
🔢 Touche saisie: 2 → Matricule: 12
🔢 Touche saisie: 3 → Matricule: 123
📤 Données reçues du clavier: 123TN456
🔍 Recherche automatique déclenchée pour: 123TN456
```

---

## ⚠️ Problèmes courants

### Le LCD ne s'allume pas
- Vérifiez l'alimentation 5V et GND
- Ajustez le contraste avec le potentiomètre à l'arrière du LCD
- Vérifiez l'adresse I2C (0x27 ou 0x3F)

### Les touches ne répondent pas
- Vérifiez les connexions des 8 fils du keypad
- Testez avec le moniteur série Arduino
- Vérifiez les numéros de pins dans le code

### Qt ne se connecte pas
- Fermez le moniteur série Arduino (il bloque le port)
- Vérifiez le bon port COM dans le gestionnaire de périphériques
- Redémarrez l'Arduino

### La recherche ne se lance pas
- Vérifiez que `btnRechercherMatriculeLCD` existe dans l'UI
- Vérifiez que `lineEditMatriculeLCD` existe dans l'UI
- Consultez les logs Qt pour voir les erreurs

---

## 📝 Personnalisation

### Changer la disposition du clavier

Modifiez dans `arduino_lcd_keypad.ino`:

```cpp
const char kp4x4Keys[ROWS][COLS]  = {
  {'A','B','C','D'},
  {'E','F','G','H'},
  {'I','J','K','L'},
  {'M','N','O','P'}
};
```

### Changer l'adresse I2C

```cpp
// Scanner I2C pour trouver l'adresse
#include <Wire.h>
void setup() {
  Wire.begin();
  for(byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Adresse I2C: 0x");
      Serial.println(i, HEX);
    }
  }
}
```

### Modifier le comportement de #

Dans `onDonneesKeypadEnvoyees()`:

```cpp
void MainWindow::onDonneesKeypadEnvoyees(const QString &donnees)
{
    // Ajouter un préfixe automatique
    QString matricule = "TN-" + donnees;
    ui->lineEditMatriculeLCD->setText(matricule);
    
    // Lancer la recherche
    on_btnRechercherMatriculeLCD_clicked();
}
```

---

## 🎯 Fonctionnalités avancées

### Affichage automatique sur LCD après recherche

Modifiez `on_btnRechercherMatriculeLCD_clicked()`:

```cpp
if (query.next()) {
    QString immat = query.value(0).toString();
    QString marque = query.value(1).toString();
    QString modele = query.value(2).toString();
    
    // Afficher sur le LCD
    arduinoLCD->afficherTexte(immat, marque + " " + modele);
    
    // Envoyer au LCD véhicule (existant)
    envoyerVehiculeAuLCD(immat, marque, modele, type);
}
```

### Timeout de recherche

```cpp
QTimer::singleShot(5000, this, [this]() {
    arduinoLCD->effacer();
});
```

---

## 📄 Résumé

✅ **Arduino configuré** avec LCD I2C + Keypad 4x4  
✅ **Classes Qt créées** (ArduinoLCD)  
✅ **Connexion automatique** des signaux  
✅ **Saisie temps réel** dans lineEditMatriculeLCD  
✅ **Validation avec #** → recherche automatique  
✅ **Affichage résultats** sur LCD  

---

## 🔗 Fichiers du projet

```
📁 integration/
├── 📄 arduino_lcd_keypad.ino      ← Sketch Arduino
├── 📄 arduinolcd.h                ← Classe Qt header
├── 📄 arduinolcd.cpp              ← Classe Qt implémentation
├── 📄 mainwindow.h                ← Modifié (ajout slots)
├── 📄 mainwindow.cpp              ← Modifié (connexions + implémentation)
└── 📄 projetsmartcity.pro         ← Modifié (ajout fichiers)
```

---

**Auteur**: Système Smart City - Intégration Arduino LCD  
**Date**: Décembre 2025  
**Version**: 1.0
