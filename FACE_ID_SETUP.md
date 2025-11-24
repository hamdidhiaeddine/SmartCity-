# Configuration de la table Face ID

## Problème : Erreur lors de l'enregistrement du Face ID

Si vous rencontrez l'erreur "Erreur lors de l'enregistrement du Face ID", cela signifie que la table `FACE_ID` n'existe pas encore dans votre base de données Oracle.

## Solution : Créer la table FACE_ID

### Étape 1 : Ouvrir SQL*Plus ou votre outil de gestion Oracle

Connectez-vous à votre base de données Oracle avec un utilisateur ayant les privilèges nécessaires (par exemple, SYSTEM).

### Étape 2 : Exécuter le script SQL

Exécutez le fichier `create_faceid_table.sql` dans votre base de données.

**Méthode 1 : Via SQL*Plus**
```sql
@create_faceid_table.sql
```

**Méthode 2 : Copier-coller le contenu**
Ouvrez le fichier `create_faceid_table.sql` et copiez-collez son contenu dans votre outil SQL, puis exécutez-le.

**Méthode 3 : Via SQL Developer**
1. Ouvrez SQL Developer
2. Connectez-vous à votre base de données
3. Ouvrez le fichier `create_faceid_table.sql`
4. Exécutez le script (F5 ou bouton "Run Script")

### Étape 3 : Vérifier la création

Vérifiez que la table a été créée avec succès :
```sql
SELECT * FROM "SYSTEM"."FACE_ID";
```

### Étape 4 : Réessayer l'enregistrement Face ID

Une fois la table créée, vous pouvez réessayer d'enregistrer votre Face ID dans l'application.

## Structure de la table

La table `FACE_ID` contient :
- `ID` : Identifiant unique (auto-incrémenté)
- `FACE_HASH` : Hash SHA256 de l'image faciale (64 caractères)
- `DATE_CREATION` : Date de création de l'enregistrement
- `DATE_DERNIERE_UTILISATION` : Date de dernière utilisation pour l'authentification

## Notes importantes

- Un seul Face ID peut être enregistré à la fois (le système remplace l'ancien lors d'un nouvel enregistrement)
- Le hash est généré à partir de l'image complète capturée par la caméra
- Pour une meilleure reconnaissance, assurez-vous d'avoir un bon éclairage et de positionner votre visage correctement

