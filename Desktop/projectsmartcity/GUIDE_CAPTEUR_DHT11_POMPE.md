# 🌡️ Guide d'utilisation - Capteur DHT11 + Pompe d'arrosage

## 📋 Vue d'ensemble

Ce système permet de :
- Lire la température et l'humidité avec un capteur DHT11
- Comparer la température avec celle des jardins en base de données
- Activer automatiquement une pompe d'eau pendant 3 secondes pour les jardins qui ont besoin d'arrosage

## 🔧 Matériel requis

### Capteur DHT11
- **VCC** → 5V Arduino
- **DATA** → Pin 2 Arduino (avec résistance 10KΩ entre VCC et DATA)
- **GND** → GND Arduino

### Pompe à eau (via module relais)
- **Signal** → Pin 7 Arduino
- **VCC** → 5V Arduino
- **GND** → GND Arduino

## 📥 Installation Arduino

1. **Télécharger la bibliothèque DHT**
   - Ouvrir l'IDE Arduino
   - Aller dans `Croquis` > `Inclure une bibliothèque` > `Gérer les bibliothèques`
   - Rechercher "DHT sensor library" par Adafruit
   - Cliquer sur `Installer`

2. **Téléverser le code**
   - Ouvrir le fichier `arduino_dht11_pompe.ino`
   - Sélectionner votre carte Arduino (Tools > Board)
   - Sélectionner le port COM (Tools > Port)
   - Cliquer sur `Téléverser` (flèche →)

3. **Vérifier le fonctionnement**
   - Ouvrir le Moniteur Série (Ctrl+Shift+M)
   - Régler la vitesse à **9600 bauds**
   - Vous devriez voir :
     ```
     ========================================
     DHT11 + POMPE - SMART CITY
     ========================================
     Temperature = 25.00 °C
     Humidite = 60.00 %
     Pompe = 0
     ```

## 🖥️ Utilisation dans Qt

### Étape 1 : Connexion du capteur

1. Brancher l'Arduino avec le DHT11 et la pompe
2. Lancer l'application Qt Smart City
3. Aller dans le menu **Jardins**
4. Cliquer sur le bouton **🔌 Connecter Capteur**
5. Sélectionner le port COM de votre Arduino (ex: COM9)
6. Le bouton devient **✅ Capteur Connecté**

### Étape 2 : Démarrer la lecture

1. Cliquer sur le bouton **▶️ Lire Température**
2. Le bouton devient **⏹️ Arrêter Lecture** (rouge)
3. La lecture de température démarre toutes les 2 secondes
4. L'affichage se met à jour :
   - 🌡️ Température: XX°C
   - 💧 Humidité: XX%

### Étape 3 : Détection automatique

Toutes les **5 secondes**, le système :

1. **Récupère la température** du capteur DHT11
2. **Compare avec les jardins** en base de données
3. **Si température capteur > température sol du jardin** :
   - ✅ Le jardin a besoin d'arrosage
   - 🚿 La pompe s'active pendant **3 secondes**
   - 💬 Une boîte de dialogue affiche les jardins arrosés

### Étape 4 : Arrêter la lecture

1. Cliquer sur **⏹️ Arrêter Lecture**
2. Le bouton redevient **▶️ Lire Température** (vert)
3. La vérification automatique s'arrête
4. La pompe ne se déclenchera plus

## 📊 Fonctionnement détaillé

### Logique de comparaison

```
SI (Température_Capteur > Température_Sol_Jardin) ALORS
    → Le jardin a besoin d'arrosage
    → Activer pompe pendant 3s
```

**Exemple :**
- Capteur DHT11 : **30°C**
- Jardin #1 (Potager) : Sol à **22°C** → ✅ ARROSER (30 > 22)
- Jardin #2 (Pelouse) : Sol à **28°C** → ✅ ARROSER (30 > 28)
- Jardin #3 (Serre) : Sol à **32°C** → ❌ PAS D'ARROSAGE (30 < 32)

### Gestion de la pompe

- **Activation** : Envoi de la commande `1` à l'Arduino
- **Durée** : 3 secondes (timer automatique)
- **Arrêt** : Envoi de la commande `0` à l'Arduino
- **Sécurité** : La pompe s'arrête automatiquement en cas de déconnexion

