/*
 * Arduino RFID + Servo Motor pour Smart City
 * 
 * Matériel requis:
 * - Arduino Uno
 * - Module RFID RC522 (SPI)
 * - Servo moteur SG90 ou compatible
 * 
 * Connexions RFID RC522:
 * SDA  -> Pin 10
 * SCK  -> Pin 13
 * MOSI -> Pin 11
 * MISO -> Pin 12
 * IRQ  -> Non connecté
 * GND  -> GND
 * RST  -> Pin 9
 * 3.3V -> 3.3V
 * 
 * Connexion Servo:
 * Signal -> Pin 6
 * VCC    -> 5V
 * GND    -> GND
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Pins RFID
#define SS_PIN 10
#define RST_PIN 9

// Pin Servo
#define SERVO_PIN 6

// Positions du servo
#define SERVO_FERME 0      // Position initiale (porte fermée)
#define SERVO_OUVERT 90    // Position ouverte (porte ouverte)
#define DELAI_OUVERTURE 3000  // Durée d'ouverture en millisecondes (3 secondes)

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servoPorte;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Initialiser le servo
  servoPorte.attach(SERVO_PIN);
  servoPorte.write(SERVO_FERME);  // Position initiale fermée
  
  Serial.println("Systeme RFID + Servo pret");
  Serial.println("En attente de carte RFID...");
}

void loop() {
  // Vérifier les commandes du PC (Qt)
  if (Serial.available() > 0) {
    String commande = Serial.readStringUntil('\n');
    commande.trim();
    
    if (commande == "OPEN") {
      ouvrirPorte();
      return; // Éviter la détection RFID pendant l'ouverture
    } else if (commande == "CLOSE") {
      servoPorte.write(SERVO_FERME);
      Serial.println("Porte FERMEE manuellement");
      return;
    } else if (commande == "TEST") {
      // Test du servo
      Serial.println("Test servo...");
      servoPorte.write(SERVO_OUVERT);
      delay(2000);
      servoPorte.write(SERVO_FERME);
      Serial.println("Test termine");
      return;
    }
  }
  
  // Détecter les cartes RFID
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Lire l'UID de la carte
  String rfidCode = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      rfidCode += "0";
    }
    rfidCode += String(mfrc522.uid.uidByte[i], HEX);
  }
  rfidCode.toUpperCase();
  
  // Envoyer le code RFID au PC avec marqueur
  Serial.print("RFID:");
  Serial.println(rfidCode);
  
  // Ouvrir automatiquement la porte quand un RFID est détecté
  ouvrirPorte();
  
  // Arrêter la communication avec la carte
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  delay(1000); // Anti-rebond
}

void ouvrirPorte() {
  Serial.println("SERVO:OPENING");
  
  // Ouvrir progressivement (rotation à 90°)
  for (int pos = SERVO_FERME; pos <= SERVO_OUVERT; pos++) {
    servoPorte.write(pos);
    delay(15); // Mouvement fluide
  }
  Serial.println("SERVO:OPENED");
  
  // Attendre le délai d'ouverture
  delay(DELAI_OUVERTURE);
  
  // Fermer progressivement (retour à 0°)
  Serial.println("SERVO:CLOSING");
  for (int pos = SERVO_OUVERT; pos >= SERVO_FERME; pos--) {
    servoPorte.write(pos);
    delay(15); // Mouvement fluide
  }
  Serial.println("SERVO:CLOSED");
}
