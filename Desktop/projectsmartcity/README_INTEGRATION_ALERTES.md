# Intégration de la Gestion des Maisons et Alertes

## 📋 Résumé de l'intégration

Cette intégration ajoute la fonctionnalité de gestion des alertes au projet SmartCity en complément de la gestion des maisons existante.

## ✅ Fichiers ajoutés

### 1. Classes Alerte
- **alerte.h** : Définition de la classe Alerte
- **alerte.cpp** : Implémentation CRUD pour les alertes

### 2. Modifications apportées

#### projetsmartcity.pro
- Ajout de `alerte.cpp` dans la section SOURCES
- Ajout de `alerte.h` dans la section HEADERS

#### mainwindow.h
- Ajout de `#include "alerte.h"`
- Nouvelles méthodes pour la gestion des alertes :
  - `chargerAlertes()`
  - `onGestionAlertes()`
  - `onAjouterAlerte()`
  - `onModifierAlerte()`
  - `onSupprimerAlerte()`
  - `onAlerteSelectionChanged()`
  - `onMarquerAlerteTraitee()`
  - `onRetourAlertes()`
  - `reinitialiserFormulaireAlerte()`
  - `connectAlerteButtons()`
  - `refreshAlertes()`

#### mainwindow.cpp
- Ajout des includes SQL nécessaires (`QSqlQuery`, `QSqlDatabase`, `QSqlError`)
- Appel de `connectAlerteButtons()` dans le constructeur
- Implémentation complète des méthodes CRUD pour les alertes
- **Création automatique d'alertes** lors de l'ajout/modification d'une maison avec niveau de sécurité < 3

## 🔧 Fonctionnalités

### Gestion des Alertes
1. **Affichage** : Liste toutes les alertes avec leurs détails (ID, ID_MAISON, ZONE, NIVEAU, STATUT, DATE_ALERTE)
2. **Ajout** : Permet d'ajouter une nouvelle alerte manuellement
3. **Modification** : Modification des alertes existantes
4. **Suppression** : Suppression d'alertes
5. **Marquer comme traitée** : Mise à jour du statut d'une alerte

### Intégration avec les Maisons
- **Alerte automatique** : Lorsqu'une maison est ajoutée ou modifiée avec un niveau de sécurité < 3, une alerte est automatiquement créée
- **Vérification** : Le système vérifie qu'il n'existe pas déjà d'alerte active avant d'en créer une nouvelle

## 🗄️ Structure de la table GEST_ALERTES

```sql
CREATE TABLE GEST_ALERTES (
    ID NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    ID_MAISON NUMBER NOT NULL,
    ZONE VARCHAR2(255),
    NIVEAU NUMBER,
    STATUT VARCHAR2(50),
    DATE_ALERTE DATE
);
```

## 📝 Utilisation dans l'interface

### Pour que les boutons et les formulaires fonctionnent, votre fichier UI doit contenir :

#### Widgets nécessaires (dans l'UI Designer) :
- `tableau_10` : QTableWidget pour afficher les alertes
- `lineEditIdMaisonAlerte` : QLineEdit pour l'ID de la maison
- `lineEditZoneAlerte` : QLineEdit pour la zone
- `spinBoxNiveauAlerte` : QSpinBox pour le niveau (1-5)
- `comboBoxStatutAlerte` : QComboBox pour le statut (En attente, Traitée, etc.)

#### Boutons :
- `btnAjouterAlerte` : Ajouter une alerte
- `btnModifierAlerte` : Modifier l'alerte sélectionnée
- `btnSupprimerAlerte` : Supprimer l'alerte sélectionnée
- `btnMarquerTraitee` : Marquer comme traitée
- `btnRetourAlertes` : Retour à la page précédente
- `btnAlertes` : Accéder à la page de gestion des alertes (depuis le menu principal)

### Exemple d'utilisation du code :

```cpp
// Créer une alerte manuellement
Alerte alerte(1, "Rue de la Paix", 2, "En attente");
if (alerte.ajouter()) {
    qDebug() << "Alerte ajoutée avec succès";
}

// Marquer une alerte comme traitée
Alerte::marquerCommeTraitee(5);

// Vérifier si une maison a déjà une alerte active
if (Alerte::existeAlertePourMaison(1)) {
    qDebug() << "La maison a déjà une alerte active";
}
```

## 🔍 Points importants

1. **Connexion à la base de données** : Les alertes utilisent la connexion "qt_oracle"
2. **ID automatique** : L'ID des alertes est généré automatiquement (IDENTITY)
3. **Date automatique** : La date de l'alerte est initialisée à la date actuelle
4. **Intégration maison** : Les alertes sont automatiquement créées pour les maisons à risque

## 📌 Prochaines étapes possibles

- [ ] Ajouter la fonctionnalité de carte pour localiser les alertes (déjà implémentée dans le projet mariem)
- [ ] Créer des statistiques sur les alertes
- [ ] Ajouter des notifications en temps réel pour les nouvelles alertes
- [ ] Exporter les alertes au format PDF

## 🚀 Comment tester

1. Compiler le projet avec Qt Creator
2. Vérifier que la table GEST_ALERTES existe dans Oracle
3. Ajouter une maison avec un niveau de sécurité < 3
4. Vérifier qu'une alerte est automatiquement créée
5. Tester les opérations CRUD sur les alertes

## ⚠️ Notes

- Si vous avez déjà intégré l'UI dans Qt Designer, les méthodes connectées aux boutons fonctionneront automatiquement
- Si certains widgets n'existent pas dans votre UI, le code vérifie leur existence avec des conditions (`if (ui->widget)`) avant de les utiliser
- Les alertes sont liées aux maisons via le champ `ID_MAISON`
