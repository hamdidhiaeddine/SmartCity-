# 🔐 CONFIGURATION DE L'AUTHENTIFICATION

## 📋 Étapes d'installation

### 1. Créer la table USERS dans la base de données

**Option A : Exécution automatique**

```
Double-cliquez sur : EXECUTER_CREATE_USERS.bat
```

**Option B : Exécution manuelle**

```sql
sqlplus hiba/esprit18@XE
SQL> @CREATE_TABLE_USERS.sql
```

### 2. Vérifier la création

```sql
SELECT LOGIN, NOM, PRENOM, ROLE FROM USERS;
```

Résultat attendu :

```
LOGIN          NOM             PRENOM      ROLE
-------------- --------------- ----------- --------
admin          Administrateur  Système     ADMIN
hiba           HIBA            User        ADMIN
user           Test            User        USER
gestionnaire   Gestionnaire    SmartCity   MANAGER
```

---

## 🔑 Comptes disponibles

| Login          | Mot de passe | Rôle    | Description            |
| -------------- | ------------ | ------- | ---------------------- |
| `admin`        | `admin`      | ADMIN   | Administrateur système |
| `hiba`         | `esprit18`   | ADMIN   | Compte base de données |
| `user`         | `user123`    | USER    | Utilisateur standard   |
| `gestionnaire` | `gest123`    | MANAGER | Gestionnaire           |

---

## ✅ Test de connexion

1. Lancez l'application
2. La fenêtre d'authentification s'ouvre automatiquement
3. Entrez un login et mot de passe
4. Cliquez sur "connexion"

**Connexion réussie** → Accès à l'application principale  
**Connexion échouée** → Message d'erreur

---

## 🔧 Ajouter un nouvel utilisateur

### Via SQL

```sql
INSERT INTO USERS (ID_USER, LOGIN, MOT_DE_PASSE, NOM, PRENOM, EMAIL, ROLE, ACTIF)
VALUES (SEQ_USERS.NEXTVAL, 'nouveau_login', 'mot_de_passe', 'Nom', 'Prenom', 'email@example.com', 'USER', 1);

COMMIT;
```

### Paramètres

- **LOGIN** : Identifiant unique (50 caractères max)
- **MOT_DE_PASSE** : Mot de passe (100 caractères max)
- **NOM/PRENOM** : Nom complet de l'utilisateur
- **EMAIL** : Adresse email
- **ROLE** : `ADMIN`, `MANAGER` ou `USER`
- **ACTIF** : `1` = actif, `0` = désactivé

---

## 🛡️ Désactiver un utilisateur

```sql
UPDATE USERS
SET ACTIF = 0
WHERE LOGIN = 'nom_utilisateur';

COMMIT;
```

---

## 🔄 Réinitialiser un mot de passe

```sql
UPDATE USERS
SET MOT_DE_PASSE = 'nouveau_mot_de_passe'
WHERE LOGIN = 'nom_utilisateur';

COMMIT;
```

---

## ⚠️ Mode fallback

Si la table USERS n'existe pas, l'application utilise un mode fallback avec :

- `admin` / `admin`
- `hiba` / `esprit18`

**Créez la table USERS pour une sécurité complète !**

---

## 🔒 Sécurité - Recommandations pour la production

### ⚠️ IMPORTANT : Mots de passe en clair

Actuellement, les mots de passe sont stockés **en clair** dans la base de données.  
**Ceci est acceptable uniquement pour le développement/démonstration.**

### 🛡️ Pour la production, implémentez :

1. **Hashage des mots de passe**

   ```sql
   -- Utiliser DBMS_CRYPTO pour hasher
   CREATE OR REPLACE FUNCTION hash_password(p_password VARCHAR2)
   RETURN VARCHAR2 IS
   BEGIN
       RETURN DBMS_CRYPTO.HASH(
           UTL_RAW.CAST_TO_RAW(p_password),
           DBMS_CRYPTO.HASH_SH256
       );
   END;
   ```

2. **Salt aléatoire** pour chaque utilisateur

3. **Politique de mots de passe forts**

   - Minimum 8 caractères
   - Lettres majuscules/minuscules
   - Chiffres et caractères spéciaux

4. **Limitation des tentatives de connexion**

5. **Session timeout**

6. **Logs d'authentification**

---

## 📊 Structure de la table USERS

```sql
DESC USERS;

Nom                 Type             Nullable
------------------- ---------------- --------
ID_USER             NUMBER           NOT NULL (PK)
LOGIN               VARCHAR2(50)     NOT NULL (UNIQUE)
MOT_DE_PASSE        VARCHAR2(100)    NOT NULL
NOM                 VARCHAR2(50)
PRENOM              VARCHAR2(50)
EMAIL               VARCHAR2(100)
ROLE                VARCHAR2(20)     DEFAULT 'USER'
DATE_CREATION       DATE             DEFAULT SYSDATE
ACTIF               NUMBER(1)        DEFAULT 1
```

---

## 🐛 Dépannage

### Erreur : "Table or view does not exist"

➜ Exécutez `EXECUTER_CREATE_USERS.bat`

### Erreur : "Unique constraint violated"

➜ Le login existe déjà, choisissez un autre login

### Connexion refusée malgré bon mot de passe

➜ Vérifiez que `ACTIF = 1` :

```sql
SELECT LOGIN, ACTIF FROM USERS WHERE LOGIN = 'votre_login';
```

### Mode fallback activé

➜ La table USERS n'a pas été créée ou n'est pas accessible

---

## 📝 Exemple d'utilisation dans le code C++

```cpp
// L'authentification est automatique dans main.cpp
Authentification authDialog;
if (authDialog.exec() == QDialog::Accepted) {
    // Connexion réussie
    QString login = authDialog.getIdentifiant();
    QString nom = authDialog.getNom(); // À ajouter
}
```

---

## ✨ Prochaines améliorations possibles

- [ ] Récupération de mot de passe par email
- [ ] Double authentification (2FA)
- [ ] Gestion des rôles et permissions
- [ ] Historique des connexions
- [ ] Changement de mot de passe par l'utilisateur
- [ ] Questions de sécurité
- [ ] Blocage après X tentatives échouées

---

**Créé le** : 1 décembre 2025  
**Projet** : SmartCity - Gestion intégrée
