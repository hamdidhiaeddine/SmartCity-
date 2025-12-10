# 📟 Affichage des Véhicules sur LCD Arduino

## 🎯 Fonctionnalité

Cette fonctionnalité permet de **rechercher un véhicule par sa matricule** et d'afficher ses informations sur un écran LCD connecté à l'Arduino.

## 🖥️ Interface Utilisateur

Dans la page **Gestion des Véhicules**, vous trouverez :

1. **Champ de saisie** : `lineEditMatriculeLCD`
   - Placeholder: "Entrez une matricule..."
   - Position: En bas du formulaire
   
2. **Bouton** : `📟 Afficher sur LCD`
   - Couleur: Bleu clair (#17a2b8)
   - Action: Recherche et envoi au LCD

## 📋 Utilisation

### Étape 1 : Entrer la Matricule
- Tapez la matricule du véhicule dans le champ de recherche
- Exemple: `ABC123`, `TN-1234-TN`

### Étape 2 : Cliquer sur "Afficher sur LCD"
- Le système recherche dans la base de données
- Si trouvé : envoi des infos à l'Arduino
- Si non trouvé : message d'erreur

### Étape 3 : Affichage
Le LCD affiche sur 2 lignes (16 caractères max) :
```
Ligne 1: Mat:ABC123
Ligne 2: TOYOTA Corolla
```

## 🔧 Protocole de Communication

### Format de la commande
```
LCD|ligne1|ligne2\n
```

### Exemple réel
```
LCD|Mat:ABC123|TOYOTA Corolla\n
```

## 🎛️ Configuration Arduino

### Code Arduino
Le fichier `arduino_rfid_reader.ino` gère maintenant 3 types de commandes :

1. **Servo** : `1` (ouvrir) ou `0` (fermer)
2. **Angle** : `S90` (rotation à 90°)
3. **LCD** : `LCD|texte1|texte2` ✨ **NOUVEAU**

### Traitement côté Arduino
```cpp
if (commande.startsWith("LCD|")) {
  int premier = commande.indexOf('|');
  int deuxieme = commande.indexOf('|', premier + 1);
  
  String ligne1 = commande.substring(premier + 1, deuxieme);
  String ligne2 = commande.substring(deuxieme + 1);
  
  // Affichage sur moniteur série
  Serial.println("📟 AFFICHAGE LCD");
  Serial.println(ligne1);
  Serial.println(ligne2);
}
```

## 🔌 Matériel Nécessaire (Optionnel)

Pour afficher réellement sur un LCD physique :

### LCD I2C 16x2
- **Connexions** :
  - SDA → A4 (Arduino Uno)
  - SCL → A5 (Arduino Uno)
  - VCC → 5V
  - GND → GND

### Code à ajouter dans Arduino

```cpp
#include <LiquidCrystal_I2C.h>

// Initialisation LCD I2C (adresse 0x27 ou 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // ... code existant ...
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // Dans la partie LCD|
  if (commande.startsWith("LCD|")) {
    // ... code existant ...
    
    // Affichage réel sur LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ligne1);
    lcd.setCursor(0, 1);
    lcd.print(ligne2);
  }
}
```

## 🗄️ Base de Données

### Requête SQL utilisée
```sql
SELECT IMMAT, MARQUE, MODELE, TYPE 
FROM GEST_VEHICULE 
WHERE UPPER(IMMAT) = :immat
```

### Table GEST_VEHICULE
- `IMMAT` : Matricule (clé primaire)
- `MARQUE` : Constructeur
- `MODELE` : Modèle du véhicule
- `TYPE` : Type (Voiture, Camion, etc.)

## ✅ Validation et Tests

### Test 1 : Matricule Existante
```
Input: ABC123
Output LCD:
  Mat:ABC123
  TOYOTA Corolla
Message Qt: "Informations envoyées au LCD"
```

### Test 2 : Matricule Inexistante
```
Input: XXXYYY
Output: "Aucun véhicule avec la matricule: XXXYYY"
```

### Test 3 : Arduino Déconnecté
```
Output: "Arduino non connecté !"
```

### Test 4 : Champ Vide
```
Output: "Veuillez entrer une matricule !"
```

## 🐛 Débogage

### Moniteur Série Arduino
Vous verrez :
```
Commande recue: [LCD|Mat:ABC123|TOYOTA Corolla]
========================================
📟 AFFICHAGE LCD
========================================
Ligne 1: Mat:ABC123
Ligne 2: TOYOTA Corolla
========================================
```

### Console Qt (qDebug)
```
📟 Envoi au LCD: LCD|Mat:ABC123|TOYOTA Corolla\n
```

## 📝 Limitations

- **16 caractères max** par ligne (LCD 16x2)
- **Texte tronqué** automatiquement si trop long
- **Majuscules** automatiques pour la recherche
- **Arduino doit être connecté**

## 🚀 Améliorations Futures

1. ✨ Affichage de la date de maintenance
2. ✨ État du véhicule (Neuf/Usé/En panne)
3. ✨ Couleur LED selon l'état
4. ✨ Animation de défilement pour texte long
5. ✨ Historique des affichages

## 📚 Fichiers Modifiés

1. `mainwindow.h` - Ajout du slot `on_btnRechercherMatriculeLCD_clicked()`
2. `mainwindow.cpp` - Implémentation de la recherche et envoi
3. `mainwindow.ui` - Ajout du champ `lineEditMatriculeLCD` et bouton
4. `arduino_rfid_reader.ino` - Gestion du protocole `LCD|`

---

**Auteur** : Smart City Integration Team  
**Date** : Décembre 2025  
**Version** : 1.0
