/*
 * Code Arduino pour LCD I2C + Keypad 4x4 - Affichage Véhicules
 * Smart City - Système d'affichage des véhicules
 * 
 * Connexions LCD I2C 16x2:
 * SDA       ----> A4 (Arduino Uno)
 * SCL       ----> A5 (Arduino Uno)
 * VCC       ----> 5V
 * GND       ----> GND
 * 
 * Connexions Keypad 4x4:
 * Rangées: pins 9,8,7,6
 * Colonnes: pins 5,4,3,2
 * 
 * Bibliothèques requises:
 * - LiquidCrystal_I2C (par Frank de Brabander)
 * - Keypad (par Mark Stanley)
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Initialiser le LCD I2C
// Adresse I2C: 0x27 ou 0x3F (selon le modèle)
// Format: 16 colonnes x 2 lignes
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ============================
// CONFIG CLAVIER 4x4
// ============================
#define ROWS 4
#define COLS 4

const char kp4x4Keys[ROWS][COLS] = {
  {'1','2','3','T'},
  {'4','5','6','N'},
  {'7','8','9','-'},
  {'*','0','#','D'}
};

byte rowKp4x4Pin[4] = {9,8,7,6};
byte colKp4x4Pin[4] = {5,4,3,2};

Keypad kp4x4 = Keypad(makeKeymap(kp4x4Keys), rowKp4x4Pin, colKp4x4Pin, ROWS, COLS);

// ============================
// VARIABLES
// ============================
String matriculeSaisie = "";

void setup() {
  Serial.begin(9600);
  
  // Attendre que le port série soit prêt
  while (!Serial) {
    ; // Attendre la connexion du port série
  }
  delay(100);  // Petit délai supplémentaire pour stabilisation
  
  // Vider le buffer série
  while (Serial.available()) {
    Serial.read();
  }
  
  // Initialiser le LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Message de bienvenue
  lcd.setCursor(0, 0);
  lcd.print("Smart City");
  lcd.setCursor(0, 1);
  lcd.print("LCD + Keypad OK!");
  
  Serial.println("========================================");
  Serial.println("Systeme LCD + Keypad Pret.");
  Serial.println("========================================");
  Serial.println("Commandes acceptees:");
  Serial.println("  LCD|ligne1|ligne2");
  Serial.println("  CLEAR (effacer ecran)");
  Serial.println("========================================");
  Serial.println("Keypad: # = Soumettre, * = Effacer");
  Serial.println("========================================");
  
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Matricule:");
  lcd.setCursor(0, 1);
  lcd.print("_");
}

void loop() {
  // Gérer le clavier
  gererClavier();
  
  // Vérifier les commandes du PC (Qt)
  gererCommandesSerie();
}

// ============================
// GESTION DU CLAVIER
// ============================
void gererClavier() {
  char touche = kp4x4.getKey();
  
  if (touche) {
    // Envoyer la touche au PC
    Serial.print("Touche : ");
    Serial.println(touche);
    Serial.flush();  // S'assurer que tout est envoyé

    // * = Effacer la saisie
    if (touche == '*') {
      matriculeSaisie = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Matricule:");
      lcd.setCursor(0, 1);
      lcd.print("_");
      return;
    }

    // # = Soumettre la matricule pour vérification
    if (touche == '#') {
      if (matriculeSaisie.length() > 0) {
        Serial.print("ENVOI: ");
        Serial.println(matriculeSaisie);
        Serial.flush();  // S'assurer que tout est envoyé
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Verification...");
        lcd.setCursor(0, 1);
        lcd.print(matriculeSaisie);
        
        // Réinitialiser après envoi
        matriculeSaisie = "";
        
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Matricule:");
        lcd.setCursor(0, 1);
        lcd.print("_");
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Erreur:");
        lcd.setCursor(0, 1);
        lcd.print("Saisir matricule");
        delay(1500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Matricule:");
        lcd.setCursor(0, 1);
        lcd.print("_");
      }
      return;
    }

    // Ajouter le caractère à la matricule (max 16 caractères)
    if (matriculeSaisie.length() < 16) {
      matriculeSaisie += touche;
      
      // Mettre à jour l'affichage
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Matricule:");
      lcd.setCursor(0, 1);
      lcd.print(matriculeSaisie);
      lcd.print("_");
    }
  }
}

// ============================
// GESTION DES COMMANDES SERIE
// ============================
void gererCommandesSerie() {
  if (!Serial.available()) return;
  
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
      Serial.println("LCD AFFICHAGE");
      Serial.println("========================================");
      Serial.print("Ligne 1: ");
      Serial.println(ligne1);
      Serial.print("Ligne 2: ");
      Serial.println(ligne2);
      Serial.println("========================================");
      
      // Après 5 secondes, revenir à l'écran de saisie
      delay(5000);
      matriculeSaisie = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Matricule:");
      lcd.setCursor(0, 1);
      lcd.print("_");
    } else {
      Serial.println("Erreur: Format invalide. Utilisez LCD|ligne1|ligne2");
    }
  }
  // Commande CLEAR: effacer l'écran
  else if (commande == "CLEAR") {
    matriculeSaisie = "";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Matricule:");
    lcd.setCursor(0, 1);
    lcd.print("_");
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
    
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Matricule:");
    lcd.setCursor(0, 1);
    lcd.print("_");
  }
  else {
    Serial.println("Commande inconnue");
    Serial.println("Commandes valides:");
    Serial.println("  LCD|texte1|texte2");
    Serial.println("  CLEAR");
    Serial.println("  TEST");
  }
}
