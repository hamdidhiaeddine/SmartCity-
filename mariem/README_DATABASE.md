# Guide d'Installation de la Base de Données Smart City

## 📋 Prérequis

- Oracle Database XE (Express Edition) installé
- SQL*Plus ou SQL Developer
- Utilisateur Oracle: **mariem** / Mot de passe: **test1234**

## 🚀 Installation

### Méthode 1: Via SQL*Plus (Ligne de commande)

1. **Ouvrir une invite de commandes**
2. **Se connecter à Oracle:**
   ```cmd
   sqlplus mariem/test1234@XE
   ```

3. **Exécuter le script:**
   ```sql
   @database_setup.sql
   ```
   OU
   ```sql
   START database_setup.sql
   ```

### Méthode 2: Via SQL Developer (Interface graphique)

1. Ouvrir **Oracle SQL Developer**
2. Créer une connexion:
   - Nom: Smart City
   - Utilisateur: mariem
   - Mot de passe: test1234
   - Nom du service: XE
3. Ouvrir le fichier `database_setup.sql`
4. Cliquer sur **Exécuter le script** (F5)

## 📊 Structure de la Base de Données

### Tables Créées

#### 1. **GEST_MAISON**
| Colonne | Type | Description |
|---------|------|-------------|
| ID | NUMBER (PK) | Identifiant unique |
| ASRESSE | VARCHAR2(255) | Adresse de la maison |
| SECURITE | NUMBER(1-5) | Niveau de sécurité |
| STATUS | VARCHAR2(50) | Statut (Actif, Inactif, etc.) |
| TYPE | VARCHAR2(100) | Type de maison |
| NBRDESPIECES | NUMBER | Nombre de pièces |
| DATE_CREATION | DATE | Date de création |

#### 2. **GEST_ALERTES**
| Colonne | Type | Description |
|---------|------|-------------|
| ID | NUMBER (PK) | Identifiant unique |
| ID_MAISON | NUMBER (FK) | Référence vers GEST_MAISON |
| ZONE | VARCHAR2(200) | Zone géographique |
| NIVEAU | NUMBER(1-5) | Gravité de l'alerte |
| STATUT | VARCHAR2(50) | État de l'alerte |
| DATE_ALERTE | DATE | Date de l'alerte |
| DESCRIPTION | VARCHAR2(500) | Description détaillée |

### Séquences
- **SEQ_MAISON** - Auto-incrémentation des IDs de maisons
- **SEQ_ALERTES** - Auto-incrémentation des IDs d'alertes

### Vues
- **V_MAISONS_ALERTES** - Maisons avec compteur d'alertes
- **V_ALERTES_ACTIVES** - Alertes actives avec détails

### Procédures Stockées
- **TRAITER_ALERTE(id)** - Marquer une alerte comme traitée
- **CREER_ALERTE(id_maison, zone, niveau, description)** - Créer une nouvelle alerte

## 🔍 Vérification de l'Installation

Après exécution du script, vérifiez:

```sql
-- Vérifier les tables
SELECT table_name FROM user_tables WHERE table_name LIKE 'GEST_%';

-- Compter les maisons
SELECT COUNT(*) FROM GEST_MAISON;

-- Compter les alertes
SELECT COUNT(*) FROM GEST_ALERTES;

-- Voir toutes les maisons
SELECT * FROM GEST_MAISON;

-- Voir toutes les alertes actives
SELECT * FROM V_ALERTES_ACTIVES;
```

## 📈 Données de Test Incluses

Le script insère automatiquement:
- ✅ **8 maisons** dans différentes villes tunisiennes
- ✅ **6 alertes** avec différents niveaux de gravité
- ✅ Données réalistes pour tester l'application

## 🛠️ Maintenance

### Réinitialiser la base
```sql
@database_setup.sql
```
Le script supprime et recrée automatiquement toutes les tables.

### Ajouter une maison manuellement
```sql
INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES)
VALUES (SEQ_MAISON.NEXTVAL, 'Votre adresse', 3, 'Actif', 'Villa', 5);
COMMIT;
```

### Ajouter une alerte manuellement
```sql
EXEC CREER_ALERTE(1, 'Tunis Centre', 3, 'Description de l''alerte');
```

## ⚠️ Notes Importantes

1. **Nom de colonne**: La colonne s'appelle `ASRESSE` (avec faute) pour correspondre au code Qt existant
2. **Séquences**: Les IDs sont générés automatiquement via triggers
3. **Contraintes**: Foreign key avec `ON DELETE CASCADE` (suppression en cascade)
4. **Index**: Créés sur les colonnes fréquemment recherchées pour optimiser les performances

## 🔗 Connexion depuis Qt

L'application Qt se connecte automatiquement via:
- Driver: QODBC
- DSN: Oracle in XE
- Utilisateur: mariem
- Mot de passe: test1234
- Connexion nommée: "qt_oracle"

## 📞 Résolution de Problèmes

### Erreur "Table already exists"
Le script supprime automatiquement les tables existantes. Si erreur persiste:
```sql
DROP TABLE GEST_ALERTES CASCADE CONSTRAINTS;
DROP TABLE GEST_MAISON CASCADE CONSTRAINTS;
```

### Erreur de connexion
Vérifier que Oracle XE est démarré:
```cmd
net start OracleServiceXE
```

### Permissions insuffisantes
Connectez-vous en tant que SYSTEM et donnez les droits:
```sql
GRANT CREATE TABLE, CREATE SEQUENCE, CREATE TRIGGER, CREATE VIEW, CREATE PROCEDURE TO mariem;
```

---

**✅ Installation terminée!** Vous pouvez maintenant lancer l'application Qt Smart City.
