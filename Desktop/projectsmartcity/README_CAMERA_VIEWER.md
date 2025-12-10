# Visualiseur Caméra OV7670 en Temps Réel

## Installation

Installez les dépendances Python nécessaires:

```bash
pip install pyserial opencv-python numpy
```

## Utilisation

### 1. Démarrer le visualiseur

```bash
python camera_viewer.py
```

### 2. Configuration

Si votre Arduino est sur un port différent de COM9, modifiez la ligne 10 dans `camera_viewer.py`:

```python
PORT = 'COM9'  # Changez selon votre port
```

### 3. Contrôles

- **`q`** : Quitter le programme
- **`s`** : Sauvegarder une capture (capture_000.jpg, capture_001.jpg, etc.)

## Fonctionnalités

✅ **Flux vidéo en temps réel** - Affiche les images de la caméra dès leur réception
✅ **Conversion YUV422→RGB** - Conversion automatique du format caméra
✅ **Agrandissement 2x** - L'image 320x240 est affichée en 640x480 pour meilleure visibilité
✅ **Sauvegarde de captures** - Enregistrez des images avec la touche 's'

## Résolution des problèmes

### Erreur: "Permission denied" ou "Port already in use"

Le port est déjà utilisé par l'application Qt. **Fermez l'application Qt** avant de lancer ce script Python.

### Erreur: "No module named 'cv2'"

```bash
pip install opencv-python
```

### Erreur: "No module named 'serial'"

```bash
pip install pyserial
```

### L'image ne s'affiche pas

- Vérifiez que l'Arduino envoie bien le marqueur `*RDY*`
- Vérifiez le baud rate (doit être 2000000)
- Attendez quelques secondes, l'image prend du temps à se transférer

## Performance

- **Résolution**: 320x240 pixels (QVGA)
- **Taille image**: 153,600 bytes (YUV422)
- **Vitesse transfert**: ~0.6 secondes à 2 Mbaud
- **FPS attendu**: ~1-2 images par seconde (dépend du code Arduino)

## Format de données

L'Arduino envoie:
1. Marqueur: `*RDY*` (5 bytes)
2. Données image: 153,600 bytes en YUV422
3. Répète...

Format YUV422: `Y0 U Y1 V Y0 U Y1 V ...` (2 bytes par pixel)