## 🔍 Dépannage

### Problème : Aucune donnée reçue

**Solutions :**
1. Vérifier le câblage du DHT11 (VCC, DATA, GND)
2. Vérifier la résistance 10KΩ entre VCC et DATA
3. Ouvrir le Moniteur Série Arduino pour voir les données brutes
4. Vérifier que le bon port COM est sélectionné

### Problème : La pompe ne s'active pas

**Solutions :**
1. Vérifier le câblage du relais (Signal → Pin 7)
2. Tester manuellement via le Moniteur Série :
   - Taper `1` puis Entrée → Pompe ON
   - Taper `0` puis Entrée → Pompe OFF
3. Vérifier que la lecture est bien démarrée (bouton rouge)
4. Vérifier qu'au moins un jardin nécessite un arrosage

### Problème : Pas d'alerte d'arrosage

**Vérifications :**
1. La lecture est-elle démarrée ? (bouton rouge)
2. Y a-t-il des jardins en base de données ?
3. La température du capteur est-elle supérieure à celle des jardins ?
4. Consulter les logs dans la console Qt :
   ```
   🔍 Vérification jardins - Temp capteur: 30.0°C
   🌿 Nombre de jardins en BD: 3
     Jardin #1: Potager - Sol: 22.0°C vs Capteur: 30.0°C
       ✅ NÉCESSITE ARROSAGE
   ```

## 🧪 Test du système

### Test 1 : Lecture température
```
1. Connecter le capteur
2. Démarrer la lecture
3. Vérifier l'affichage de la température
4. Souffler sur le capteur → température doit augmenter
```

### Test 2 : Pompe manuelle
```
1. Ouvrir Arduino IDE > Moniteur Série
2. Taper "1" puis Entrée
3. Vérifier que "POMPE:MARCHE" s'affiche
4. Taper "0" puis Entrée
5. Vérifier que "POMPE:ARRET" s'affiche
```

### Test 3 : Arrosage automatique
```
1. Ajouter un jardin avec température sol basse (ex: 20°C)
2. Démarrer la lecture
3. Attendre 5 secondes
4. La pompe devrait s'activer automatiquement
5. Une boîte de dialogue devrait apparaître
```

## 📝 Commandes série

### Qt → Arduino
- `1` ou `POMPE_ON` : Activer la pompe
- `0` ou `POMPE_OFF` : Arrêter la pompe
- `STATUS` : Afficher l'état actuel

### Arduino → Qt
- `Temperature = XX.XX °C` : Données de température
- `Humidite = XX.XX %` : Données d'humidité
- `Pompe = 0/1` : État de la pompe
- `POMPE:MARCHE` : Confirmation activation
- `POMPE:ARRET` : Confirmation arrêt

## ⚙️ Configuration avancée

### Modifier l'intervalle de vérification
Dans `temperaturesensor.cpp`, ligne 13 :
```cpp
m_timer->setInterval(5000);  // 5000 ms = 5 secondes
```

### Modifier la durée de la pompe
Dans `temperaturesensor.cpp`, ligne 18 :
```cpp
m_pompeTimer->setInterval(3000);  // 3000 ms = 3 secondes
```

### Modifier les pins Arduino
Dans `arduino_dht11_pompe.ino` :
```cpp
#define DHTPIN 2        // Pin du capteur DHT11
#define POMPE_PIN 7     // Pin de la pompe
```

## 🎯 Bonnes pratiques

1. **Toujours** arrêter la lecture avant de déconnecter le capteur
2. **Ne pas** débrancher l'Arduino pendant que la pompe fonctionne
3. **Vérifier** régulièrement que les données de température sont cohérentes
4. **Calibrer** les températures des jardins selon les besoins réels
5. **Tester** la pompe manuellement avant d'utiliser le mode automatique

## 📞 Support

En cas de problème, consulter les logs dans :
- Console Qt (fenêtre Application Output)
- Moniteur Série Arduino (9600 bauds)

Les messages importants :
- ✅ = Succès
- ❌ = Erreur
- ⚠️ = Avertissement
- 🔍 = Vérification en cours
- 💦 = Arrosage détecté
- 🚿 = Pompe active
