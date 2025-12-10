# Correction des numéros de téléphone dans la base de données

## 📋 Description

Ce script SQL permet de corriger automatiquement les numéros de téléphone mal formatés dans la table `HIBA.GEST_RESIDENT`.

## 🔧 Problèmes corrigés

1. **Numéros tunisiens mal formatés** : `+28500392` → `+21628500392`
2. **Numéros sans préfixe +** : `28500392` → `+21628500392`
3. **Numéros avec zéro initial** : `028500392` → `+21628500392`
4. **Numéros avec caractères spéciaux** : `+28 500 392` → `+21628500392`

## 📝 Utilisation

### Étape 1 : Vérification avant correction

Exécutez la première requête du script pour voir les numéros qui seront corrigés :

```sql
SELECT 
    ID,
    NOM,
    PRENOM,
    TELEPHONE AS "Numéro actuel",
    ...
FROM HIBA.GEST_RESIDENT
WHERE ...
```

### Étape 2 : Sauvegarde

**⚠️ IMPORTANT : Faites une sauvegarde de votre base de données avant d'exécuter les UPDATE !**

```sql
-- Exemple de sauvegarde (Oracle)
CREATE TABLE HIBA.GEST_RESIDENT_BACKUP AS SELECT * FROM HIBA.GEST_RESIDENT;
```

### Étape 3 : Exécution des corrections

Exécutez les requêtes UPDATE dans l'ordre :

1. Correction des numéros `+28XXXXXX` et `+29XXXXXX`
2. Correction des numéros sans préfixe `+`
3. Correction des numéros commençant par `0`
4. Nettoyage des caractères spéciaux

### Étape 4 : Vérification après correction

Exécutez la requête de vérification pour voir les résultats :

```sql
SELECT 
    ID,
    NOM,
    PRENOM,
    TELEPHONE AS "Numéro corrigé",
    ...
FROM HIBA.GEST_RESIDENT
WHERE TELEPHONE IS NOT NULL
ORDER BY ID;
```

## 📊 Formats de numéros supportés

### Numéros tunisiens
- `+28500392` → `+21628500392`
- `28500392` → `+21628500392`
- `028500392` → `+21628500392`

### Numéros français
- `0123456789` → `+33123456789`
- `123456789` → `+33123456789`

### Numéros marocains
- `+212XXXXXXXXX` (déjà correct)

### Numéros algériens
- `+213XXXXXXXXX` (déjà correct)

## ⚠️ Notes importantes

1. **Sauvegarde obligatoire** : Toujours faire une sauvegarde avant d'exécuter les UPDATE
2. **Vérification** : Vérifiez les résultats de la première requête avant de corriger
3. **Adaptation** : Adaptez les codes pays selon vos besoins
4. **Test** : Testez d'abord sur une copie de la base de données

## 🔍 Vérification des résultats

Après l'exécution, vérifiez que :
- Tous les numéros commencent par `+`
- Les numéros tunisiens ont le format `+216XXXXXXXX`
- Les numéros ont au moins 10 chiffres après le `+`
- Aucun caractère spécial (espaces, tirets, etc.) n'est présent

## 📞 Codes pays courants

- **+216** : Tunisie
- **+33** : France
- **+212** : Maroc
- **+213** : Algérie
- **+1** : États-Unis/Canada

## 🐛 Dépannage

Si certains numéros ne sont pas corrigés :
1. Vérifiez le format exact dans la base de données
2. Adaptez les conditions WHERE dans les requêtes UPDATE
3. Exécutez les corrections une par une pour identifier le problème

