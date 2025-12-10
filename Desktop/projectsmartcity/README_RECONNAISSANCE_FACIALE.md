# Reconnaissance Faciale pour Employés

## Vue d'ensemble
Ce système permet d'enregistrer et d'authentifier les employés par reconnaissance faciale.

## Configuration Initiale

### 1. Ajouter la colonne PHOTO à la base de données
Exécutez le script batch :
```
AJOUTER_COLONNE_PHOTO.bat
```

Ou exécutez manuellement dans SQL*Plus :
```sql
ALTER TABLE EMPLOYES ADD (PHOTO BLOB);
COMMIT;
```

## Utilisation

### Enregistrer le visage d'un employé

1. Ouvrez l'application Qt
2. Allez dans l'onglet **Employés**
3. Sélectionnez un employé dans le tableau
4. Cliquez sur le bouton **"Capturer Visage"**
5. Dans la fenêtre qui s'ouvre :
   - Cliquez sur **"Sélectionner une Photo"**
   - Choisissez une photo du visage de l'employé
   - Vérifiez que l'image s'affiche correctement
   - Cliquez sur **"Valider"**

La photo est maintenant enregistrée dans la base de données.

### Authentification par reconnaissance faciale

1. À l'écran de connexion
2. Cliquez sur le bouton **"Connexion par Visage"** (à ajouter dans l'UI)
3. Sélectionnez une photo du visage
4. Le système compare avec toutes les photos enregistrées
5. Si une correspondance est trouvée (>70% de similarité), l'authentification réussit

## Architecture Technique

### Classes créées

#### FacialRecognition
- **saveFaceImage()** : Enregistre l'image faciale d'un employé dans la BD et en local
- **loadFaceImage()** : Récupère l'image faciale depuis la BD
- **hasFaceImage()** : Vérifie si un employé a une photo enregistrée
- **compareFaces()** : Compare deux images et retourne un score de similarité (0.0 - 1.0)
- **authenticateByFace()** : Compare une image capturée avec toutes les photos enregistrées
- **imageToByteArray()** / **byteArrayToImage()** : Conversion pour stockage en BD

#### FaceCaptureDialog
Dialogue Qt pour :
- Sélectionner une photo depuis le système de fichiers
- Prévisualiser l'image
- Valider ou annuler la capture

### Base de données

Nouvelle colonne dans `EMPLOYES` :
```sql
PHOTO BLOB  -- Stocke l'image faciale au format JPEG
```

### Stockage
- **Base de données** : Image en BLOB (prioritaire)
- **Fichier local** : `%APPDATA%/projetsmartcity/FaceImages/employee_X.jpg` (backup)

## Algorithme de Comparaison

L'algorithme actuel utilise une comparaison simple :
1. Redimensionne les images à 100x100 pixels
2. Compare pixel par pixel
3. Calcule le pourcentage de pixels similaires
4. Seuil d'acceptation : **70%** de similarité

### Amélioration Future (OpenCV)

Pour une meilleure précision, vous pouvez intégrer OpenCV :

```cpp
// Avec OpenCV et dlib
#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/dnn.h>

// Utiliser un modèle de reconnaissance faciale
// - Haar Cascade pour détection
// - Face landmarks pour alignement
// - CNN pour extraction de features
// - Comparaison par distance euclidienne
```

## Intégration dans l'UI

### Bouton "Capturer Visage"
À connecter dans `mainwindow.cpp` :
```cpp
connect(ui->capturerVisageBtn, &QPushButton::clicked, 
        this, &MainWindow::onCapturerVisage);
```

### Bouton "Connexion par Visage"
À ajouter dans `authentification.ui` et connecter :
```cpp
connect(ui->connexionVisageBtn, &QPushButton::clicked, 
        this, &Authentification::onConnexionVisage);
```

## Sécurité

### Points forts
- Photo stockée en base de données sécurisée
- Comparaison locale (pas d'envoi à un serveur externe)
- Backup local en cas de problème BD

### Limitations actuelles
- Algorithme de comparaison basique (pas d'IA)
- Sensible à l'éclairage et l'angle
- Peut être trompé par une photo imprimée

### Recommandations
1. Utiliser OpenCV avec détection de "liveness" (vivacité)
2. Demander plusieurs angles du visage
3. Combiner avec mot de passe (authentification à 2 facteurs)
4. Chiffrer les images en base de données

## Tests

### Test de stockage
```cpp
// Dans mainwindow.cpp
void MainWindow::testFacialRecognition()
{
    QImage testImage("test_face.jpg");
    QString error;
    
    if (FacialRecognition::saveFaceImage(1, testImage, error)) {
        qDebug() << "✓ Image sauvegardée";
    } else {
        qDebug() << "✗ Erreur:" << error;
    }
    
    QImage loaded = FacialRecognition::loadFaceImage(1, error);
    if (!loaded.isNull()) {
        qDebug() << "✓ Image chargée";
    }
}
```

### Test de comparaison
```cpp
QImage img1("face1.jpg");
QImage img2("face2.jpg");
double score = FacialRecognition::compareFaces(img1, img2);
qDebug() << "Score de similarité:" << (score * 100) << "%";
```

## Dépannage

### Problème : Image non sauvegardée
- Vérifier que la colonne PHOTO existe : `DESC EMPLOYES`
- Vérifier les permissions d'écriture dans `%APPDATA%`
- Vérifier la taille de l'image (limiter à 2MB)

### Problème : Authentification échoue toujours
- Vérifier le seuil de comparaison (actuellement 70%)
- Essayer avec la même photo exacte pour tester
- Vérifier les logs de debug dans la console

### Problème : Erreur de compilation
- Vérifier que tous les fichiers sont dans le .pro :
  - facialrecognition.h/.cpp
  - facecapturedialog.h/.cpp
- Nettoyer et reconstruire le projet

## Fichiers du Système

```
facialrecognition.h           # Interface de la classe
facialrecognition.cpp         # Implémentation
facecapturedialog.h           # Dialogue de capture (header)
facecapturedialog.cpp         # Dialogue de capture (implémentation)
add_photo_column.sql          # Script SQL pour ajouter la colonne
AJOUTER_COLONNE_PHOTO.bat     # Script automatisé
README_RECONNAISSANCE_FACIALE.md  # Ce fichier
```

## Prochaines Étapes

1. **Ajouter le bouton dans l'UI** :
   - Ouvrir `mainwindow.ui` dans Qt Designer
   - Ajouter un QPushButton "Capturer Visage"
   - Le positionner près du formulaire employé
   - Connecter au slot `onCapturerVisage()`

2. **Ajouter l'authentification par visage** :
   - Ouvrir `authentification.ui` dans Qt Designer
   - Ajouter un QPushButton "Connexion par Visage"
   - Connecter au slot `onConnexionVisage()`

3. **Améliorer l'algorithme** (optionnel) :
   - Intégrer OpenCV
   - Utiliser dlib pour face landmarks
   - Implémenter détection de vivacité

4. **Tests utilisateurs** :
   - Tester avec différentes photos
   - Tester avec différents éclairages
   - Ajuster le seuil de comparaison si nécessaire
