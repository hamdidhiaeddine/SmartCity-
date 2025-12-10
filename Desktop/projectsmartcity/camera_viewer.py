"""
Visualiseur en temps réel pour caméra OV7670 Arduino
Reçoit les images YUV422 du port série et les affiche avec OpenCV
"""

import serial
import numpy as np
import cv2
import sys

# Configuration
PORT = 'COM9'  # Port Arduino
BAUD_RATE = 2000000  # 2 Mbaud
WIDTH = 320
HEIGHT = 240
IMAGE_SIZE = WIDTH * HEIGHT * 2  # YUV422 = 2 bytes par pixel

def yuv422_to_rgb(yuv_data):
    """Convertit les données YUV422 en RGB"""
    rgb_image = np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8)
    
    for y in range(HEIGHT):
        for x in range(0, WIDTH, 2):
            idx = (y * WIDTH + x) * 2
            if idx + 3 >= len(yuv_data):
                break
            
            y0 = yuv_data[idx]
            u = yuv_data[idx + 1]
            y1 = yuv_data[idx + 2]
            v = yuv_data[idx + 3]
            
            # Convertir YUV vers RGB pour les deux pixels
            for i, y_val in enumerate([y0, y1]):
                c = y_val - 16
                d = u - 128
                e = v - 128
                
                r = np.clip((298 * c + 409 * e + 128) >> 8, 0, 255)
                g = np.clip((298 * c - 100 * d - 208 * e + 128) >> 8, 0, 255)
                b = np.clip((298 * c + 516 * d + 128) >> 8, 0, 255)
                
                if x + i < WIDTH:
                    rgb_image[y, x + i] = [b, g, r]  # OpenCV utilise BGR
    
    return rgb_image

def main():
    print("🎥 Démarrage du visualiseur caméra OV7670...")
    print(f"📡 Connexion au port {PORT} à {BAUD_RATE} bauds...")
    
    try:
        # Ouvrir le port série
        ser = serial.Serial(PORT, BAUD_RATE, timeout=2)
        print("✅ Port série ouvert!")
        print("\n⌨️  Appuyez sur 'q' pour quitter")
        print("⌨️  Appuyez sur 's' pour sauvegarder une capture")
        print("⌨️  Appuyez sur 'd' pour voir les données brutes (debug)\n")
        
        capture_count = 0
        image_buffer = bytearray()
        is_capturing = False
        debug_mode = False
        frames_received = 0
        
        while True:
            # Lire les données disponibles
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                
                # Chercher le marqueur *RDY*
                if not is_capturing:
                    # Chercher le marqueur dans les données brutes
                    marker_idx = data.find(b'*RDY*')
                    if marker_idx >= 0:
                        is_capturing = True
                        image_buffer.clear()
                        print(f"\n📷 Frame #{frames_received + 1} - Réception en cours...", end='')
                        
                        # Supprimer tout avant et y compris le marqueur
                        data = data[marker_idx + 5:]
                        
                        if debug_mode:
                            print(f"\n   Marqueur trouvé à l'index {marker_idx}")
                            print(f"   Données après marqueur: {len(data)} bytes")
                
                # Accumuler les données de l'image
                if is_capturing:
                    image_buffer.extend(data)
                    
                    # Afficher progression
                    if len(image_buffer) % 10000 == 0:
                        progress = (len(image_buffer) / IMAGE_SIZE) * 100
                        print(f"\r📷 Frame #{frames_received + 1} - {progress:.1f}%", end='')
                    
                    # Vérifier si l'image est complète
                    if len(image_buffer) >= IMAGE_SIZE:
                        is_capturing = False
                        frames_received += 1
                        
                        # Convertir YUV422 en RGB
                        yuv_data = image_buffer[:IMAGE_SIZE]
                        rgb_image = yuv422_to_rgb(yuv_data)
                        
                        # Agrandir l'image pour meilleure visibilité (2x)
                        display_image = cv2.resize(rgb_image, (WIDTH * 2, HEIGHT * 2), 
                                                   interpolation=cv2.INTER_LINEAR)
                        
                        # Ajouter informations sur l'image
                        info_text = f"Frame {frames_received} | {len(yuv_data)} bytes | Press 's' to save"
                        cv2.putText(display_image, info_text, (10, 20), 
                                   cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)
                        
                        # Afficher l'image
                        cv2.imshow('Camera OV7670 - Real Time', display_image)
                        
                        print(f"\r✅ Frame #{frames_received} reçue: {len(yuv_data)} bytes     ")
                        
                        if debug_mode:
                            print(f"   Premiers bytes YUV: {list(yuv_data[:20])}")
                            print(f"   Min: {min(yuv_data)}, Max: {max(yuv_data)}, Moyenne: {sum(yuv_data)/len(yuv_data):.1f}")
                        
                        # Vider le buffer
                        image_buffer.clear()
            
            # Gérer les événements clavier
            key = cv2.waitKey(1) & 0xFF
            
            if key == ord('q'):
                print("\n🛑 Arrêt du programme...")
                break
            elif key == ord('s'):
                # Sauvegarder la capture
                if 'rgb_image' in locals():
                    filename = f"capture_{capture_count:03d}.jpg"
                    cv2.imwrite(filename, rgb_image)
                    print(f"\n💾 Image sauvegardée: {filename}")
                    capture_count += 1
            elif key == ord('d'):
                # Basculer mode debug
                debug_mode = not debug_mode
                print(f"\n🔧 Mode debug: {'ON' if debug_mode else 'OFF'}")
    
    except serial.SerialException as e:
        print(f"\n❌ Erreur port série: {e}")
        print(f"\nVérifiez que:")
        print(f"  • L'Arduino est branché sur {PORT}")
        print(f"  • Le port n'est pas utilisé par une autre application")
        print(f"  • Vous avez les permissions nécessaires")
        return 1
    
    except KeyboardInterrupt:
        print("\n\n⚠️  Interruption utilisateur (Ctrl+C)")
    
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("📡 Port série fermé")
        
        cv2.destroyAllWindows()
        print("👋 Programme terminé")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
