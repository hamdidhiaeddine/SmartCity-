# 🏗️ ARCHITECTURE DU PROJET SMARTCITY INTÉGRÉ

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        APPLICATION SMARTCITY                             │
│                          (Qt 6.7.3 C++)                                  │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
                ┌───────────────────┼───────────────────┐
                │                   │                   │
         ┌──────▼─────┐      ┌─────▼──────┐     ┌─────▼──────┐
         │ MainWindow │      │ Connection │     │  UI Forms  │
         │   (MVC)    │      │  (Oracle)  │     │ (.ui files)│
         └────────────┘      └────────────┘     └────────────┘
                │                   │
       ┌────────┼────────┬──────────┼──────────┬────────┐
       │        │        │          │          │        │
┌──────▼──┐ ┌──▼────┐ ┌─▼──────┐ ┌─▼─────┐ ┌──▼───┐ ┌─▼───────┐
│Employee │ │Resident│ │Vehicule│ │SMS    │ │Histo-│ │Alertes  │
│  Class  │ │ Class  │ │ Class  │ │Manager│ │rique │ │ (futur) │
└─────────┘ └────────┘ └────────┘ └───────┘ └──────┘ └─────────┘
     │          │          │          │         │          │
     └──────────┴──────────┴──────────┴─────────┴──────────┘
                            │
                ┌───────────▼────────────┐
                │  BASE DE DONNÉES       │
                │  Oracle 11g XE         │
                │  Schéma: HIBA          │
                │  User: hiba/esprit18   │
                └────────────────────────┘
                            │
    ┌───────────────────────┼───────────────────────┐
    │                       │                       │
┌───▼───────┐     ┌─────────▼──────┐    ┌─────────▼────────┐
│ EMPLOYES  │     │ GEST_RESIDENT  │    │    VEHICULE      │
│ 2 lignes  │     │ 23 lignes      │    │    2 lignes      │
├───────────┤     ├────────────────┤    ├──────────────────┤
│ID_EMPLOYE │     │ID              │    │ID_VEHI           │
│NOM        │     │NOM             │    │IMMAT             │
│PRENOM     │     │PRENOM          │    │MARQUE            │
│EMAIL      │     │DATENAISS       │    │MODELE            │
│POSTE      │     │ADRESSE         │    │TYPE              │
│SALAIRE    │     │TELEPHONE       │    │ETAT              │
│ADRESSE    │     │EMAIL           │    │SERVICE           │
│TELEPHONE  │     │STATUT          │    │DATE_MAINT        │
│ID_RES(FK) │     │SITUATIONFAM.   │    └──────────────────┘
└───────────┘     └────────────────┘             │
                          │                      │
                          │                  ┌───▼────────────┐
                  ┌───────▼───────┐          │  SEQ_VEHI      │
                  │HISTORIQUE_RES │          │  (Sequence)    │
                  │ + triggers    │          └────────────────┘
                  ├───────────────┤
                  │ID_HISTORIQUE  │
                  │ID_RESIDENT(FK)│
                  │ACTION         │
                  │DATE_ACTION    │
                  └───────────────┘
                          │
            ┌─────────────┼─────────────┐
            │             │             │
    ┌───────▼──────┐ ┌────▼─────┐ ┌────▼─────┐
    │ TRG_INSERT   │ │TRG_UPDATE│ │Manuel en │
    │   "Ajout"    │ │"Modifica-│ │ C++ pour │
    │              │ │  tion"   │ │ DELETE   │
    └──────────────┘ └──────────┘ └──────────┘

┌────────────────────────────────────────────────────────────┐
│              TABLES SUPPLÉMENTAIRES CRÉÉES                 │
├────────────────────────────────────────────────────────────┤
│ GEST_MAISON      │ GEST_ALERTES                            │
│ 2 lignes         │ 2 lignes                                │
├──────────────────┼─────────────────────────────────────────┤
│ ID               │ ID (auto via trigger)                   │
│ ADRESSE          │ ID_MAISON (FK)                          │
│ SECURITE         │ ZONE                                    │
│ STATUS           │ NIVEAU                                  │
│ TYPE             │ STATUT                                  │
│ NBRDESPIECES     │ DATE_ALERTE                             │
└──────────────────┴─────────────────────────────────────────┘
```

## 📊 FLUX DE DONNÉES

### 1. Ajout d'un Employé

```
[UI] Formulaire rempli
  │
  ▼
[MainWindow::onAjouterEmploye()]
  │
  ├─ Validation des champs
  │
  ▼
