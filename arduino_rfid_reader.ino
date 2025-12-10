/*
 * Code Arduino pour Lecteur RFID RC522 + Servo-moteur + LCD I2C
 * Smart City - Contrôle d'accès automatique + Affichage véhicules
 * 
 * Connexions:
 * RC522 Module    Arduino Uno
 * SDA/SS    ----> Pin 10
 * SCK       ----> Pin 13
 * MOSI      ----> Pin 11
 * MISO      ----> Pin 12
 * IRQ       ----> Non connecté
 * GND       ----> GND
 * RST       ----> Pin 9
 * 3.3V      ----> 3.3V
 * 
 * Servo-moteur    Arduino Uno
 * Signal    ----> Pin 6
 * VCC       ----> 5V
 * GND       ----> GND
 * 
 * LCD I2C 16x2    Arduino Uno
 * SDA       ----> A4
 * SCL       ----> A5
 * VCC       ----> 5V
 * GND       ----> GND
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 6

// Positions du servo
#define SERVO_FERME 0      // Position initiale (porte fermée)
#define SERVO_OUVERT 90    // Position ouverte (porte ouverte)
#define DELAI_OUVERTURE 3000  // Durée d'ouverture en millisecondes (3 secondes)

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servoPorte;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adresse I2C 0x27, LCD 16x2

// Variables pour éviter les lectures répétées
String dernierRFID = "";
unsigned long dernierScanTime = 0;
const unsigned long DEBOUNCE_DELAY = 5000; // 5 secondes entre chaque scan

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
  // Initialiser le LCD I2C
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart City");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation..");
  
  // Initialiser le servo
  servoPorte.attach(SERVO_PIN);
  servoPorte.write(SERVO_FERME);  // Position initiale fermée
  
  Serial.println("Systeme RFID + Servo initialise");
  
  // Test du servo au démarrage
  Serial.println("========================================");
  Serial.println("TEST DU SERVO-MOTEUR");
  Serial.println("========================================");
  Serial.println("Rotation a 90 degres...");
  servoPorte.write(90);
  delay(1000);
  Serial.println("Rotation a 0 degre...");
  servoPorte.write(0);
  delay(1000);
  Serial.println("Test termine - Servo OK");
  Serial.println("========================================");
  
  // LCD prêt
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Pret!");
  lcd.setCursor(0, 1);
  lcd.print("En attente...");
  
  Serial.println("En attente de badge...");
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
        
        // Afficher sur LCD I2C
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(ligne1);
        lcd.setCursor(0, 1);
        lcd.print(ligne2);
        
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
    // Commande simple: 1 = ouvrir, 0 = fermer
    else if (commande == "1") {
      Serial.println(">>> Commande: OUVRIR");
      servoPorte.write(90);
      delay(15);
      Serial.println(">>> Servo a 90 degres - OUVERT");
    }
    else if (commande == "0") {
      Serial.println(">>> Commande: FERMER");
      servoPorte.write(0);
      delay(15);
      Serial.println(">>> Servo a 0 degre - FERME");
    }
    // Commande de contrôle servo: S<angle>
    // Exemple: S90 = rotation à 90°, S0 = rotation à 0°
    else if (commande.startsWith("S")) {
      String angleStr = commande.substring(1);
      int angle = angleStr.toInt();
      
      // Vérifier que l'angle est valide (0-180)
      if (angle >= 0 && angle <= 180) {
        Serial.print(">>> Rotation servo a ");
        Serial.print(angle);
        Serial.println(" degres");
        servoPorte.write(angle);
        delay(15);  // Petit délai pour que le servo commence à bouger
        Serial.println(">>> OK - Servo en mouvement");
      } else {
        Serial.println(">>> Erreur: angle invalide (0-180)");
      }
    } 
    // Ancienne commande pour compatibilité
    else if (commande == "OPEN") {
      ouvrirPorte();
    }
    // Commande CLEAR: effacer l'écran LCD
    else if (commande == "CLEAR") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("En attente...");
      Serial.println(">>> LCD efface");
    }
    // Commande TEST: afficher un message de test sur LCD
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
      Serial.println("Commandes valides: LCD|text1|text2, 1, 0, S<angle>, CLEAR, TEST");
    }
  }
  
  // Vérifier la présence d'un nouveau badge
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Vérifier si le badge peut être lu
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }
  
  // Lire l'UID du badge
  String rfidCode = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    rfidCode += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    rfidCode += String(rfid.uid.uidByte[i], HEX);
  }
  rfidCode.toUpperCase();
  
  // Vérifier si c'est le même badge scanné récemment
  unsigned long currentTime = millis();
  if (rfidCode == dernierRFID && (currentTime - dernierScanTime) < DEBOUNCE_DELAY) {
    // Ignorer ce scan (même badge trop récemment)
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }
  
  // Nouveau badge ou délai écoulé - envoyer au PC
  dernierRFID = rfidCode;
  dernierScanTime = currentTime;
  
  Serial.write(rfidCode.c_str());
  Serial.write('\n');  // Ajouter saut de ligne pour Qt
  
  // Arrêter la lecture du badge
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void ouvrirPorte() {
  Serial.println("========================================");
  Serial.println("DEBUT OUVERTURE DE LA PORTE");
  Serial.println("========================================");
  
  // Ouvrir (rotation à 90°)
  Serial.println("Rotation servo a 90 degres...");
  servoPorte.write(SERVO_OUVERT);
  Serial.println("Porte OUVERTE (90 degres)");
  
  // Attendre le délai d'ouverture
  Serial.print("Attente ");
  Serial.print(DELAI_OUVERTURE);
  Serial.println(" ms...");
  delay(DELAI_OUVERTURE);
  
  // Fermer (retour à 0°)
  Serial.println("Rotation servo a 0 degre...");
  servoPorte.write(SERVO_FERME);
  Serial.println("Porte FERMEE (0 degre)");
  Serial.println("========================================");
}
