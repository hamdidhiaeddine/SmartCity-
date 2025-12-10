# 🔍 Pourquoi l'erreur "No executable specified" ?

## ❓ Explication simple

**L'exécutable existe**, mais **Qt Creator ne sait pas où il se trouve**.

C'est comme avoir une clé dans votre poche mais ne pas savoir quelle poche !

## 🔎 Causes possibles

### 1. **Configuration non rechargée**
- Qt Creator a peut-être ouvert le projet avant que la configuration soit corrigée
- **Solution**: Fermer et rouvrir Qt Creator

### 2. **Mauvais format de configuration**
- Le fichier `.pro.user` utilise un format Qt 4 au lieu de Qt 6
- Qt Creator moderne ne le reconnaît pas correctement
- **Solution**: Reconfigurer manuellement dans Qt Creator

### 3. **Plusieurs configurations de run**
- Il peut y avoir 2 configurations de run dans le projet
- Qt Creator utilise peut-être la mauvaise
- **Solution**: Vérifier quelle configuration est active

### 4. **Kit Qt incorrect**
- Si vous utilisez un kit Qt 5 au lieu de Qt 6.7.3
- Les chemins de build sont différents
- **Solution**: Sélectionner le bon kit (Qt 6.7.3 MinGW 64-bit)

## ✅ Solution définitive

### Méthode 1: Reconfigurer dans Qt Creator (RECOMMANDÉ)

1. **Dans Qt Creator**, allez dans **Projects** (Ctrl+5)
2. Cliquez sur l'onglet **"Run"** (en haut)
3. Vérifiez que vous voyez:
   - **Executable**: `C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe`
   - **Working directory**: `C:\ayooo`

4. Si ces valeurs sont **vides** ou **incorrectes**:
   - Cliquez sur **"..."** à côté de "Executable"
   - Naviguez vers le bon chemin
   - Sélectionnez `Atelier_Connexion.exe`
   - Cliquez sur **"Apply"**

5. **Fermez et rouvrez Qt Creator** pour recharger la configuration

### Méthode 2: Supprimer et recréer la configuration

1. **Fermez Qt Creator**
2. Supprimez le fichier: `projetsmartcity.pro.user`
3. **Rouvrez Qt Creator**
4. Ouvrez le projet `projetsmartcity.pro`
5. Sélectionnez **Qt 6.7.3 MinGW 64-bit**
6. Configurez l'exécutable (Méthode 1, étape 3-4)

### Méthode 3: Utiliser le script batch (IMMÉDIAT)

Double-cliquez sur **`Lancer_Application.bat`** - cela fonctionne toujours !

## 🎯 Pourquoi ça arrive souvent ?

1. **Qt Creator ne devine pas** où vous compilez
2. Les **chemins de build varient** selon le kit Qt
3. Les **projets peuvent être déplacés** d'un dossier à l'autre
4. Les **anciennes configurations** peuvent rester en mémoire

## 📝 Résumé

- ✅ L'exécutable **existe** et est **compilé**
- ❌ Qt Creator **ne sait pas** où il se trouve
- ✅ **Solution**: Lui indiquer le chemin une fois
- ✅ **Résultat**: Ça fonctionnera ensuite automatiquement

---

**Astuce**: Après avoir configuré une fois, Qt Creator se souviendra du chemin pour les prochaines fois !