[Employee::insert()]
  │
  ├─ Génération ID si besoin
  ├─ Vérification unicité
  ├─ Préparation requête SQL
  │
  ▼
[Oracle] INSERT INTO EMPLOYES
  │
  ▼
[Commit automatique]
  │
  ▼
[UI] Rafraîchissement tableau
```

### 2. Ajout d'un Résident avec Historique

```
[UI] Formulaire rempli
  │
  ▼
[MainWindow::onAjouterResident()]
  │
  ├─ Validation
  │
  ▼
[Resident::ajouter()]
  │
  ▼
[Oracle] INSERT INTO GEST_RESIDENT
  │
  ▼
[Trigger] TRG_AUTO_HISTORIQUE_RESIDENT
  │
  ├─ Détecte INSERT
  ├─ Génère ID via séquence
  │
  ▼
[Oracle] INSERT INTO HISTORIQUE_RESIDENT
         (ID_RESIDENT, ACTION='Ajout', DATE=NOW)
  │
  ▼
[Commit]
  │
  ▼
[UI] Rafraîchissement
```

### 3. Suppression d'un Résident

```
[UI] Sélection + clic Supprimer
  │
  ▼
[MainWindow::onSupprimerResident()]
  │
  ├─ Confirmation
  │
  ▼
[Historique::ajouter()]
  │   (Enregistrement MANUEL)
  ├─ INSERT INTO HISTORIQUE_RESIDENT
  │   (ACTION='Suppression')
  │
  ▼
[Resident::supprimer()]
  │
  ▼
[Oracle] DELETE FROM GEST_RESIDENT
  │
  ▼
[Commit]
  │
  ▼
[UI] Rafraîchissement
```

## 🔄 ARCHITECTURE MVC

```
┌─────────────────────────────────────────────┐
│              VIEW (Interface UI)            │
│  - mainwindow.ui (Qt Designer)              │
│  - Widgets: QTableWidget, QLineEdit, etc.   │
└────────────┬────────────────────────────────┘
             │ Signals & Slots
             │
┌────────────▼────────────────────────────────┐
│         CONTROLLER (MainWindow)             │
│  - Gestion des événements utilisateur       │
│  - Validation des données                   │
│  - Appels aux modèles                       │
│  - Mise à jour de la vue                    │
└────────────┬────────────────────────────────┘
             │ Appels méthodes
             │
┌────────────▼────────────────────────────────┐
│        MODEL (Classes métier)               │
│  - Employee  : CRUD employés                │
│  - Resident  : CRUD résidents               │
│  - Vehicule  : CRUD véhicules               │
│  - Historique: Traçabilité                  │
└────────────┬────────────────────────────────┘
             │ Requêtes SQL (QSqlQuery)
             │
