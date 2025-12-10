#!/usr/bin/env python3
"""
Script de test pour le keypad Arduino
Fermez Arduino IDE et Qt avant de lancer ce script!
"""

import serial
import time

PORT = "COM10"  # Changez si nécessaire
BAUD = 9600

print(f"=== Test Keypad Arduino sur {PORT} ===")
print("Fermez Arduino IDE et Qt avant de continuer!")
print()

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print(f"✅ Connecté sur {PORT}")
    print("Tapez sur le keypad pour voir les touches...")
    print("Appuyez sur Ctrl+C pour quitter")
    print()
    
    time.sleep(2)  # Attendre l'initialisation Arduino
    
    while True:
        if ser.in_waiting > 0:
            data = ser.readline()
            try:
                ligne = data.decode('latin-1').strip()
                if ligne:
                    print(f"📥 Reçu: [{ligne}]")
                    
                    if "Touche" in ligne:
                        print(f"   ➡️ TOUCHE DÉTECTÉE!")
                    if "ENVOI" in ligne:
                        print(f"   ➡️ ENVOI DÉTECTÉ!")
            except:
                print(f"📥 Brut: {data.hex()} = {data}")
                
except serial.SerialException as e:
    print(f"❌ Erreur: {e}")
    print()
    print("Vérifiez que:")
    print("  1. Arduino IDE est fermé")
    print("  2. Qt est fermé")
    print("  3. Le port COM10 est correct")
except KeyboardInterrupt:
    print("\n👋 Arrêt")
finally:
    try:
        ser.close()
    except:
        pass
