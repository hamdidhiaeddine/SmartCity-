/*
 * Code Arduino pour Lecteur RFID RC522 + Servo-moteur
 * Smart City - Contrôle d'accès automatique
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
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 6

// Positions du servo
#define SERVO_FERME 0      // Position initiale (porte fermée)
#define SERVO_OUVERT 90    // Position ouverte (porte ouverte)
#define DELAI_OUVERTURE 3000  // Durée d'ouverture en millisecondes (3 secondes)

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servoPorte;

// Variables pour éviter les lectures répétées
String dernierRFID = "";
unsigned long dernierScanTime = 0;
const unsigned long DEBOUNCE_DELAY = 5000; // 5 secondes entre chaque scan

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
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
    
    // Commande simple: 1 = ouvrir, 0 = fermer
    if (commande == "1") {
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
    } else {
      Serial.println("Commande inconnue");
      Serial.println("Commandes valides: S<angle> (ex: S90, S0) ou OPEN");
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
