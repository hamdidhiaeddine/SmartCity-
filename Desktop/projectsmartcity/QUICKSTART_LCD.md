# Quick Start - Arduino LCD + Keypad

## Installation rapide

### 1. Bibliothèques Arduino
```
IDE Arduino → Gérer les bibliothèques:
- LiquidCrystal_I2C
- Keypad
```

### 2. Connexions
```
LCD I2C:  SDA→A4, SCL→A5, VCC→5V, GND→GND
Keypad:   Rangées→9,8,7,6  Colonnes→5,4,3,2
```

### 3. Upload Arduino
```
Fichier: arduino_lcd_keypad.ino
Port: COM3 (ou votre port)
Téléverser (Ctrl+U)
```

### 4. Recompiler Qt
```powershell
cd C:\Users\walae\Desktop\qt_smartcity\integration
qmake
make
```

## Utilisation

### Dans l'application Qt:
1. Cliquer **"Connecter LCD"**
2. Saisir matricule sur keypad: `1 2 3 T N 4 5 6`
3. Appuyer `#` → Recherche automatique

### Touches spéciales:
- `*` = Effacer
- `#` = Valider et rechercher

## Test rapide

### Moniteur série Arduino (9600):
```
Touche : 1
Touche : 2
ENVOI: 123TN456
```

### Qt Debug:
```
✅ Arduino LCD connecté sur COM5
🔢 Touche saisie: 1 → Matricule: 1
🔍 Recherche automatique déclenchée pour: 123TN456
```

## Dépannage

| Problème | Solution |
|----------|----------|
| LCD noir | Ajuster le potentiomètre à l'arrière |
| Port occupé | Fermer moniteur série Arduino |
| Keypad ne répond pas | Vérifier connexions 8 fils |
| Qt ne trouve pas | Installer module serialport |

## Fichiers créés

```
✅ arduino_lcd_keypad.ino   - Sketch Arduino
✅ arduinolcd.h             - Classe Qt
✅ arduinolcd.cpp           - Implémentation
✅ mainwindow.h/cpp         - Intégration
✅ projetsmartcity.pro      - Configuration
✅ GUIDE_LCD_KEYPAD.md      - Documentation complète
```

Pour plus de détails, voir: **GUIDE_LCD_KEYPAD.md**
