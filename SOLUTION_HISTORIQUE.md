# 🔍 Solution : Historique introuvable dans la base de données

## ❌ Problème
L'application ne trouve pas l'historique dans la base de données.

## 🔎 Causes possibles

### 1. **La table n'existe pas**
La table `HIST_RESIDENT` ou `HISTORIQUE_RESIDENT` n'a pas été créée dans la base de données.

### 2. **Mauvais schéma/utilisateur**
Le code essaie d'accéder à la table mais l'utilisateur connecté (`hiba`) n'a pas les permissions ou la table est dans un autre schéma.

### 3. **Nom de table incorrect**
Le code utilise `HIST_RESIDENT` mais la table peut s'appeler `HISTORIQUE_RESIDENT`.

### 4. **La table est vide**
La table existe mais ne contient aucun enregistrement (les triggers ne fonctionnent pas).

## ✅ Solutions

### Étape 1 : Vérifier que la table existe

Exécutez ce script SQL dans SQL Developer ou SQL*Plus (connecté en tant que `hiba`) :

```sql
-- Vérifier si la table existe
SELECT TABLE_NAME, OWNER 
FROM ALL_TABLES 
WHERE TABLE_NAME IN ('HIST_RESIDENT', 'HISTORIQUE_RESIDENT')
  AND OWNER = 'HIBA';

-- Vérifier le synonyme
SELECT SYNONYM_NAME, TABLE_OWNER, TABLE_NAME
FROM ALL_SYNONYMS
WHERE SYNONYM_NAME = 'HIST_RESIDENT'
  AND OWNER = 'HIBA';

-- Compter les enregistrements
SELECT COUNT(*) AS NOMBRE_TOTAL 
FROM HIBA.HISTORIQUE_RESIDENT;
```

**OU** exécutez le script complet : `diagnostic_historique.sql`

### Étape 2 : Créer la table si elle n'existe pas

Si la table n'existe pas, exécutez :

```sql
-- Script de création (voir create_historique_table.sql)
CREATE TABLE HIBA.HISTORIQUE_RESIDENT (
    ID_HISTORIQUE NUMBER PRIMARY KEY,
    ID_RESIDENT NUMBER NOT NULL,
    ACTION VARCHAR2(50) NOT NULL,
    DATE_ACTION DATE DEFAULT SYSDATE NOT NULL,
    CONSTRAINT FK_HIST_RESIDENT FOREIGN KEY (ID_RESIDENT) 
        REFERENCES HIBA.GEST_RESIDENT(ID)
);

-- Créer la séquence
CREATE SEQUENCE HIBA.HISTORIQUE_RESIDENT_SEQ
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

-- Créer le synonyme pour compatibilité
CREATE OR REPLACE SYNONYM HIBA.HIST_RESIDENT FOR HIBA.HISTORIQUE_RESIDENT;
```

**OU** exécutez le script : `create_historique_table.sql`

### Étape 3 : Créer les triggers automatiques

Pour que l'historique se remplisse automatiquement lors des ajouts/modifications/suppressions :

```sql
-- Voir create_trigger_historique.sql pour le script complet
```

**OU** exécutez : `create_trigger_historique.sql`

### Étape 4 : Vérifier les permissions

Assurez-vous que l'utilisateur `hiba` a les permissions :

```sql
-- Vérifier les permissions
SELECT PRIVILEGE, TABLE_NAME
FROM USER_TAB_PRIVS
WHERE TABLE_NAME IN ('HIST_RESIDENT', 'HISTORIQUE_RESIDENT');

-- Si nécessaire, donner les permissions (en tant qu'admin)
GRANT SELECT, INSERT, UPDATE, DELETE ON HIBA.HISTORIQUE_RESIDENT TO HIBA;
```

### Étape 5 : Tester manuellement

Insérez un enregistrement de test :

```sql
-- Test d'insertion
INSERT INTO HIBA.HISTORIQUE_RESIDENT (ID_RESIDENT, ACTION, DATE_ACTION)
VALUES (1, 'Ajout', SYSDATE);
COMMIT;

-- Vérifier
SELECT * FROM HIBA.HISTORIQUE_RESIDENT;
```

## 🔧 Améliorations du code

Le code a été amélioré pour :
- Essayer plusieurs noms de table (`HIST_RESIDENT`, `HIBA.HIST_RESIDENT`, `HISTORIQUE_RESIDENT`, etc.)
- Afficher des messages d'erreur plus détaillés
- Donner des instructions de diagnostic

## 📋 Checklist de vérification

- [ ] La table `HISTORIQUE_RESIDENT` existe dans le schéma `HIBA`
- [ ] Le synonyme `HIST_RESIDENT` existe et pointe vers `HISTORIQUE_RESIDENT`
- [ ] La séquence `HISTORIQUE_RESIDENT_SEQ` existe
- [ ] Les triggers automatiques sont créés et actifs
- [ ] L'utilisateur `hiba` a les permissions nécessaires
- [ ] La table contient des données (ou les triggers fonctionnent)

## 🚀 Scripts disponibles

1. **`diagnostic_historique.sql`** - Diagnostic complet
2. **`create_historique_table.sql`** - Création de la table
3. **`create_trigger_historique.sql`** - Création des triggers
4. **`test_historique.sql`** - Tests de vérification

## 💡 Après correction

Une fois la table créée et les triggers installés :
1. Redémarrez l'application
2. Ajoutez/modifiez/supprimez un résident
3. Vérifiez que l'historique se remplit automatiquement
4. Affichez l'historique dans l'application





