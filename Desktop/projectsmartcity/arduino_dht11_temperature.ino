/*
 * CAPTEUR DE TEMPÉRATURE DHT11 POUR SYSTÈME D'ARROSAGE INTELLIGENT
 * ================================================================
 * 
 * Description:
 * Ce sketch Arduino lit la température et l'humidité du capteur DHT11
 * et envoie les données via le port série vers l'application Qt.
 * L'application compare la température avec celle des jardins stockés
 * en base de données et génère des alertes d'arrosage si nécessaire.
 * 
 * Connexions:
 * - DHT11 VCC -> 5V Arduino
 * - DHT11 GND -> GND Arduino  
 * - DHT11 DATA -> Pin 2 Arduino (avec résistance 10kΩ entre DATA et VCC)
 * 
 * Bibliothèque requise:
 * - DHT sensor library by Adafruit (installez via le Gestionnaire de bibliothèques)
 * 
 * Communication série:
 * - Baud rate: 9600
 * - Format: "25.30 °C\n65.00 %\n"
 * 
 * Logique d'alerte:
 * Si température ambiante (capteur) > température sol (jardin)
 * => Le jardin nécessite un arrosage
 */

#include <DHT.h>

// Configuration du capteur
#define DHT11_PIN 2
#define DHTTYPE DHT11

// Initialisation du capteur
DHT dht(DHT11_PIN, DHTTYPE);

// Variables pour stocker les mesures
float temp, hum;

// Variables pour gérer la lecture (éviter trop de lectures)
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 2000; // Lire toutes les 2 secondes

void setup() {
  // Initialiser la communication série
  Serial.begin(9600);
  
  // Initialiser le capteur DHT11
  dht.begin();
  
  // Message de démarrage
  Serial.println("========================================");
  Serial.println("CAPTEUR DHT11 - Système d'Arrosage");
  Serial.println("========================================");
  delay(2000); // Attendre que le capteur se stabilise
}

void loop() {
  unsigned long currentTime = millis();
  
  // Lire le capteur toutes les 2 secondes
  if (currentTime - lastReadTime >= READ_INTERVAL) {
    lastReadTime = currentTime;
    
    // Lire la température en Celsius
    temp = dht.readTemperature();
    
    // Lire l'humidité en %
    hum = dht.readHumidity();
    
    // Vérifier si la lecture a échoué
    if (isnan(temp) || isnan(hum)) {
      Serial.println("ERREUR: Échec de lecture du capteur DHT11");
      Serial.println("Vérifiez les connexions et la résistance pull-up");
      return;
    }
    
    // Envoyer les données au format attendu par Qt
    // Format: "température °C" puis "humidité %"
    Serial.print(temp);
    Serial.println(" °C");
    Serial.print(hum);
    Serial.println(" %");
    
    // Indicateur visuel optionnel (si LED connectée sur pin 13)
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

/*
 * NOTES D'INSTALLATION:
 * 
 * 1. Installer la bibliothèque DHT:
 *    - Ouvrir Arduino IDE
 *    - Menu: Croquis > Inclure une bibliothèque > Gérer les bibliothèques
 *    - Rechercher "DHT sensor library"
 *    - Installer "DHT sensor library by Adafruit"
 *    - Installer aussi "Adafruit Unified Sensor" (dépendance)
 * 
 * 2. Câblage du DHT11:
 *    DHT11 (4 pins)          Arduino Uno
 *    ---------------         -----------
 *    Pin 1 (VCC)    ------>  5V
 *    Pin 2 (DATA)   ------>  Digital Pin 2 (avec résistance 10kΩ vers VCC)
 *    Pin 3 (NC)     ------>  Non connecté
 *    Pin 4 (GND)    ------>  GND
 * 
 *    OU
 * 
 *    DHT11 (3 pins module)   Arduino Uno
 *    ---------------------   -----------
 *    + ou VCC       ------>  5V
 *    OUT ou DATA    ------>  Digital Pin 2 (résistance intégrée au module)
 *    - ou GND       ------>  GND
 * 
 * 3. Téléverser le sketch sur Arduino
 * 
 * 4. Connecter Arduino à un port COM différent de celui du RFID
 * 
 * 5. L'application Qt détectera automatiquement le capteur
 * 
 * DÉPANNAGE:
 * - Si lecture NaN: Vérifier câblage et résistance pull-up
 * - Si pas de données: Vérifier le bon port COM dans Qt
 * - Si valeurs aberrantes: Attendre 2-3 secondes au démarrage
 */
