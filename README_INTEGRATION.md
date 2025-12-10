# 🚀 GUIDE D'INTÉGRATION DU PROJET SMARTCITY COMPLET

## ✅ Étape 1: Créer les tables dans la base de données

Connectez-vous à Oracle avec l'utilisateur `hiba` et exécutez le script:

```bash
sqlplus hiba/esprit18@XE @INTEGRATION_TABLES_BDD.sql
```

Cela va créer les tables suivantes:

- **EMPLOYES** - Gestion des employés
- **VEHICULE** - Gestion des véhicules avec séquence SEQ_VEHI
- **GEST_MAISON** - Gestion des maisons
- **GEST_ALERTES** - Système d'alertes avec séquence

## ✅ Étape 2: Fichiers déjà créés

Les fichiers suivants ont été créés/copiés dans `C:\ayooo`:

### Classes métier

- ✅ `Employee.h` / `Employee.cpp` - Gestion des employés
- ✅ `vehicule.h` / `vehicule.cpp` - Gestion des véhicules

### Fichier projet

- ✅ `projetsmartcity.pro` - MIS À JOUR avec les nouveaux fichiers

### Configuration

- ✅ `mainwindow.h` - MIS À JOUR avec includes Employee et vehicule

## ⚠️ Étape 3: Ajouter l'implémentation dans mainwindow.cpp

Vous devez maintenant ajouter les implémentations des slots dans `mainwindow.cpp`.

### 3.1 Ajouter les implémentations des slots de navigation

Trouvez la fonction `void MainWindow::onGestionResidents()` et ajoutez AVANT celle-ci:

```cpp
// ============================================================
// SLOTS DE NAVIGATION
// ============================================================

void MainWindow::onGestionEmployes()
{
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
    ui->stackedWidget_2->setCurrentWidget(ui->page_4); // Page CRUD employés
    chargerEmployes();
}

void MainWindow::onGestionVehicules()
{
    ui->stackedWidget->setCurrentWidget(ui->pageVehicules);
    ui->stackedWidget_3->setCurrentWidget(ui->page_9); // Page CRUD véhicules
    chargerVehicules();
}
```

### 3.2 Ajouter les fonctions de gestion des employés

Ajoutez APRÈS les fonctions de navigation:

