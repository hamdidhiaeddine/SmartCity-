# Création de la table HISTORIQUE_RESIDENT et des triggers

## 📋 Description

Ce dossier contient les scripts SQL pour créer la table d'historique et les triggers automatiques pour enregistrer les actions sur les résidents.

## 🗄️ Structure de la table

La table `HIBA.HISTORIQUE_RESIDENT` contient :
- **ID_HISTORIQUE** : Identifiant unique (clé primaire, auto-incrémenté)
- **ID_RESIDENT** : Identifiant du résident (clé étrangère vers GEST_RESIDENT)
- **ACTION** : Type d'action (Ajout, Modification, Suppression)
- **DATE_ACTION** : Date et heure de l'action (par défaut SYSDATE)

## 🚀 Installation

### Étape 1 : Créer la table

Exécutez le script `create_historique_table.sql` dans votre base de données Oracle :

```sql
@create_historique_table.sql
```

Ou copiez-collez le contenu dans SQL Developer / SQL*Plus.

### Étape 2 : Créer les triggers

Exécutez le script `create_trigger_historique.sql` :

```sql
@create_trigger_historique.sql
```

## 🔄 Fonctionnement des triggers

### Trigger d'ajout (TRG_AUTO_HISTORIQUE_RESIDENT)
- Se déclenche **après** chaque INSERT sur `GEST_RESIDENT`
- Insère automatiquement une ligne avec ACTION = 'Ajout'

### Trigger de modification (TRG_AUTO_HISTORIQUE_RESIDENT_UPDATE)
- Se déclenche **après** chaque UPDATE sur `GEST_RESIDENT`
- Insère automatiquement une ligne avec ACTION = 'Modification'

### Trigger de suppression (TRG_AUTO_HISTORIQUE_RESIDENT_DELETE)
- Se déclenche **après** chaque DELETE sur `GEST_RESIDENT`
- Insère automatiquement une ligne avec ACTION = 'Suppression'

## 📊 Vérification

Pour vérifier que tout fonctionne :

```sql
-- Voir la structure de la table
DESC HIBA.HISTORIQUE_RESIDENT;

-- Voir les triggers créés
SELECT TRIGGER_NAME, STATUS 
FROM USER_TRIGGERS 
WHERE TRIGGER_NAME LIKE 'TRG_AUTO_HISTORIQUE%';

-- Tester : Ajouter un résident et vérifier l'historique
INSERT INTO HIBA.GEST_RESIDENT (ID, NOM, PRENOM, ...) VALUES (...);
SELECT * FROM HIBA.HISTORIQUE_RESIDENT ORDER BY DATE_ACTION DESC;
```

## ⚠️ Notes importantes

1. **Permissions** : Assurez-vous d'avoir les droits nécessaires pour créer des tables et triggers
2. **Schéma** : Les scripts utilisent le schéma `HIBA`, modifiez si nécessaire
3. **Séquence** : La séquence `HISTORIQUE_RESIDENT_SEQ` est créée automatiquement
4. **Index** : Des index sont créés sur `RESIDENT_ID` et `DATE_ACTION` pour améliorer les performances

## 🔧 Dépannage

Si vous rencontrez des erreurs :

1. **Table existe déjà** : Supprimez-la d'abord avec `DROP TABLE HIBA.HISTORIQUE_RESIDENT CASCADE CONSTRAINTS;`
2. **Trigger existe déjà** : Les scripts utilisent `CREATE OR REPLACE`, donc ils remplaceront les anciens triggers
3. **Permissions insuffisantes** : Contactez votre administrateur de base de données

