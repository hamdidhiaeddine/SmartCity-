# Instructions pour exécuter l'application

## Méthode 1 : Via Qt Creator (Recommandé)

### Étape 1 : Ouvrir le projet
1. Ouvrez Qt Creator
2. Fichier → Ouvrir un fichier ou un projet
3. Sélectionnez `projetsmartcity.pro`

### Étape 2 : Configurer le kit Qt
1. Qt Creator va demander de configurer le projet
2. Sélectionnez **"Desktop Qt 6.7.3 MinGW 64-bit"** (ou votre version Qt 6)
3. Cliquez sur **"Configure Project"**

### Étape 3 : Configurer l'exécution manuellement
1. Cliquez sur l'icône **"Projets"** dans la barre latérale (ou appuyez sur `Ctrl+5`)
2. Dans la section **"Run"** :
   - **Executable** : Cliquez sur le bouton **"..."** et naviguez vers :
     ```
     C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe
     ```
   - **Working directory** : 
     ```
     C:\ayooo
     ```

### Étape 4 : Construire le projet
1. Cliquez sur l'icône **"Build"** (marteau) ou appuyez sur `Ctrl+B`
2. Attendez la fin de la compilation

### Étape 5 : Exécuter
1. Cliquez sur l'icône **"Run"** (flèche verte) ou appuyez sur `Ctrl+R`

---

## Méthode 2 : Exécution directe (Alternative)

Si Qt Creator ne fonctionne pas, vous pouvez lancer l'application directement :

1. Double-cliquez sur le fichier **`Lancer_Application.bat`** dans le dossier du projet
2. Ou exécutez manuellement :
   ```
   C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe
   ```

---

## Vérification

L'exécutable doit se trouver à :
```
C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe
```

Si ce fichier n'existe pas, vous devez d'abord compiler le projet dans Qt Creator.

---

## Résolution des problèmes

### "No executable specified"
- Vérifiez que l'exécutable existe au chemin indiqué ci-dessus
- Configurez manuellement le chemin dans Qt Creator (Projets → Run → Executable)
- Ou utilisez le fichier `Lancer_Application.bat`

### Erreurs de compilation
- Assurez-vous d'utiliser Qt 6.7.3 (ou une version compatible)
- Vérifiez que tous les modules Qt sont installés (charts, network, sql, etc.)





