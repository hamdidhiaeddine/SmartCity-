# 📚 Guide d'installation de la base de données Oracle

## 🎯 Prérequis

- Oracle Database installé et fonctionnel
- SQL Developer, SQL\*Plus ou tout autre client Oracle
- Utilisateur `HIBA` créé avec les privilèges nécessaires
- Source ODBC `source_projet2A` configurée

## 📋 Étapes d'installation

### Option 1 : Avec SQL Developer (Recommandé)

1. **Ouvrir SQL Developer**

   - Lancez Oracle SQL Developer

2. **Se connecter**

   - Utilisateur : `hiba`
   - Mot de passe : `esprit18`
   - Type de connexion : Basic
   - Nom d'hôte : `localhost` (ou votre serveur Oracle)
   - Port : `1521`
   - SID ou Service : Votre SID Oracle

3. **Exécuter le script**

   - Ouvrir le fichier `create_database_complete.sql`
   - Cliquer sur le bouton **"Run Script"** (F5) ou icône de document avec flèche verte
   - Attendre la fin de l'exécution (environ 30 secondes)

4. **Vérifier les résultats**
   - Vérifier qu'il n'y a pas d'erreurs dans l'onglet "Script Output"
   - Vous devriez voir le message "BASE DE DONNÉES CRÉÉE AVEC SUCCÈS"

### Option 2 : Avec SQL\*Plus

1. **Ouvrir SQL\*Plus**

   ```cmd
   sqlplus hiba/esprit18@localhost:1521/ORCL
   ```

2. **Exécuter le script**

   ```sql
   @C:\Users\Raouf\Desktop\ayooo\create_database_complete.sql
   ```

3. **Vérifier la création**
   - Le script affiche automatiquement le nombre d'enregistrements par table
   - Vérifiez qu'aucune erreur n'est survenue

## 🗂️ Structure créée

Le script crée les tables suivantes :

### Tables principales

1. **GEST_RESIDENT** - Gestion des résidents (9 colonnes)
2. **GEST_VEHICULE** - Gestion des véhicules (8 colonnes)
3. **GEST_EMPLOYE** - Gestion des employés (7 colonnes)
4. **GEST_CABINET** - Gestion des cabinets médicaux (7 colonnes)
5. **GEST_JARDIN** - Gestion des jardins (6 colonnes)
6. **GEST_MAISON** - Gestion des maisons (6 colonnes)

### Tables d'association

7. **GEST_RESIDENT_JARDIN** - Relation résidents ↔ jardins
8. **GEST_RESIDENT_MAISON** - Relation résidents ↔ maisons

### Tables d'historique

9. **HISTORIQUE_RESIDENT** - Historique complet des actions
10. **HIST_RESIDENT** - Table alternative pour l'historique

## 🔧 Fonctionnalités automatiques

### Triggers créés :

- **TRG_HISTORIQUE_RESIDENT_ID** : Auto-incrémentation ID historique
- **TRG_HIST_RESIDENT_ID** : Auto-incrémentation ID historique (table alternative)
- **TRG_AUTO_HISTORIQUE_RESIDENT** : Enregistrement automatique des AJOUTS
- **TRG_AUTO_HISTORIQUE_RESIDENT_UPDATE** : Enregistrement automatique des MODIFICATIONS
- **TRG_AUTO_HISTORIQUE_RESIDENT_DELETE** : Enregistrement automatique des SUPPRESSIONS

### Séquences :

- **HISTORIQUE_RESIDENT_SEQ** : Séquence pour l'auto-incrémentation

### Index :

- Index sur toutes les clés étrangères
- Index sur les colonnes de recherche fréquente (NOM, EMAIL, TELEPHONE)
- Index sur les colonnes d'historique (ID_RESIDENT, ACTION, DATE_ACTION)

## 📊 Données de test insérées

Le script insère automatiquement :

- ✅ 3 résidents
- ✅ 2 véhicules
- ✅ 2 employés
- ✅ 2 cabinets
- ✅ 2 jardins
- ✅ 2 maisons
- ✅ 2 relations résident-jardin
- ✅ 2 relations résident-maison

### Résidents de test :

1. **Jean Dupont** (ID: 1)

   - Email: jean.dupont@email.com
   - Téléphone: +33123456789
   - Statut: Actif, Marié

2. **Sophie Martin** (ID: 2)

   - Email: sophie.martin@email.com
   - Téléphone: +33987654321
   - Statut: Actif, Célibataire