```cpp
// ============================================================
// GESTION DES EMPLOYÉS
// ============================================================

void MainWindow::chargerEmployes()
{
    QString err;
    QVector<Employee> employes = Employee::fetchAll(err);

    if (!err.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Impossible de charger les employés:\n" + err);
        return;
    }

    ui->tableau->setRowCount(0);
    for (const Employee &e : employes) {
        int row = ui->tableau->rowCount();
        ui->tableau->insertRow(row);
        ui->tableau->setItem(row, 0, new QTableWidgetItem(QString::number(e.idEmploye)));
        ui->tableau->setItem(row, 1, new QTableWidgetItem(e.nom));
        ui->tableau->setItem(row, 2, new QTableWidgetItem(e.prenom));
        ui->tableau->setItem(row, 3, new QTableWidgetItem(e.email));
        ui->tableau->setItem(row, 4, new QTableWidgetItem(e.poste));
        ui->tableau->setItem(row, 5, new QTableWidgetItem(QString::number(e.salaire, 'f', 2)));
    }
}

void MainWindow::onAjouterEmploye()
{
    Employee e;
    e.nom = ui->nomline->text().trimmed();
    e.prenom = ui->prenomline->text().trimmed();
    e.email = ui->emailline->text().trimmed();
    e.poste = ui->posteline->text().trimmed();
    e.salaire = ui->salaireline->text().toDouble();
    e.adresse = ui->adresseline->text().trimmed();
    e.telephone = ui->telephoneline->text().toLongLong();

    if (e.nom.isEmpty() || e.prenom.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Nom et prénom sont obligatoires");
        return;
    }

    QString err;
    int outId;
    if (e.insert(outId, err)) {
        QMessageBox::information(this, "Succès", "Employé ajouté avec succès (ID: " + QString::number(outId) + ")");
        chargerEmployes();
        reinitialiserFormulaireEmploye();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout:\n" + err);
    }
}

void MainWindow::onModifierEmploye()
{
    QList<QTableWidgetItem*> selection = ui->tableau->selectedItems();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un employé à modifier");
        return;
    }

    int row = selection.first()->row();
    int id = ui->tableau->item(row, 0)->text().toInt();

    Employee e;
    e.nom = ui->nomline->text().trimmed();
    e.prenom = ui->prenomline->text().trimmed();
    e.email = ui->emailline->text().trimmed();
    e.poste = ui->posteline->text().trimmed();
    e.salaire = ui->salaireline->text().toDouble();
    e.adresse = ui->adresseline->text().trimmed();
    e.telephone = ui->telephoneline->text().toLongLong();

    QString err;
    if (e.updateById(id, err)) {
        QMessageBox::information(this, "Succès", "Employé modifié avec succès");
        chargerEmployes();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification:\n" + err);
    }
}

void MainWindow::onSupprimerEmploye()
{
    QList<QTableWidgetItem*> selection = ui->tableau->selectedItems();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un employé à supprimer");
        return;
    }

    int row = selection.first()->row();
    int id = ui->tableau->item(row, 0)->text().toInt();
    QString nom = ui->tableau->item(row, 1)->text();

    if (QMessageBox::question(this, "Confirmation",
        "Supprimer l'employé " + nom + " ?") == QMessageBox::Yes) {
        QString err;
        if (Employee::removeById(id, err)) {
            QMessageBox::information(this, "Succès", "Employé supprimé");
            chargerEmployes();
            reinitialiserFormulaireEmploye();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression:\n" + err);
        }
    }
}

void MainWindow::reinitialiserFormulaireEmploye()
{
    ui->idline->clear();
    ui->nomline->clear();
    ui->prenomline->clear();
    ui->emailline->clear();
    ui->posteline->clear();
    ui->salaireline->clear();
    ui->adresseline->clear();
    ui->telephoneline->clear();
}

// ============================================================
// GESTION DES VÉHICULES
// ============================================================

void MainWindow::chargerVehicules()
{
    Vehicule v;
    QSqlQuery query = v.afficher();

    ui->tableau_3->setRowCount(0);
    while (query.next()) {
        int row = ui->tableau_3->rowCount();
        ui->tableau_3->insertRow(row);
        ui->tableau_3->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // IMMAT
        ui->tableau_3->setItem(row, 1, new QTableWidgetItem(query.value(1).toString())); // MARQUE
        ui->tableau_3->setItem(row, 2, new QTableWidgetItem(query.value(2).toString())); // MODELE
        ui->tableau_3->setItem(row, 3, new QTableWidgetItem(query.value(3).toString())); // TYPE
        ui->tableau_3->setItem(row, 4, new QTableWidgetItem(query.value(4).toString())); // ETAT
        ui->tableau_3->setItem(row, 5, new QTableWidgetItem(query.value(5).toString())); // SERVICE
        ui->tableau_3->setItem(row, 6, new QTableWidgetItem(query.value(6).toDate().toString("dd/MM/yyyy")));
    }
}

void MainWindow::onAjouterVehicule()
{
    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    QString type = ui->triemail_2->currentText(); // Combobox Type
    QString etat = ui->Etatline_2->currentText(); // Combobox Etat
    QString service = ui->serviceline_2->text().trimmed();
    QDate dateMaint = ui->datemaintline_2->date();

    if (immat.isEmpty() || marque.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Immatriculation et marque sont obligatoires");
        return;
    }

    Vehicule v(immat, marque, modele, type, etat, service, dateMaint);
    if (v.ajouter()) {
        QMessageBox::information(this, "Succès", "Véhicule ajouté avec succès");
        chargerVehicules();
        reinitialiserFormulaireVehicule();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout du véhicule");
    }
}

void MainWindow::onModifierVehicule()
{
    QList<QTableWidgetItem*> selection = ui->tableau_3->selectedItems();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un véhicule à modifier");
        return;
    }

    int row = selection.first()->row();
    QString oldImmat = ui->tableau_3->item(row, 0)->text();

    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    QString type = ui->triemail_2->currentText();
    QString etat = ui->Etatline_2->currentText();
    QString service = ui->serviceline_2->text().trimmed();
    QDate dateMaint = ui->datemaintline_2->date();

    Vehicule v(immat, marque, modele, type, etat, service, dateMaint);
    if (v.modifier(oldImmat)) {
        QMessageBox::information(this, "Succès", "Véhicule modifié avec succès");
        chargerVehicules();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification");
    }
}

void MainWindow::onSupprimerVehicule()
{
    QList<QTableWidgetItem*> selection = ui->tableau_3->selectedItems();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un véhicule à supprimer");
        return;
    }

    int row = selection.first()->row();
    QString immat = ui->tableau_3->item(row, 0)->text();

    if (QMessageBox::question(this, "Confirmation",
        "Supprimer le véhicule " + immat + " ?") == QMessageBox::Yes) {
        Vehicule v;
        if (v.supprimer(immat)) {
            QMessageBox::information(this, "Succès", "Véhicule supprimé");
            chargerVehicules();
            reinitialiserFormulaireVehicule();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression");
        }
    }
}

void MainWindow::reinitialiserFormulaireVehicule()
{
    ui->immatline_2->clear();
    ui->marqueline_2->clear();
    ui->modeleline_2->clear();
    ui->serviceline_2->clear();
    ui->datemaintline_2->setDate(QDate::currentDate());
}
```

### 3.3 Connecter les boutons dans connectButtons()

Ajoutez dans la fonction `connectButtons()`:

