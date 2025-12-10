# ✅ CHECKLIST D'INTÉGRATION - SMARTCITY

## 📁 FICHIERS CRÉÉS

### Scripts SQL

- [x] `INTEGRATION_TABLES_BDD.sql` - Script de création des tables
- [x] `EXECUTER_INTEGRATION_BDD.bat` - Script batch d'exécution

### Classes C++

- [x] `Employee.h` - Déclarations classe Employee
- [x] `Employee.cpp` - Implémentation CRUD Employee
- [x] `vehicule.h` - Déclarations classe Vehicule
- [x] `vehicule.cpp` - Implémentation CRUD Vehicule

### Documentation

- [x] `README_INTEGRATION.md` - Guide d'intégration complet
- [x] `SYNTHESE_INTEGRATION.md` - Synthèse du projet
- [x] `CHECKLIST_INTEGRATION.md` - Ce fichier

### Fichiers modifiés

- [x] `projetsmartcity.pro` - Ajout des nouveaux fichiers sources
- [x] `mainwindow.h` - Ajout des includes Employee.h et vehicule.h

## 🎯 NOMS DES WIDGETS UI (mainwindow.ui)

### Page Employés

```
QTableWidget: ui->tableau
Boutons CRUD:
- ui->ajouter      (Ajouter)
- ui->modifier     (Modifier)
- ui->supprimer    (Supprimer)
- ui->exporter     (Exporter PDF)

Champs de saisie:
- ui->idline           (ID)
- ui->nomline          (Nom)
- ui->prenomline       (Prénom)
- ui->emailline        (Email)
- ui->posteline        (Poste)
- ui->salaireline      (Salaire)
- ui->adresseline      (Adresse)
- ui->telephoneline    (Téléphone)

Recherche/Tri:
- ui->recherche        (Recherche)
- ui->triemail         (ComboBox Email)
- ui->trisalaire       (ComboBox Salaire)
```

### Page Véhicules

```
QTableWidget: ui->tableau_3
Boutons CRUD:
- ui->ajouter_3    (Ajouter)
- ui->modifier_3   (Modifier)
- ui->supprimer_3  (Supprimer)
- ui->exporter_3   (Exporter PDF)

Champs de saisie:
- ui->immatline_2      (Immatriculation)
- ui->marqueline_2     (Marque)
- ui->modeleline_2     (Modèle)
- ui->triemail_2       (ComboBox Type)
- ui->Etatline_2       (ComboBox État)
- ui->serviceline_2    (Service)
- ui->datemaintline_2  (QDateEdit Date maintenance)

Recherche/Tri:
- ui->recherche_2      (Recherche)
- ui->tribox           (ComboBox Tri)
```

## 🔧 CODE À AJOUTER DANS mainwindow.cpp

### ✅ Slot: Chargement des employés

```cpp
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
```

### ✅ Slot: Chargement des véhicules

```cpp
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
```

### ✅ Slots de navigation

```cpp
void MainWindow::onGestionEmployes()
{
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
    ui->stackedWidget_2->setCurrentWidget(ui->page_4);
    chargerEmployes();
}

void MainWindow::onGestionVehicules()
{
    ui->stackedWidget->setCurrentWidget(ui->pageVehicules);
    ui->stackedWidget_3->setCurrentWidget(ui->page_9);
    chargerVehicules();
}
```

### ✅ Connexion des boutons (dans connectButtons())

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

## 📝 DÉCLARATIONS À AJOUTER DANS mainwindow.h

### Dans `private slots:`

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

### Dans `private:`

```cpp
void reinitialiserFormulaireEmploye();
void reinitialiserFormulaireVehicule();
```

## 🗃️ TABLES CRÉÉES DANS ORACLE

### EMPLOYES

```sql
ID_EMPLOYE (NUMBER, PK)
NOM (VARCHAR2(20))
PRENOM (VARCHAR2(20))
EMAIL (VARCHAR2(20))
POSTE (VARCHAR2(20))
SALAIRE (NUMBER)
ADRESSE (VARCHAR2(20))
TELEPHONE (NUMBER)
ID_RES (NUMBER, FK optionnel)
```

### VEHICULE

```sql
ID_VEHI (NUMBER, PK) -- Auto-increment via SEQ_VEHI
IMMAT (VARCHAR2(50))
MARQUE (VARCHAR2(50))
MODELE (VARCHAR2(50))
TYPE (VARCHAR2(50))
ETAT (VARCHAR2(50))
SERVICE (VARCHAR2(50))
DATE_MAINT (DATE)
```

### GEST_MAISON

```sql
ID (NUMBER, PK)
ADRESSE (VARCHAR2(200))
SECURITE (NUMBER)
STATUS (VARCHAR2(50))
TYPE (VARCHAR2(50))
NBRDESPIECES (NUMBER)
```