┌────────────▼────────────────────────────────┐
│       DATABASE (Oracle 11g XE)              │
│  - Tables relationnelles                    │
│  - Triggers automatiques                    │
│  - Contraintes d'intégrité                  │
└─────────────────────────────────────────────┘
```

## 🗂️ STRUCTURE DES FICHIERS

```
C:\ayooo\
│
├── 📁 Sources C++
│   ├── main.cpp                      (Point d'entrée)
│   ├── mainwindow.cpp/.h/.ui         (Contrôleur principal)
│   ├── connection.cpp/.h             (Gestion BD)
│   ├── resident.cpp/.h               (Modèle Resident)
│   ├── Employee.cpp/.h               (Modèle Employee) ✨ NOUVEAU
│   ├── vehicule.cpp/.h               (Modèle Vehicule) ✨ NOUVEAU
│   ├── historique.cpp/.h             (Modèle Historique)
│   ├── smsmanager.cpp/.h             (SMS Twilio)
│   └── smsreceiver.cpp/.h            (Webhook SMS)
│
├── 📁 Scripts SQL
│   ├── INTEGRATION_TABLES_BDD.sql    ✨ NOUVEAU (Tables intégration)
│   ├── REPARER_HISTORIQUE_COMPLET.sql (Réparation historique)
│   ├── VERIFIER_HISTORIQUE_COMPLET.sql (Vérification)
│   └── create_database_complete.sql   (Création initiale)
│
├── 📁 Documentation
│   ├── README_INTEGRATION.md         ✨ NOUVEAU (Guide complet)
│   ├── SYNTHESE_INTEGRATION.md       ✨ NOUVEAU (Vue d'ensemble)
│   ├── CHECKLIST_INTEGRATION.md      ✨ NOUVEAU (Checklist)
│   ├── ARCHITECTURE_PROJET.md        ✨ NOUVEAU (Ce fichier)
│   ├── README_HISTORIQUE.md          (Doc historique)
│   └── README_SMS.md                 (Doc SMS)
│
├── 📁 Configuration
│   ├── projetsmartcity.pro           ✨ MODIFIÉ (Ajout fichiers)
│   ├── projetsmartcity.pro.user      (Config Qt Creator)
│   └── resources.qrc                 (Ressources)
│
├── 📁 Scripts Batch
│   ├── EXECUTER_INTEGRATION_BDD.bat  ✨ NOUVEAU
│   ├── Lancer_Application.bat        (Lancement app)
│   └── CONFIGURER_RUN_QT_CREATOR.bat (Config Qt)
│
└── 📁 Build
    └── Desktop_Qt_6_7_3_MinGW_64_bit-Debug/
        ├── Makefile
        └── debug/Atelier_Connexion.exe
```

## 🔐 SÉCURITÉ ET CONTRAINTES

### Contraintes d'intégrité

```sql
-- EMPLOYES
PK: ID_EMPLOYE
FK: ID_RES → GEST_RESIDENT.ID (optionnel, NULL autorisé)

-- GEST_RESIDENT
PK: ID
UNIQUE: TELEPHONE, EMAIL

-- HISTORIQUE_RESIDENT
PK: ID_HISTORIQUE
FK: ID_RESIDENT → GEST_RESIDENT.ID (ON DELETE désactivé)

-- VEHICULE
PK: ID_VEHI
UNIQUE: IMMAT

-- GEST_ALERTES
PK: ID
FK: ID_MAISON → GEST_MAISON.ID (NOT NULL)

-- GEST_MAISON
PK: ID
```

### Triggers actifs

```
TRG_AUTO_HISTORIQUE_RESIDENT → AFTER INSERT on GEST_RESIDENT
TRG_AUTO_HIST_RES_UPDATE     → AFTER UPDATE on GEST_RESIDENT
GEST_ALERTES_TRG             → BEFORE INSERT on GEST_ALERTES (auto-ID)
```

### Séquences

```
SEQ_VEHI                → Véhicules
HISTORIQUE_RESIDENT_SEQ → Historique résidents
GEST_ALERTES_SEQ        → Alertes
```

## 🚀 EXTENSIONS FUTURES

### Modules prêts à ajouter

```
┌─────────────┐
│ MAISONS     │ Tables créées ✅
│ - CRUD      │ Code disponible dans projet intégration
│ - Statistiq.│
└─────────────┘

┌─────────────┐
│ ALERTES     │ Tables créées ✅
│ - Gestion   │ Code disponible
│ - Carte     │
│ - Statut    │
└─────────────┘

┌─────────────┐
│ JARDINS     │ À créer
│ - CRUD      │ Code disponible
│ - Planning  │
└─────────────┘

┌─────────────┐
│ CHATBOT AI  │ Code disponible (Azure)
│ - Recommand.│
│ - Assistance│
└─────────────┘
```

## 💡 POINTS CLÉS DE L'ARCHITECTURE

### ✅ Points forts

1. **Séparation des responsabilités** : MVC respecté
2. **Réutilisabilité** : Classes métier indépendantes
3. **Robustesse** : Gestion d'erreurs complète
4. **Traçabilité** : Historique automatique via triggers
5. **Maintenabilité** : Code bien documenté
6. **Extensibilité** : Facile d'ajouter de nouveaux modules

### ⚠️ Points d'attention

1. **Schéma unique HIBA** : Tous les modules utilisent le même utilisateur
2. **Triggers limités** : DELETE manuel (problème mutating table)
3. **ODBC Oracle** : Nécessite driver Oracle installé
4. **Taille colonnes** : VARCHAR2(20) pour employés (limitation)
5. **Séquences** : Gestion manuelle de l'auto-increment

## 📈 STATISTIQUES DU PROJET

```
Lignes de code C++:         ~3000 lignes
Fichiers sources (.cpp):     8 fichiers
Fichiers headers (.h):       8 fichiers
Tables Oracle:               6 tables
Triggers:                    3 triggers
Séquences:                   3 séquences
Pages UI (stackedWidget):    6 pages
Boutons CRUD:                ~20 boutons
Modules fonctionnels:        3 (Résidents, Employés, Véhicules)
Modules prêts:               3 (Maisons, Alertes, Jardins)
```

---

**Date**: 1er décembre 2025  
**Version**: 1.0  
**Framework**: Qt 6.7.3  
**Base de données**: Oracle 11g XE  
**Compilateur**: MinGW 64-bit