```cpp
// Employés
connect(ui->ajouter, &QPushButton::clicked, this, &MainWindow::onAjouterEmploye);
connect(ui->modifier, &QPushButton::clicked, this, &MainWindow::onModifierEmploye);
connect(ui->supprimer, &QPushButton::clicked, this, &MainWindow::onSupprimerEmploye);

// Véhicules
connect(ui->ajouter_3, &QPushButton::clicked, this, &MainWindow::onAjouterVehicule);
connect(ui->modifier_3, &QPushButton::clicked, this, &MainWindow::onModifierVehicule);
connect(ui->supprimer_3, &QPushButton::clicked, this, &MainWindow::onSupprimerVehicule);
```

### 3.4 Ajouter les déclarations dans mainwindow.h

Ajoutez dans la section `private slots:`:

```cpp
void onAjouterEmploye();
void onModifierEmploye();
void onSupprimerEmploye();
void chargerEmployes();

void onAjouterVehicule();
void onModifierVehicule();
void onSupprimerVehicule();
void chargerVehicules();
```

Ajoutez dans la section `private:`:

```cpp
void reinitialiserFormulaireEmploye();
void reinitialiserFormulaireVehicule();
```

## ✅ Étape 4: Compiler le projet

```powershell
cd C:\ayooo
qmake projetsmartcity.pro
mingw32-make
```

Ou utilisez Qt Creator:

1. Ouvrez `projetsmartcity.pro` dans Qt Creator
2. Cliquez sur Build > Build Project
3. Cliquez sur Run

## ✅ Étape 5: Tester l'intégration

### Test de la base de données

1. Lancez l'application
2. Cliquez sur le bouton "Employés" dans le menu latéral
3. Vérifiez que les 2 employés de test apparaissent
4. Testez Ajouter/Modifier/Supprimer

### Test des véhicules

1. Cliquez sur le bouton "Véhicules"
2. Vérifiez que les 2 véhicules de test apparaissent
3. Testez Ajouter/Modifier/Supprimer

### Test de l'historique des résidents

1. Cliquez sur "Résidents"
2. Ajoutez un nouveau résident
3. Cliquez sur "Afficher Historique"
4. Vérifiez qu'une entrée "Ajout" apparaît
5. Modifiez le résident → Vérifiez "Modification"
6. Supprimez le résident → Vérifiez "Suppression"

## 📋 Résumé des changements

| Fichier                      | Action     | Description                                |
| ---------------------------- | ---------- | ------------------------------------------ |
| `INTEGRATION_TABLES_BDD.sql` | ✅ Créé    | Script SQL pour créer toutes les tables    |
| `Employee.h`                 | ✅ Créé    | Classe de gestion des employés             |
| `Employee.cpp`               | ✅ Créé    | Implémentation CRUD employés               |
| `vehicule.h`                 | ✅ Créé    | Classe de gestion des véhicules            |
| `vehicule.cpp`               | ✅ Créé    | Implémentation CRUD véhicules              |
| `projetsmartcity.pro`        | ✅ Modifié | Ajout des nouveaux fichiers                |
| `mainwindow.h`               | ✅ Modifié | Ajout des includes Employee et vehicule    |
| `mainwindow.cpp`             | ⚠️ À FAIRE | Ajouter les implémentations (voir Étape 3) |

## 🎯 Fonctionnalités intégrées

✅ **Module Employés**

- Ajout, modification, suppression d'employés
- Recherche et tri
- Export PDF
- Statistiques par poste

✅ **Module Véhicules**

- Ajout, modification, suppression de véhicules
- Recherche par immatriculation/type
- Tri par date de maintenance
- Export PDF

✅ **Module Résidents** (déjà existant)

- CRUD complet
- Historique avec triggers automatiques
- Envoi/Réception SMS via Twilio

✅ **Base de données Oracle**

- Schéma HIBA unifié
- Triggers pour historique des résidents
- Séquences auto-increment pour véhicules et alertes
- Données de test incluses

## 🚨 Problèmes potentiels et solutions

### Erreur: "Employee.h: No such file or directory"

**Solution:** Vérifiez que `Employee.h` et `Employee.cpp` sont bien dans `C:\ayooo`

### Erreur: "table or view does not exist"

**Solution:** Exécutez le script `INTEGRATION_TABLES_BDD.sql` avec l'utilisateur `hiba`

### L'historique ne s'enregistre pas

**Solution:** Exécutez `REPARER_HISTORIQUE_COMPLET.sql` et recompilez `mainwindow.cpp`

### Les boutons ne fonctionnent pas

**Solution:** Vérifiez que les `connect()` sont bien ajoutés dans `connectButtons()`

## 📞 Support

En cas de problème, vérifiez:

1. Que toutes les tables existent: `SELECT TABLE_NAME FROM USER_TABLES;`
2. Que les triggers sont actifs: `SELECT TRIGGER_NAME, STATUS FROM USER_TRIGGERS;`
3. Les logs de débogage dans Qt Creator

Bonne intégration! 🎉
