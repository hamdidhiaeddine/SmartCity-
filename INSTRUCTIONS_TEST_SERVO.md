# Instructions pour tester le servo avec Qt

## ⚠️ IMPORTANT: Un seul programme à la fois peut utiliser le port série!

### Étapes de test:

1. **FERMER Arduino Serial Monitor** (très important!)
   - Si le Serial Monitor est ouvert, Qt ne peut pas communiquer avec l'Arduino
   - Fermez complètement l'IDE Arduino

2. **Vérifier dans Qt Creator:**
   - Ouvrir le panneau "Application Output" (en bas)
   - Lancer l'application Qt

3. **Ce que vous devriez voir au démarrage:**
   ```
   📡 Ports série disponibles:
   🔌 Tentative de connexion sur: COM3 (ou COM4, COM5...)
   ✅ Arduino connecté avec succès sur COM3
   ```

4. **Tester le bouton "🔧 Test Servo":**
   - Cliquer sur le bouton
   - Vérifier les logs dans "Application Output":
   ```
   ========================================
   🚪 OUVERTURE DE LA PORTE
   ========================================
   📤 Envoi commande: 1 (OUVRIR)
   ✅ Commande envoyée: 2 bytes
   🔄 Servo → 90° (porte ouverte)
   🔄 Fermeture de la porte...
   ✅ Servo → 0° (porte fermée)
   ========================================
   ```

5. **Le servo devrait bouger:**
   - Rotation à 90° pendant 3 secondes
   - Retour à 0°

## 🔍 Si ça ne marche toujours pas:

### Vérifier le port COM:
```powershell
# Dans PowerShell, exécuter:
Get-WmiObject Win32_SerialPort | Select-Object Name, DeviceID, Description
```

### Ajouter des logs supplémentaires:
Si vous ne voyez rien dans les logs Qt, le problème est la connexion au port série.

### Vérifier l'alimentation:
- Si le servo bouge avec Serial Monitor mais pas avec Qt, c'est un problème logiciel
- Si le servo ne bouge jamais, vérifier l'alimentation (USB peut ne pas suffire)

## 📝 Résumé des commandes:
- **Qt envoie "1\n"** → Arduino reçoit "1" → Servo à 90°
- **Qt envoie "0\n"** → Arduino reçoit "0" → Servo à 0°

## ✅ Test réussi si:
- Logs Qt affichent "Commande envoyée: 2 bytes"
- Servo tourne à 90° puis revient à 0° après 3 secondes
