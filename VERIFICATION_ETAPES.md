# Guide de vérification étape par étape

## Problème : La table n'existe toujours pas

Si vous obtenez toujours l'erreur `ORA-00942`, suivez ces étapes :

## Méthode recommandée : Exécution ligne par ligne

### Dans SQL Developer :

1. **Ouvrez le fichier `create_faceid_step_by_step.sql`**

2. **ÉTAPE 1 - Créer la table :**
   - Sélectionnez UNIQUEMENT la commande `CREATE TABLE` (lignes 7-12)
   - Appuyez sur **F9** (Execute Statement) ou cliquez sur le bouton "Execute Statement"
   - **Vérifiez le résultat** : Vous devriez voir "Table created" dans l'onglet "Résultat de requête"
   - Si vous voyez une erreur, notez-la

3. **ÉTAPE 2 - Vérifier :**
   - Exécutez la requête de vérification (ligne 15)
   - Elle doit retourner `1` si la table existe

4. **ÉTAPE 3 - Créer la séquence :**
   - Sélectionnez la commande `CREATE SEQUENCE` (lignes 18-21)
   - Appuyez sur **F9**
   - Vérifiez : "Sequence created"

5. **ÉTAPE 4 - Créer le trigger :**
   - Sélectionnez TOUT le bloc `CREATE OR REPLACE TRIGGER...END;` (lignes 24-31)
   - Appuyez sur **F9**
   - Vérifiez : "Trigger created"

6. **ÉTAPE 5 - Test final :**
   - Exécutez `SELECT * FROM "SYSTEM"."FACE_ID";`
   - Cette fois, ça doit fonctionner !

## Vérifications importantes

### Vérifier votre connexion :
```sql
SELECT USER FROM DUAL;
```
Cela doit retourner l'utilisateur connecté (probablement "SYSTEM" ou "dhiaeddine")

### Vérifier les privilèges :
Si vous obtenez "insufficient privileges", vous devez être connecté avec l'utilisateur SYSTEM.

### Vérifier si la table existe déjà ailleurs :
```sql
SELECT OWNER, TABLE_NAME 
FROM ALL_TABLES 
WHERE TABLE_NAME = 'FACE_ID';
```

## Si ça ne fonctionne toujours pas

1. **Vérifiez les erreurs** dans l'onglet "Messages - Journal"
2. **Copiez l'erreur exacte** et partagez-la
3. **Essayez de créer la table sans le schéma SYSTEM** (utilisez `create_faceid_table_user.sql`)

