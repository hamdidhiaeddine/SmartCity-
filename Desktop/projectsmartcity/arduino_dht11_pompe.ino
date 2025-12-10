/*
 * Code Arduino pour DHT11 + Pompe à Eau
 * Smart City - Système d'arrosage automatique des jardins
 * 
 * Connexions DHT11:
 * VCC       ----> 5V
 * DATA      ----> Pin 2 (avec résistance 10K vers VCC)
 * GND       ----> GND
 * 
 * Connexions Pompe (via relais):
 * Signal    ----> Pin 7
 * VCC       ----> 5V
 * GND       ----> GND
 * 
 * Bibliothèque requise: DHT sensor library
 * Installation: Croquis > Inclure une bibliothèque > Gérer les bibliothèques
 * Rechercher "DHT sensor library" par Adafruit
 */

#include "DHT.h"

// Configuration des pins
#define DHTPIN 2        // Pin DATA du capteur DHT11
#define DHTTYPE DHT11   // Type de capteur
#define POMPE_PIN 7     // Pin de commande de la pompe (relais)

// Initialiser le capteur DHT11
DHT dht(DHTPIN, DHTTYPE);

// Variables globales
float temperature = 0;
float humidite = 0;
bool pompeActive = false;
unsigned long dernierEnvoi = 0;
const unsigned long INTERVALLE_ENVOI = 2000;  // Envoyer données toutes les 2 secondes

void setup() {
  // Initialiser la communication série
  Serial.begin(9600);
  
  // Initialiser le capteur DHT11
  dht.begin();
  
  // Configurer le pin de la pompe en sortie
  pinMode(POMPE_PIN, OUTPUT);
  digitalWrite(POMPE_PIN, LOW);  // Pompe arrêtée au démarrage
  
  // Message de démarrage
  Serial.println("========================================");
  Serial.println("DHT11 + POMPE - SMART CITY");
  Serial.println("========================================");
  Serial.println("Commandes acceptees:");
  Serial.println("  1 = Pompe MARCHE");
  Serial.println("  0 = Pompe ARRET");
  Serial.println("  STATUS = Etat actuel");
  Serial.println("========================================");
  
  delay(2000);  // Attendre que le capteur soit prêt
}

void loop() {
  // Vérifier les commandes reçues du PC (Qt)
  if (Serial.available() > 0) {
    String commande = Serial.readStringUntil('\n');
    commande.trim();
    
    // Commande '1' : Activer la pompe
    if (commande == "1" || commande == "POMPE_ON") {
      digitalWrite(POMPE_PIN, HIGH);
      pompeActive = true;
      Serial.println("POMPE:MARCHE");
      Serial.println(">>> Pompe activee!");
    }
    // Commande '0' : Arrêter la pompe
    else if (commande == "0" || commande == "POMPE_OFF") {
      digitalWrite(POMPE_PIN, LOW);
      pompeActive = false;
      Serial.println("POMPE:ARRET");
      Serial.println(">>> Pompe arretee!");
    }
    // Commande STATUS : Afficher l'état
    else if (commande == "STATUS") {
      Serial.println("========================================");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");
      Serial.print("Humidite: ");
      Serial.print(humidite);
      Serial.println(" %");
      Serial.print("Pompe: ");
      Serial.println(pompeActive ? "MARCHE" : "ARRET");
      Serial.println("========================================");
    }
  }
  
  // Lire et envoyer les données du capteur périodiquement
  unsigned long maintenant = millis();
  if (maintenant - dernierEnvoi >= INTERVALLE_ENVOI) {
    dernierEnvoi = maintenant;
    
    // Lire les données du capteur DHT11
    float nouvTemperature = dht.readTemperature();
    float nouvHumidite = dht.readHumidity();
    
    // Vérifier si les lectures sont valides
    if (!isnan(nouvTemperature) && !isnan(nouvHumidite)) {
      temperature = nouvTemperature;
      humidite = nouvHumidite;
      
      // Envoyer les données au format attendu par Qt
      // Format: "Temperature = XX.XX °C" et "Humidite = XX.XX %"
      Serial.print("Temperature = ");
      Serial.print(temperature, 2);
      Serial.println(" °C");
      
      Serial.print("Humidite = ");
      Serial.print(humidite, 2);
      Serial.println(" %");
      
      // Afficher aussi l'état de la pompe
      Serial.print("Pompe = ");
      Serial.println(pompeActive ? "1" : "0");
    } else {
      Serial.println("Erreur lecture capteur DHT11!");
    }
  }
  
  delay(100);  // Petite pause pour éviter la surcharge
}
