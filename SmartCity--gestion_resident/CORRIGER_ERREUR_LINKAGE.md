# 🔧 Correction de l'erreur de linkage

## ❌ Erreur actuelle
```
collect2.exe: error: ld returned 1 exit status
[Makefile.Debug:98: C:/ayooo/build/win32/mingw-Debug/debugAtelier_Connexion.exe] Error 1
```

## 🔍 Cause du problème
Le projet essaie de se compiler avec un **mauvais kit Qt** (probablement Qt 5 ou 32-bit) au lieu de **Qt 6.7.3 MinGW 64-bit**.

Le chemin de build `win32/mingw-Debug` indique qu'un kit incorrect est utilisé.

## ✅ Solution

### Étape 1 : Nettoyer le projet
1. Dans Qt Creator, allez dans **Build** → **Clean All**
2. Supprimez manuellement le dossier `build/win32` s'il existe

### Étape 2 : Reconfigurer le projet avec le bon kit
1. Cliquez sur l'icône **Projets** (ou `Ctrl+5`)
2. Dans la section **Kit**, vous devriez voir plusieurs kits disponibles
3. **DÉSÉLECTIONNEZ** tous les kits sauf :
   - ✅ **Desktop Qt 6.7.3 MinGW 64-bit**
4. Si ce kit n'apparaît pas :
   - Allez dans **Tools** → **Options** → **Kits**
   - Vérifiez que Qt 6.7.3 MinGW 64-bit est installé et configuré

### Étape 3 : Vérifier la configuration de build
Dans l'onglet **Build** :
- **Build directory** devrait être : `C:/ayooo/build/Desktop_Qt_6_7_3_MinGW_64_bit-Debug`
- **NOT** `C:/ayooo/build/win32/mingw-Debug`

### Étape 4 : Exécuter qmake
1. Dans Qt Creator, allez dans **Build** → **Run qmake**
2. Cela régénère les Makefiles avec le bon kit

### Étape 5 : Reconstruire
1. **Build** → **Rebuild Project** (ou `Ctrl+Shift+B`)
2. Attendez la fin de la compilation

## 🎯 Vérification

Après la reconstruction, vérifiez que :
- Le dossier de build est : `build/Desktop_Qt_6_7_3_MinGW_64_bit-Debug/`
- L'exécutable est créé : `build/Desktop_Qt_6_7_3_MinGW_64_bit-Debug/debug/Atelier_Connexion.exe`
- **PAS** dans `build/win32/mingw-Debug/`

## 🚨 Si le problème persiste

### Option 1 : Supprimer tous les fichiers de build
```powershell
cd C:\ayooo
Remove-Item -Recurse -Force build\win32
Remove-Item -Recurse -Force build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug
```
Puis reconstruisez dans Qt Creator.

### Option 2 : Vérifier les kits Qt installés
1. **Tools** → **Options** → **Kits**
2. Vérifiez que vous avez :
   - ✅ Qt 6.7.3 MinGW 64-bit (actif)
   - ❌ Qt 5.x (désactivé pour ce projet)
   - ❌ Qt 32-bit (désactivé pour ce projet)

### Option 3 : Recréer le projet
1. Fermez Qt Creator
2. Supprimez tous les fichiers `.pro.user*`
3. Rouvrez le projet
4. Sélectionnez **uniquement** Qt 6.7.3 MinGW 64-bit

## 📝 Notes importantes

- Le projet utilise **Qt 6.7.3** avec les modules : `charts`, `network`, `sql`, `printsupport`
- Assurez-vous que tous ces modules sont installés pour Qt 6.7.3
- Le code utilise des API Qt 6 (pas de namespace QtCharts, `Qt::SkipEmptyParts`, etc.)




