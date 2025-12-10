/*
 * Code Arduino pour LCD I2C - Affichage Véhicules
 * Smart City - Système d'affichage des véhicules
 * 
 * Connexions LCD I2C 16x2:
 * SDA       ----> A4 (Arduino Uno)
 * SCL       ----> A5 (Arduino Uno)
 * VCC       ----> 5V
 * GND       ----> GND
 * 
 * Bibliothèque requise: LiquidCrystal_I2C
 * Installation: Croquis > Inclure une bibliothèque > Gérer les bibliothèques
 * Rechercher "LiquidCrystal I2C" par Frank de Brabander
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialiser le LCD I2C
// Adresse I2C: 0x27 ou 0x3F (selon le modèle)
// Format: 16 colonnes x 2 lignes
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  
  // Initialiser le LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Message de bienvenue
  lcd.setCursor(0, 0);
  lcd.print("Smart City");
  lcd.setCursor(0, 1);
  lcd.print("LCD Pret!");
  
  Serial.println("========================================");
  Serial.println("LCD I2C VEHICULE - PRET");
  Serial.println("========================================");
  Serial.println("Commandes acceptees:");
  Serial.println("  LCD|ligne1|ligne2");
  Serial.println("  CLEAR (effacer ecran)");
  Serial.println("========================================");
  
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("En attente...");
}

void loop() {
  // Vérifier les commandes du PC (Qt)
  if (Serial.available() > 0) {
    String commande = Serial.readStringUntil('\n');
    commande.trim();
    
    Serial.print("Commande recue: [");
    Serial.print(commande);
    Serial.println("]");
    
    // Commande LCD: LCD|ligne1|ligne2
    if (commande.startsWith("LCD|")) {
      int premier = commande.indexOf('|');
      int deuxieme = commande.indexOf('|', premier + 1);
      
      if (deuxieme > 0) {
        String ligne1 = commande.substring(premier + 1, deuxieme);
        String ligne2 = commande.substring(deuxieme + 1);
        
        // Limiter à 16 caractères
        if (ligne1.length() > 16) ligne1 = ligne1.substring(0, 16);
        if (ligne2.length() > 16) ligne2 = ligne2.substring(0, 16);
        
        // Afficher sur LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(ligne1);
        lcd.setCursor(0, 1);
        lcd.print(ligne2);
        
        // Confirmation sur moniteur série
        Serial.println("========================================");
        Serial.println("📟 AFFICHAGE LCD");
        Serial.println("========================================");
        Serial.print("Ligne 1: ");
        Serial.println(ligne1);
        Serial.print("Ligne 2: ");
        Serial.println(ligne2);
        Serial.println("========================================");
      } else {
        Serial.println("Erreur: Format invalide. Utilisez LCD|ligne1|ligne2");
      }
    }
    // Commande CLEAR: effacer l'écran
    else if (commande == "CLEAR") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("En attente...");
      Serial.println(">>> LCD efface");
    }
    // Commande TEST: afficher un message de test
    else if (commande == "TEST") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Test LCD OK!");
      lcd.setCursor(0, 1);
      lcd.print("Smart City 2025");
      Serial.println(">>> Test LCD affiche");
    }
    else {
      Serial.println("Commande inconnue");
      Serial.println("Commandes valides:");
      Serial.println("  LCD|texte1|texte2");
      Serial.println("  CLEAR");
      Serial.println("  TEST");
    }
  }
  
  delay(100);  // Petite pause pour éviter la surcharge
}