### GEST_ALERTES

```sql
ID (NUMBER, PK) -- Auto-increment via trigger
ID_MAISON (NUMBER, NOT NULL)
ZONE (VARCHAR2(200))
NIVEAU (NUMBER)
STATUT (VARCHAR2(50)) -- Default 'En attente'
DATE_ALERTE (DATE) -- Default SYSDATE
```

## ✅ COMMANDES DE VÉRIFICATION

### Vérifier les tables créées

```sql
SELECT TABLE_NAME FROM USER_TABLES
WHERE TABLE_NAME IN ('EMPLOYES', 'VEHICULE', 'GEST_MAISON', 'GEST_ALERTES', 'GEST_RESIDENT', 'HISTORIQUE_RESIDENT');
```

### Vérifier les séquences

```sql
SELECT SEQUENCE_NAME FROM USER_SEQUENCES
WHERE SEQUENCE_NAME IN ('SEQ_VEHI', 'GEST_ALERTES_SEQ', 'HISTORIQUE_RESIDENT_SEQ');
```

### Vérifier les triggers

```sql
SELECT TRIGGER_NAME, STATUS, TABLE_NAME
FROM USER_TRIGGERS
WHERE TABLE_NAME IN ('GEST_RESIDENT', 'GEST_ALERTES')
ORDER BY TABLE_NAME, TRIGGER_NAME;
```

### Compter les enregistrements

```sql
SELECT 'EMPLOYES' AS TABLE_NAME, COUNT(*) AS NB FROM EMPLOYES
UNION ALL SELECT 'VEHICULE', COUNT(*) FROM VEHICULE
UNION ALL SELECT 'GEST_MAISON', COUNT(*) FROM GEST_MAISON
UNION ALL SELECT 'GEST_ALERTES', COUNT(*) FROM GEST_ALERTES;
```

## 🚀 ÉTAPES D'EXÉCUTION

1. **Créer les tables** (5 min)

   ```
   Double-cliquer sur: EXECUTER_INTEGRATION_BDD.bat
   ```

2. **Modifier mainwindow.h** (5 min)

   - Ajouter les déclarations de slots
   - Ajouter les déclarations de méthodes privées

3. **Modifier mainwindow.cpp** (15 min)

   - Ajouter les implémentations (voir README_INTEGRATION.md)
   - Connecter les boutons

4. **Compiler** (5 min)

   ```powershell
   cd C:\ayooo
   qmake projetsmartcity.pro
   mingw32-make
   ```

5. **Tester** (10 min)
   - Lancer l'application
   - Tester module Employés
   - Tester module Véhicules
   - Tester module Résidents + Historique

## 🎯 RÉSULTAT ATTENDU

### Avant intégration

- ✅ Module Résidents avec historique
- ✅ SMS Twilio
- ❌ Employés (UI existe mais pas de code)
- ❌ Véhicules (UI existe mais pas de code)

### Après intégration

- ✅ Module Résidents avec historique
- ✅ SMS Twilio
- ✅ Module Employés complet
- ✅ Module Véhicules complet
- ✅ Base de données unifiée (schéma HIBA)
- ✅ Prêt pour ajout Maisons/Alertes/Jardins

## 📊 TEMPS ESTIMÉ TOTAL

| Tâche                       | Temps      |
| --------------------------- | ---------- |
| Exécution script SQL        | 5 min      |
| Modification mainwindow.h   | 5 min      |
| Modification mainwindow.cpp | 15 min     |
| Compilation                 | 5 min      |
| Tests                       | 10 min     |
| **TOTAL**                   | **40 min** |

## 🆘 EN CAS DE PROBLÈME

1. **Erreur compilation "Employee.h not found"**
   → Vérifier que Employee.h est dans C:\ayooo
   → Vérifier projetsmartcity.pro contient Employee.cpp et Employee.h

2. **Erreur "table or view does not exist"**
   → Exécuter INTEGRATION_TABLES_BDD.sql avec utilisateur hiba

3. **Historique ne fonctionne pas**
   → Exécuter REPARER_HISTORIQUE_COMPLET.sql
   → Vérifier triggers: SELECT \* FROM USER_TRIGGERS WHERE TABLE_NAME='GEST_RESIDENT'

4. **Boutons ne font rien**
   → Vérifier les connect() dans connectButtons()
   → Vérifier les noms de widgets dans mainwindow.ui

## 📞 CONTACT

Pour toute question sur l'intégration:

- Consulter README_INTEGRATION.md (guide détaillé)
- Consulter SYNTHESE_INTEGRATION.md (vue d'ensemble)
- Consulter ce fichier CHECKLIST_INTEGRATION.md (référence rapide)

---

**Status actuel**: ✅ Prêt pour intégration  
**Dernière mise à jour**: 1er décembre 2025
