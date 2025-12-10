# Guide d'intégration de la fonction Vidéo pour les Jardins

## Intégration réussie ! ✅

La fonction vidéo du projet SmartCity--gestion_resident a été intégrée dans votre projet principal.

## Fichiers ajoutés

1. **videodialog.h** - Header pour le dialogue vidéo
2. **videodialog.cpp** - Implémentation du lecteur vidéo avec QMediaPlayer
3. **metier.h** - Header pour la gestion des médias et cartes de visite
4. **metier.cpp** - Implémentation des fonctions métier (vidéos + cartes)
5. **path.txt** - Fichier de configuration des chemins vidéo

## Modifications apportées

### mainwindow.h
- Ajout de `#include "videodialog.h"`
- Ajout de la méthode `void onOuvrirVideoDialogJardin();`

### mainwindow.cpp
- Connexion du bouton `btnVideoJardin` dans `connectButtons()`
- Implémentation de `onOuvrirVideoDialogJardin()` qui ouvre le dialogue vidéo

### projetsmartcity.pro
- Ajout de `videodialog.cpp` et `metier.cpp` dans SOURCES
- Ajout de `videodialog.h` et `metier.h` dans HEADERS

## Comment utiliser

### 1. Ajouter un bouton dans l'interface Qt Designer

Ouvrez `mainwindow.ui` dans Qt Designer et ajoutez un bouton :
- **Nom de l'objet** : `btnVideoJardin`
- **Texte** : `📹 Vidéos Jardins` ou `Voir Vidéos`
- **Position** : Dans la section Gestion Jardins (près des boutons maintenance/recommandation)

### 2. Préparer les vidéos

Créez un dossier `vd` dans votre répertoire de projet :
```
integration/
├── vd/
│   ├── jardin1.mp4
│   ├── jardin2.mp4
│   ├── jardin3.mp4
│   └── jardin4.mp4
├── path.txt
└── ... (autres fichiers)
```

### 3. Configuration du fichier path.txt

Le fichier `path.txt` contient la liste des vidéos au format :
```
Nom du Jardin:chemin/vers/video.mp4
```

Exemple :
```
Jardin Public Centre-Ville:vd/jardin1.mp4
Jardin Botanique:vd/jardin2.mp4
Jardin des Enfants:vd/jardin3.mp4
Parc Municipal:vd/jardin4.mp4
```

## Fonctionnalités incluses

### 📹 Lecteur Vidéo
- Lecture de vidéos MP4
- Contrôles Play/Pause/Stop
- Boutons de sélection de jardins (maximum 4 par défaut)
- Interface moderne avec style vert nature

### 🎴 Génération de Cartes de Visite
Le dialogue vidéo inclut aussi une fonctionnalité bonus :
- Bouton "Générer Carte de Visite Jardinier"
- Création de cartes professionnelles en PNG ou PDF
- Design vert thème nature/jardins
- Informations personnalisables (nom, spécialité, contact)

**Champs de la carte :**
- Nom du jardinier/responsable
- Spécialité (ex: Jardinage, Arrosage, Maintenance)
- Nom du jardin
- Email
- Téléphone
- Adresse

**Sortie :**
- Dossier `cartes_visite_jardins/` créé automatiquement
- Format PNG (850x550 pixels) ou PDF

## Dépendances Qt

Assurez-vous que votre projet inclut :
```cpp
QT += multimedia multimediawidgets
```

Déjà présent dans `projetsmartcity.pro` ✅

## Personnalisation

### Changer les couleurs du dialogue
Éditez `videodialog.cpp`, fonction `setupUi()` :
```cpp
btn->setStyleSheet("QPushButton { background-color: #4CAF50; ... }");
```

### Ajouter plus de vidéos
1. Modifiez `path.txt` pour ajouter des lignes
2. Les boutons s'ajusteront automatiquement en grille 4 colonnes

### Changer le nombre de colonnes
Éditez `videodialog.cpp`, ligne ~130 :
```cpp
int cols = 4;  // Changez cette valeur
```

## Résolution de problèmes

### ❌ Erreur : "Impossible de charger path.txt"
- Vérifiez que `path.txt` existe dans le dossier du projet ou de l'exécutable
- Le fichier sera créé automatiquement avec des valeurs par défaut s'il manque

### ❌ Vidéo introuvable
- Vérifiez les chemins dans `path.txt`
- Assurez-vous que le dossier `vd/` existe
- Les chemins peuvent être absolus ou relatifs au dossier du projet

### ❌ Vidéo ne se lit pas
- Vérifiez le codec de la vidéo (H.264 recommandé)
- Installez les codecs vidéo système si nécessaire
- Testez avec un fichier MP4 standard

### ❌ Bouton invisible dans l'interface
- Ouvrez `mainwindow.ui` dans Qt Designer
- Ajoutez manuellement le QPushButton `btnVideoJardin`
- Recompilez le projet

## Test de l'intégration

1. Compilez le projet
2. Cliquez sur le bouton "Vidéos Jardins" dans la section Jardins
3. Le dialogue vidéo devrait s'ouvrir avec 4 boutons
4. Cliquez sur un bouton pour lire une vidéo (si les fichiers existent)
5. Testez les boutons Play/Pause/Stop

## Exemples de vidéos de test

Vous pouvez utiliser des vidéos libres de droits pour tester :
- **Pexels** : https://www.pexels.com/search/videos/garden/
- **Pixabay** : https://pixabay.com/videos/search/nature/
- Téléchargez 4 vidéos courtes de jardins
- Renommez-les : jardin1.mp4, jardin2.mp4, etc.
- Placez-les dans le dossier `vd/`

## Support

Si vous rencontrez des problèmes :
1. Vérifiez la console de débogage Qt (qDebug)
2. Assurez-vous que Qt Multimedia est installé
3. Vérifiez que les modules sont activés dans le .pro

---

**Développé pour SmartCity - Gestion Jardins**  
Version adaptée du module Cabinet avec thème nature 🌿
