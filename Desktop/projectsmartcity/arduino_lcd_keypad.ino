/*
 * Integration LCD I2C + Keypad 4x4
 * Smart City - 2025
 * 
 * Connexions LCD I2C:
 * SDA ----> A4 (Arduino Uno)
 * SCL ----> A5 (Arduino Uno)
 * VCC ----> 5V
 * GND ----> GND
 * 
 * Connexions Keypad 4x4:
 * Rangées: pins 9,8,7,6
 * Colonnes: pins 5,4,3,2
 * 
 * Bibliothèques requises:
 * - LiquidCrystal_I2C
 * - Keypad
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// ============================
// CONFIG LCD I2C
// ============================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ============================
// CONFIG CLAVIER 4x4
// ============================
#define ROWS 4
#define COLS 4

const char kp4x4Keys[ROWS][COLS]  = {
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
String ligne1 = "";
String ligne2 = "";
bool saisieSurLigne1 = true;



// ============================
// SETUP
// ============================
void setup() {
  Serial.begin(9600);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Smart City");
  lcd.setCursor(0,1);
  lcd.print("System Ready");

  delay(2000);
  lcd.clear();
  lcd.print("En attente...");

  Serial.println("Systeme LCD + Keypad Pret.");
  Serial.println("Commandes :");
  Serial.println("  LCD|ligne1|ligne2");
  Serial.println("  CLEAR");
  Serial.println("  TEST");
}



// ============================
// BOUCLE PRINCIPALE
// ============================
void loop() {
  gererClavier();
  gererCommandesSerie();
}



// ============================
// LECTURE DU CLAVIER
// ============================
void gererClavier() {
  char touche = kp4x4.getKey();
  
  if (touche) {
    Serial.print("Touche : ");
    Serial.println(touche);

    // Effacer si *
    if (touche == '*') {
      ligne1 = "";
      ligne2 = "";
      saisieSurLigne1 = true;
      lcd.clear();
      lcd.print("Reset clavier");
      delay(500);
      lcd.clear();
      return;
    }

    // ENVOI des données lorsque # est pressé
    if (touche == '#') {

      String data = ligne1 + ligne2;  // Nombre complet

      Serial.print("ENVOI: ");
      Serial.println(data);

      // Effacer après envoi
      ligne1 = "";
      ligne2 = "";
      saisieSurLigne1 = true;

      lcd.clear();
      lcd.print("Envoye !");
      delay(500);
      lcd.clear();
      lcd.print("En attente...");
      return;
    }

    // Sinon, écrire dans la bonne ligne
    if (saisieSurLigne1) {
      if (ligne1.length() < 16) ligne1 += touche;
    } else {
      if (ligne2.length() < 16) ligne2 += touche;
    }

    // Mise à jour LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(ligne1);
    lcd.setCursor(0,1);
    lcd.print(ligne2);
  }
}



// ============================
// GESTION DES COMMANDES PC
// ============================
void gererCommandesSerie() {
  if (!Serial.available()) return;

  String commande = Serial.readStringUntil('\n');
  commande.trim();

  Serial.print("Commande recue : ");
  Serial.println(commande);

  // Commande LCD
  if (commande.startsWith("LCD|")) {
    int p1 = commande.indexOf('|');
    int p2 = commande.indexOf('|', p1 + 1);

    if (p2 < 0) {
      Serial.println("Erreur format LCD|ligne1|ligne2");
      return;
    }

    ligne1 = commande.substring(p1 + 1, p2);
    ligne2 = commande.substring(p2 + 1);

    if (ligne1.length() > 16) ligne1 = ligne1.substring(0, 16);
    if (ligne2.length() > 16) ligne2 = ligne2.substring(0, 16);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(ligne1);
    lcd.setCursor(0,1);
    lcd.print(ligne2);

    Serial.println("Affichage mis à jour.");
  }

  // CLEAR
  else if (commande == "CLEAR") {
    ligne1 = "";
    ligne2 = "";
    saisieSurLigne1 = true;
    lcd.clear();
    lcd.print("En attente...");
  }

  // TEST
  else if (commande == "TEST") {
    lcd.clear();
    lcd.print("Test LCD OK!");
    lcd.setCursor(0,1);
    lcd.print("Smart City 2025");
  }

  else {
    Serial.println("Commande inconnue.");
  }
}
