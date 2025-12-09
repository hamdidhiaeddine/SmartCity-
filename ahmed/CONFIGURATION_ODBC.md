# 🔧 Configuration ODBC pour SmartCity

## ⚠️ Prérequis
- Oracle Database installé (Oracle 11g/12c/19c/21c ou Oracle XE)
- Oracle Client installé
- Pilote ODBC Oracle installé

---

## 📋 Étape 1 : Vérifier l'installation Oracle

### Windows
1. Ouvrez PowerShell ou CMD
2. Tapez :
```cmd
tnsping localhost
```
ou pour tester la connexion utilisateur :
```cmd
sqlplus wala/esprit18@localhost/XE
```

**Si vous êtes déjà dans SQL*Plus**, utilisez simplement :
```sql
CONNECT wala/esprit18@localhost/XE
```

Si ça fonctionne, passez à l'étape suivante.

---

## 🔌 Étape 2 : Créer la source de données ODBC

### Sur Windows (32-bit et 64-bit)

**Pour application Qt 64-bit (recommandé):**

1. **Ouvrir l'administrateur ODBC 64-bit**
   - Appuyez sur `Windows + R`
   - Tapez : `C:\Windows\System32\odbcad32.exe`
   - Cliquez sur OK

2. **Créer une nouvelle source système**
   - Allez dans l'onglet **"DSN Système"**
   - Cliquez sur **"Ajouter..."**

3. **Sélectionner le pilote Oracle**
   
   Vous devriez voir l'un de ces pilotes :
   - `Oracle in OraClient19Home1` (recommandé)
   - `Oracle in OraDb19Home1`
   - `Microsoft ODBC for Oracle` (ancien, non recommandé)

   **Si vous ne voyez AUCUN pilote Oracle**, installez Oracle Instant Client (voir section Installation ci-dessous).

4. **Configuration de la source de données**
   
   Remplissez les champs suivants :
   
   | Champ | Valeur |
   |-------|--------|
   | **Data Source Name** | `source_projet2A` |
   | **Description** | SmartCity Database |
   | **TNS Service Name** | `localhost/XE` ou `localhost/ORCL` |
   | **User ID** | `wala` |

   OU si vous n'avez pas de TNS configuré :
   
   | Champ | Valeur |
   |-------|--------|
   | **Data Source Name** | `source_projet2A` |
   | **Server** | `localhost` |
   | **Port** | `1521` |
   | **Service Name** | `XE` ou `ORCL` |
   | **User ID** | `wala` |

5. **Tester la connexion**
   - Cliquez sur **"Test Connection"**
   - Entrez le mot de passe : `esprit18`
   - Si succès → **OK**, sinon voir Dépannage

6. **Enregistrer**
   - Cliquez sur **OK** → **OK**

---

## 💾 Étape 3 : Exécuter le script SQL

1. Ouvrez **SQL Developer** ou **SQL*Plus**
2. Connectez-vous en tant que **SYSTEM** (ou DBA)
3. Exécutez le fichier `database_schema.sql`
4. Vérifiez que la table est créée :
   ```sql
   SELECT * FROM wala.VEHICULE;
   ```

---

## 🔍 Dépannage

### Erreur : "Source de données introuvable"

**Solution 1 : Vérifier l'architecture (32-bit vs 64-bit)**

Votre application Qt est probablement en **64-bit**. Assurez-vous d'utiliser :
- Administrateur ODBC 64-bit : `C:\Windows\System32\odbcad32.exe`
- Pilote Oracle 64-bit installé

**Solution 2 : Utiliser la connexion directe sans DSN**

Modifiez `connection.cpp` :

```cpp
bool Connection::createconnect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    
    // Connexion SANS DSN (connection string complète)
    db.setDatabaseName("DRIVER={Oracle in OraClient19Home1};"
                       "SERVER=localhost;"
                       "PORT=1521;"
                       "SERVICE_NAME=XE;"
                       "UID=wala;"
                       "PWD=esprit18;");
    
    if (!db.open()) {
        qDebug() << "❌ Erreur:" << db.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Connexion réussie";
    return true;
}
```

Recompilez et testez.

---

### Erreur : "Pilote Oracle introuvable"

**Solution : Installer Oracle Instant Client**

1. Téléchargez depuis : https://www.oracle.com/database/technologies/instant-client/downloads.html

2. Choisissez la version **64-bit** compatible avec votre Oracle DB

3. Téléchargez :
   - `instantclient-basic-windows.x64-XX.X.zip`
   - `instantclient-odbc-windows.x64-XX.X.zip`

4. Décompressez dans : `C:\oracle\instantclient_XX_X`

5. Ajoutez au PATH système :
   - `C:\oracle\instantclient_XX_X`

6. Installez le pilote ODBC :
   ```cmd
   cd C:\oracle\instantclient_XX_X
   odbc_install.exe
   ```

7. Redémarrez votre PC

8. Retournez à l'Étape 2

---

### Erreur : "ORA-12154: TNS:could not resolve the connect identifier"

**Solution : Utiliser IP au lieu de localhost**

Remplacez `localhost` par `127.0.0.1` dans la configuration DSN.

---

### Erreur : "ORA-01017: invalid username/password"

**Solution : Vérifier les credentials**

```sql
-- En tant que SYSTEM dans SQL*Plus
ALTER USER wala IDENTIFIED BY esprit18;
GRANT CONNECT, RESOURCE TO wala;
```

---

## ✅ Vérification finale

Après configuration, testez depuis Qt :

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
db.setDatabaseName("source_projet2A");
db.setUserName("wala");
db.setPassword("esprit18");

if (db.open()) {
    qDebug() << "✅ CONNEXION RÉUSSIE !";
} else {
    qDebug() << "❌ ERREUR:" << db.lastError().text();
}
```

---

## 📞 Aide supplémentaire

Si le problème persiste :

1. Vérifiez les logs Oracle : `listener.log`
2. Testez avec SQL Developer (même username/password)
3. Vérifiez que le service Oracle est démarré :
   ```cmd
   services.msc
   ```
   Cherchez : `OracleServiceXE` ou `OracleServiceORCL`

---

**Créé pour le projet Qt SmartCity - 2025**
