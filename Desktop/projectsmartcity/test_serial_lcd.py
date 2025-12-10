#!/usr/bin/env python3
"""
Script de test pour vérifier la communication série avec Arduino LCD + Keypad
"""

import serial
import serial.tools.list_ports
import time

def lister_ports():
    """Liste tous les ports série disponibles"""
    ports = serial.tools.list_ports.comports()
    print("\n=== Ports série disponibles ===")
    for i, port in enumerate(ports):
        print(f"{i+1}. {port.device} - {port.description}")
    return ports

def tester_arduino_lcd(port_name):
    """Teste la communication avec l'Arduino LCD"""
    try:
        print(f"\n📡 Connexion à {port_name}...")
        ser = serial.Serial(port_name, 9600, timeout=1)
        time.sleep(2)  # Attendre que l'Arduino redémarre
        
        print("✅ Connexion établie")
        print("📋 En écoute des messages (Appuyez sur Ctrl+C pour arrêter)...")
        print("   Appuyez sur des touches du clavier 4x4 pour les voir s'afficher\n")
        
        while True:
            if ser.in_waiting > 0:
                ligne = ser.readline().decode('utf-8', errors='ignore').strip()
                if ligne:
                    print(f"📥 Arduino: {ligne}")
                    
                    # Détecter les touches
                    if ligne.startswith("Touche : "):
                        touche = ligne.split("Touche : ")[1]
                        print(f"   ✅ Touche détectée: '{touche}'")
                    
                    # Détecter l'envoi
                    elif ligne.startswith("ENVOI: "):
                        donnees = ligne.split("ENVOI: ")[1]
                        print(f"   ✅ Données envoyées: '{donnees}'")
                        print(f"   🔍 Ceci devrait déclencher la recherche dans Qt")
            
            time.sleep(0.1)
    
    except KeyboardInterrupt:
        print("\n\n⏹️ Test arrêté par l'utilisateur")
    except Exception as e:
        print(f"\n❌ Erreur: {e}")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("🔌 Port série fermé")

def main():
    print("=" * 60)
    print("     TEST COMMUNICATION ARDUINO LCD + KEYPAD 4x4")
    print("=" * 60)
    
    ports = lister_ports()
    
    if not ports:
        print("\n❌ Aucun port série trouvé!")
        print("   Vérifiez que l'Arduino est bien connecté en USB")
        return
    
    print("\n" + "=" * 60)
    choix = input("Entrez le numéro du port à tester (ou 'q' pour quitter): ")
    
    if choix.lower() == 'q':
        return
    
    try:
        index = int(choix) - 1
        if 0 <= index < len(ports):
            port_choisi = ports[index].device
            tester_arduino_lcd(port_choisi)
        else:
            print("❌ Numéro invalide")
    except ValueError:
        print("❌ Entrée invalide")

if __name__ == "__main__":
    main()
