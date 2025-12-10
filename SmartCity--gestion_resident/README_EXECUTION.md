# 🚀 Guide d'exécution - Projet SmartCity

## ❌ Problème actuel
Qt Creator affiche: **"No executable specified"**

## ✅ Solution rapide (30 secondes)

### Option 1: Lancer directement (RECOMMANDÉ)
Double-cliquez sur: **`Lancer_Application.bat`**

Cela lance l'application sans configuration Qt Creator.

---

### Option 2: Configurer dans Qt Creator (2 minutes)

#### Étape 1: Ouvrir les paramètres du projet
1. Dans Qt Creator, cliquez sur l'icône **"Projets"** dans la barre latérale gauche
   - Ou appuyez sur `Ctrl+5`
   - Ou menu: **Fenêtre → Vues → Projets**

#### Étape 2: Sélectionner le bon kit
- En haut de la fenêtre "Projets", vérifiez que vous avez:
  - ✅ **Desktop Qt 6.7.3 MinGW 64-bit**
- Si ce n'est pas le cas, cliquez sur le kit actif et sélectionnez Qt 6.7.3

#### Étape 3: Configurer l'exécution
1. Cliquez sur l'onglet **"RUN"** (en haut, à côté de "Build")

2. Dans la section **"Executable"**:
   - Cliquez sur le bouton **"..."** (trois points)
   - Naviguez vers: `C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\`
   - Sélectionnez: **Atelier_Connexion.exe**
   - Cliquez sur **"Ouvrir"**

3. Dans la section **"Working directory"**:
   - Cliquez sur le bouton **"..."**
   - Naviguez vers: `C:\ayooo`
   - Cliquez sur **"Ouvrir"**
   - OU tapez directement: `C:\ayooo`

4. Cliquez sur **"Apply"** (Appliquer) en bas à droite

#### Étape 4: Exécuter
- Cliquez sur le bouton **"Run"** (flèche verte) ou appuyez sur `Ctrl+R`

---

## 📍 Chemins importants

- **Exécutable**: `C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe`
- **Répertoire de travail**: `C:\ayooo`
- **Fichier projet**: `C:\ayooo\projetsmartcity.pro`

---

## 🔍 Vérifications

### L'exécutable existe-t-il?
Ouvrez l'Explorateur Windows et vérifiez:
```
C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe
```

Si ce fichier **n'existe pas**:
1. Dans Qt Creator, allez dans **Build → Build Project** (ou `Ctrl+B`)
2. Attendez la fin de la compilation
3. Réessayez d'exécuter

---

## 🆘 Si le problème persiste

### Solution 1: Nettoyer et reconstruire
1. **Build → Clean All**
2. **Build → Rebuild Project** (`Ctrl+Shift+B`)
3. Configurez à nouveau l'exécutable (Étape 3 ci-dessus)

### Solution 2: Supprimer la configuration
1. Fermez Qt Creator
2. Supprimez le fichier: `projetsmartcity.pro.user`
3. Rouvrez Qt Creator
4. Ouvrez le projet
5. Sélectionnez Qt 6.7.3 MinGW 64-bit
6. Configurez l'exécutable (Étape 3 ci-dessus)

### Solution 3: Utiliser le script batch
Double-cliquez sur **`Lancer_Application.bat`** - cela fonctionne toujours!

---

## 📝 Notes

- Le projet utilise **Qt 6.7.3** (pas Qt 5.x)
- Assurez-vous d'utiliser le kit **MinGW 64-bit** (pas 32-bit)
- L'exécutable est déjà compilé et fonctionnel

---

## ✅ Vérification finale

Après configuration, vous devriez voir dans Qt Creator:
- **Executable**: `C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe` ✓
- **Working directory**: `C:\ayooo` ✓

Si ces valeurs sont correctes, cliquez sur **Run** et l'application devrait démarrer!




