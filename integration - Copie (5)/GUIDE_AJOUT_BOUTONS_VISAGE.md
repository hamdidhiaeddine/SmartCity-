# Guide Rapide - Ajout des Boutons de Reconnaissance Faciale

## Étape 1 : Ajouter le bouton "Capturer Visage" dans l'onglet Employés

### Dans Qt Designer (mainwindow.ui)

1. **Ouvrir mainwindow.ui** dans Qt Designer
2. **Localiser** l'onglet "Employés" (tab_employe)
3. **Glisser-déposer** un **QPushButton** depuis la palette de widgets
4. **Positionner** le bouton :
   - À droite ou en dessous du tableau des employés
   - Ou près des boutons Ajouter/Modifier/Supprimer
5. **Renommer** le bouton :
   - Nom de l'objet : `capturerVisageBtn`
   - Texte : "📷 Capturer Visage"
6. **Styliser** le bouton (optionnel) :
   ```css
   background-color: #9C27B0;
   color: white;
   padding: 10px;
   font-size: 14px;
   border: none;
   border-radius: 5px;
   ```

### Dans mainwindow.cpp (Connexion du signal)

Ajouter dans le constructeur `MainWindow::MainWindow()` après les autres connexions d'employés :

```cpp
// Autour de la ligne 150-200, après les autres connect pour employés
if (ui->capturerVisageBtn) {
    connect(ui->capturerVisageBtn, &QPushButton::clicked, 
            this, &MainWindow::onCapturerVisage);
}
```

## Étape 2 : Ajouter le bouton "Connexion par Visage" dans l'écran de login

### Dans Qt Designer (authentification.ui)

1. **Ouvrir authentification.ui** dans Qt Designer
2. **Glisser-déposer** un **QPushButton**
3. **Positionner** le bouton :
   - En dessous du bouton "Connexion" classique
   - Ou à côté du bouton "Inscription"
4. **Renommer** le bouton :
   - Nom de l'objet : `connexionVisageBtn`
   - Texte : "🔐 Connexion par Visage"
5. **Styliser** le bouton :
   ```css
   background-color: #673AB7;
   color: white;
   padding: 10px;
   font-size: 14px;
   border: none;
   border-radius: 5px;
   ```

### Dans authentification.cpp (Connexion du signal)

Ajouter dans le constructeur `Authentification::Authentification()` :

```cpp
// Autour de la ligne 17-20, après les autres connect
if (ui->connexionVisageBtn) {
    connect(ui->connexionVisageBtn, &QPushButton::clicked, 
            this, &Authentification::onConnexionVisage);
}
```

## Étape 3 : Compiler et Tester

### Compilation
```powershell
cd C:\Users\walae\Desktop\qt_smartcity\integration
qmake
make
```

Ou dans Qt Creator :
- **Build** > **Clean All**
- **Build** > **Build All**

### Test du système

#### Test 1 : Enregistrement d'un visage
1. Lancer l'application
2. Se connecter normalement
3. Aller dans l'onglet **Employés**
4. **Sélectionner** un employé dans le tableau
5. Cliquer sur **"Capturer Visage"**
6. **Sélectionner** une photo du visage de cet employé
7. Vérifier le message de succès

#### Test 2 : Vérification dans la BD
```sql
SELECT ID_EMPLOYE, NOM, PRENOM, 
       CASE WHEN PHOTO IS NULL THEN 'NON' ELSE 'OUI' END AS HAS_PHOTO
FROM EMPLOYES;
```

#### Test 3 : Authentification par visage
1. **Redémarrer** l'application
2. À l'écran de connexion, cliquer sur **"Connexion par Visage"**
3. **Sélectionner** la même photo que lors de l'enregistrement
4. Vérifier que l'authentification réussit

#### Test 4 : Test avec une photo différente
1. À l'écran de connexion, cliquer sur **"Connexion par Visage"**
2. **Sélectionner** une photo d'une autre personne
3. Vérifier que l'authentification échoue

## Disposition Recommandée des Boutons

### Onglet Employés
```
┌─────────────────────────────────────────┐
│  Tableau des Employés                   │
│  ┌────────┬──────┬────────┬──────┐      │
│  │ Nom    │ Prénom│ Email  │ ...  │      │
│  └────────┴──────┴────────┴──────┘      │
│                                          │
│  [Ajouter] [Modifier] [Supprimer]       │
│  [📷 Capturer Visage] ← NOUVEAU         │
└─────────────────────────────────────────┘
```

### Écran de Connexion
```
┌─────────────────────────────────────────┐
│         SMART CITY LOGIN                │
│                                          │
│  Identifiant: [____________]            │
│  Mot de passe: [____________]           │
│                                          │
│  [      Se Connecter      ]             │
│  [🔐 Connexion par Visage ] ← NOUVEAU   │
│                                          │
│  Mot de passe oublié ?                  │
└─────────────────────────────────────────┘
```

## Vérification Finale

### Checklist avant tests
- [ ] Colonne PHOTO ajoutée à la table EMPLOYES
- [ ] Fichiers facialrecognition.h/.cpp ajoutés au projet
- [ ] Fichiers facecapturedialog.h/.cpp ajoutés au projet
- [ ] Fichiers ajoutés dans projetsmartcity.pro
- [ ] Includes ajoutés dans mainwindow.h et authentification.h
- [ ] Bouton capturerVisageBtn ajouté dans mainwindow.ui
- [ ] Bouton connexionVisageBtn ajouté dans authentification.ui
- [ ] Signaux connectés dans les constructeurs
- [ ] Projet compilé sans erreurs

### Si erreurs de compilation
```powershell
# Nettoyer le projet
cd C:\Users\walae\Desktop\qt_smartcity\integration
Remove-Item -Recurse -Force debug, release, Makefile*, *.pro.user

# Recompiler
qmake
make
```

### Si le bouton n'apparaît pas
1. Vérifier que l'objet est bien nommé dans le .ui
2. Régénérer le fichier ui_mainwindow.h : **Build > Run qmake**
3. Reconstruire : **Build > Rebuild All**

## Personnalisation Avancée

### Icônes pour les boutons
```cpp
// Dans mainwindow.cpp (constructeur)
if (ui->capturerVisageBtn) {
    ui->capturerVisageBtn->setIcon(QIcon(":/icons/camera.png"));
    ui->capturerVisageBtn->setIconSize(QSize(24, 24));
}
```

### Message de confirmation avec détails
```cpp
// Dans onCapturerVisage() après succès
QString message = QString(
    "Photo enregistrée avec succès!\n\n"
    "Employé: %1 %2\n"
    "ID: %3"
).arg(nom, prenom, QString::number(employeId));

QMessageBox::information(this, "Succès", message);
```

### Affichage de la photo dans le formulaire
```cpp
// Ajouter un QLabel photoLabel dans mainwindow.ui
// Dans onEmployeSelectionChanged()
if (FacialRecognition::hasFaceImage(employeId)) {
    QString error;
    QImage faceImage = FacialRecognition::loadFaceImage(employeId, error);
    if (!faceImage.isNull()) {
        ui->photoLabel->setPixmap(
            QPixmap::fromImage(faceImage).scaled(150, 150, 
                Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    }
} else {
    ui->photoLabel->setText("Aucune photo");
}
```

## Support

Si vous rencontrez des problèmes :
1. Vérifiez les logs de debug (QDebug)
2. Consultez README_RECONNAISSANCE_FACIALE.md
3. Vérifiez que la colonne PHOTO existe : `DESC EMPLOYES`
4. Testez avec une image simple (photo carrée, bonne qualité)
