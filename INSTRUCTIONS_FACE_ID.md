# Instructions pour créer la table FACE_ID

## Méthode rapide (recommandée)

### Dans SQL Developer :

1. **Ouvrez le fichier `create_faceid_table.sql`** dans SQL Developer

2. **Sélectionnez TOUT le contenu** (Ctrl+A)

3. **Copiez** (Ctrl+C)

4. **Dans SQL Developer, dans la feuille de calcul :**
   - Collez le contenu (Ctrl+V)
   - Cliquez sur le bouton **"Run Script"** (F5) ou utilisez le menu "Run" → "Run Script"

5. **Vérifiez les résultats :**
   - Regardez l'onglet "Messages - Journal" en bas
   - Vous devriez voir "Table created", "Sequence created", "Trigger created"
   - S'il y a des erreurs, elles seront affichées ici

6. **Testez la création :**
   ```sql
   SELECT * FROM "SYSTEM"."FACE_ID";
   ```
   Cette requête devrait s'exécuter sans erreur (même si la table est vide)

## Si vous obtenez une erreur de privilèges

Si vous obtenez une erreur comme "insufficient privileges" ou "ORA-01031", cela signifie que votre utilisateur n'a pas les droits pour créer des objets dans le schéma SYSTEM.

### Solution 1 : Utiliser un utilisateur avec privilèges SYSTEM
Connectez-vous avec l'utilisateur SYSTEM directement dans SQL Developer.

### Solution 2 : Utiliser le script alternatif
Utilisez le fichier `create_faceid_table_user.sql` qui crée la table dans votre schéma utilisateur. **MAIS** vous devrez aussi modifier le code C++ pour utiliser `FACE_ID` au lieu de `"SYSTEM"."FACE_ID"`.

## Vérification finale

Après avoir exécuté le script, testez avec :
```sql
SELECT * FROM "SYSTEM"."FACE_ID";
```

Si cette requête fonctionne (même si elle retourne 0 lignes), la table est créée correctement !

## Problèmes courants

1. **"Table already exists"** : La table existe déjà, c'est bon !
2. **"Insufficient privileges"** : Vous n'avez pas les droits, utilisez l'utilisateur SYSTEM
3. **"Invalid character"** : Vérifiez que vous avez copié tout le script, y compris les guillemets