3. **Luc Bernard** (ID: 3)
   - Email: luc.bernard@email.com
   - Téléphone: +33567891234
   - Statut: Actif, Divorcé

## ✅ Vérification post-installation

### Vérifier les tables créées

```sql
SELECT TABLE_NAME
FROM USER_TABLES
WHERE TABLE_NAME LIKE 'GEST_%' OR TABLE_NAME LIKE 'HIST%'
ORDER BY TABLE_NAME;
```

### Vérifier les triggers

```sql
SELECT TRIGGER_NAME, STATUS
FROM USER_TRIGGERS
WHERE TRIGGER_NAME LIKE '%HISTORIQUE%'
ORDER BY TRIGGER_NAME;
```

### Vérifier les données

```sql
-- Compter les résidents
SELECT COUNT(*) FROM HIBA.GEST_RESIDENT;

-- Afficher tous les résidents
SELECT * FROM HIBA.GEST_RESIDENT;

-- Vérifier l'historique (devrait contenir 3 ajouts automatiques)
SELECT * FROM HIBA.HISTORIQUE_RESIDENT ORDER BY DATE_ACTION;
```

### Tester les triggers

```sql
-- Test d'ajout (devrait créer automatiquement une ligne dans l'historique)
INSERT INTO HIBA.GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (999, 'Test', 'Utilisateur', TO_DATE('2000-01-01', 'YYYY-MM-DD'), '1 Rue Test', '+33111111111', 'test@test.com', 'Actif', 'Célibataire');

-- Vérifier que l'historique a été créé automatiquement
SELECT * FROM HIBA.HISTORIQUE_RESIDENT WHERE ID_RESIDENT = 999;

-- Nettoyer le test
DELETE FROM HIBA.GEST_RESIDENT WHERE ID = 999;

-- Vérifier que la suppression a été enregistrée dans l'historique
SELECT * FROM HIBA.HISTORIQUE_RESIDENT WHERE ID_RESIDENT = 999 ORDER BY DATE_ACTION;
```

## 🔄 Réexécution du script

Si vous devez réexécuter le script :

- Le script supprime automatiquement toutes les tables existantes avant de les recréer
- Toutes les données seront perdues et remplacées par les données de test
- Assurez-vous de sauvegarder vos données importantes avant !

## 🐛 Dépannage

### Erreur : "Table or view does not exist"

- Vérifiez que vous êtes connecté avec l'utilisateur `HIBA`
- Vérifiez que le schéma `HIBA` existe

### Erreur : "Insufficient privileges"

- L'utilisateur `HIBA` doit avoir les droits CREATE TABLE, CREATE SEQUENCE, CREATE TRIGGER
- Connectez-vous en tant que DBA et exécutez :

```sql
GRANT CREATE TABLE TO HIBA;
GRANT CREATE SEQUENCE TO HIBA;
GRANT CREATE TRIGGER TO HIBA;
GRANT UNLIMITED TABLESPACE TO HIBA;
```

### Erreur : "ORA-00955: name is already used by an existing object"

- Une table existe déjà
- Le script devrait normalement les supprimer automatiquement
- Supprimez manuellement avec :

```sql
DROP TABLE HIBA.GEST_RESIDENT CASCADE CONSTRAINTS;
-- Répétez pour chaque table
```

### Les triggers ne fonctionnent pas

```sql
-- Vérifier le statut des triggers
SELECT TRIGGER_NAME, STATUS FROM USER_TRIGGERS;

-- Si STATUS = 'DISABLED', les activer :
ALTER TRIGGER HIBA.TRG_AUTO_HISTORIQUE_RESIDENT ENABLE;
```

## 📞 Support

En cas de problème :

1. Vérifiez les logs dans SQL Developer (onglet "Script Output")
2. Notez le message d'erreur exact
3. Vérifiez que votre instance Oracle est démarrée
4. Vérifiez que vous pouvez vous connecter avec SQL Developer

## 🎉 Prochaines étapes

Une fois la base de données créée :

1. ✅ Vérifiez que toutes les tables sont présentes
2. ✅ Testez la connexion depuis votre application Qt
3. ✅ Lancez l'application et vérifiez que les 3 résidents de test s'affichent
4. ✅ Testez les opérations CRUD
5. ✅ Vérifiez que l'historique se remplit automatiquement

**La base de données est maintenant prête pour votre application SMARTCITY ! 🏙️**
