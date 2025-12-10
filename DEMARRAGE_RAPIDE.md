# 🚀 DÉMARRAGE RAPIDE - INTÉGRATION SMARTCITY

## ⏱️ EN 3 ÉTAPES (30 MINUTES)

### ÉTAPE 1: Base de données (5 min) ✅

**Double-cliquez sur:**

```
EXECUTER_INTEGRATION_BDD.bat
```

**Ce que ça fait:**

- Crée les tables EMPLOYES, VEHICULE, GEST_MAISON, GEST_ALERTES
- Insère des données de test
- Vérifie que tout fonctionne

**Résultat attendu:**

```
✓ Table EMPLOYES créée
✓ Table VEHICULE et séquence SEQ_VEHI créées
✓ Table GEST_MAISON créée
✓ Table GEST_ALERTES, séquence et trigger créés
✓ Données de test insérées
```

---

### ÉTAPE 2: Code C++ (20 min) ✅

**Ouvrez `mainwindow.h` et ajoutez dans `private slots:`**

```cpp
void onAjouterEmploye();
void onModifierEmploye();
void onSupprimerEmploye();
void chargerEmployes();

void onAjouterVehicule();
void onModifierVehicule();
void onSupprimerVehicule();
void chargerVehicules();
```

**Ajoutez dans `private:`**

```cpp
void reinitialiserFormulaireEmploye();
void reinitialiserFormulaireVehicule();
```

**Ouvrez `mainwindow.cpp` et copiez le code depuis `README_INTEGRATION.md`**

- Section 3.1: Slots de navigation (onGestionEmployes, onGestionVehicules)
- Section 3.2: Gestion des employés (6 fonctions)
- Section 3.3: Connecter les boutons dans connectButtons()

_Voir le fichier README_INTEGRATION.md pour le code complet_

---

### ÉTAPE 3: Compiler et tester (5 min) ✅

**Option A: Qt Creator (recommandé)**

1. Ouvrez Qt Creator
2. File → Open File or Project → `C:\ayooo\projetsmartcity.pro`
3. Build → Build Project
4. Run → Run

**Option B: Ligne de commande**

```powershell
cd C:\ayooo
qmake projetsmartcity.pro
mingw32-make
.\release\Atelier_Connexion.exe
```

---

## ✅ TESTS DE VALIDATION

### Test 1: Module Employés (2 min)

1. Cliquez sur le bouton "Employés" dans le menu
2. Vérifiez que 2 employés apparaissent (Dupont, Martin)
3. Cliquez "Ajouter", remplissez les champs, validez
4. Vérifiez que l'employé apparaît dans le tableau
5. Sélectionnez-le, cliquez "Modifier", changez le nom
6. Sélectionnez-le, cliquez "Supprimer"

✅ **Si tout fonctionne**: Le module Employés est opérationnel

### Test 2: Module Véhicules (2 min)

1. Cliquez sur "Véhicules"
2. Vérifiez que 2 véhicules apparaissent
3. Testez Ajouter/Modifier/Supprimer comme pour les employés

✅ **Si tout fonctionne**: Le module Véhicules est opérationnel

### Test 3: Historique Résidents (1 min)

1. Cliquez sur "Résidents"
2. Ajoutez un nouveau résident
3. Cliquez sur "Afficher Historique"
4. Vérifiez qu'une ligne "Ajout" apparaît avec la date/heure
5. Modifiez le résident → Vérifiez "Modification"
6. Supprimez le résident → Vérifiez "Suppression"

✅ **Si tout fonctionne**: L'historique est opérationnel

---

## 🆘 EN CAS DE PROBLÈME

### Erreur: "Employee.h: No such file or directory"

```
Solution: Vérifiez que Employee.h et Employee.cpp sont dans C:\ayooo
```

### Erreur: "table or view does not exist"

```
Solution: Relancez EXECUTER_INTEGRATION_BDD.bat
```

### Erreur: "undefined reference to Employee::fetchAll"

```
Solution:
1. Vérifiez que projetsmartcity.pro contient Employee.cpp
2. Relancez qmake
3. Recompilez
```

### Les boutons ne font rien

```
Solution: Vérifiez que les connect() sont bien dans connectButtons()
```

### L'historique ne s'enregistre pas

```
Solution:
1. Exécutez: sqlplus hiba/esprit18@XE @REPARER_HISTORIQUE_COMPLET.sql
2. Recompilez mainwindow.cpp
```

---

## 📁 FICHIERS IMPORTANTS

### Documentation

- 📖 `README_INTEGRATION.md` - **GUIDE COMPLET** (code détaillé)
- 📊 `SYNTHESE_INTEGRATION.md` - Vue d'ensemble
- ✅ `CHECKLIST_INTEGRATION.md` - Référence rapide
- 🏗️ `ARCHITECTURE_PROJET.md` - Architecture détaillée
- 🚀 `DEMARRAGE_RAPIDE.md` - Ce fichier

### Scripts

- 💾 `INTEGRATION_TABLES_BDD.sql` - Création des tables
- ▶️ `EXECUTER_INTEGRATION_BDD.bat` - Exécution automatique
- 🔧 `REPARER_HISTORIQUE_COMPLET.sql` - Réparation si problème

### Code source

- 👨‍💼 `Employee.h` / `Employee.cpp` - Gestion employés
- 🚗 `vehicule.h` / `vehicule.cpp` - Gestion véhicules
- 🏠 `mainwindow.h` / `mainwindow.cpp` - Contrôleur principal

---

## 💡 ASTUCES

### Pendant le développement

- Utilisez Qt Creator pour avoir l'auto-complétion
- Consultez la console (Application Output) pour les messages de debug
- Utilisez `qDebug() << "Mon message";` pour déboguer

### Pour vérifier la base de données

```sql
-- Connexion
sqlplus hiba/esprit18@XE

-- Voir les tables
SELECT TABLE_NAME FROM USER_TABLES;

-- Compter les lignes
SELECT 'EMPLOYES', COUNT(*) FROM EMPLOYES
UNION ALL SELECT 'VEHICULE', COUNT(*) FROM VEHICULE;

-- Voir les triggers
SELECT TRIGGER_NAME, STATUS FROM USER_TRIGGERS;
```

### Pour nettoyer et recompiler

```powershell
# Nettoyer
cd C:\ayooo
Remove-Item -Recurse -Force build, release, debug, *.o, Makefile

# Recompiler
qmake projetsmartcity.pro
mingw32-make
```

---

## 🎯 RÉSULTAT FINAL

Après avoir suivi ces 3 étapes, vous aurez:

✅ **3 modules fonctionnels**

- Résidents (avec historique automatique)
- Employés (CRUD complet)
- Véhicules (CRUD complet)

✅ **Base de données complète**

- 6 tables (EMPLOYES, VEHICULE, GEST_MAISON, GEST_ALERTES, GEST_RESIDENT, HISTORIQUE_RESIDENT)
- 3 triggers automatiques
- 3 séquences auto-increment

✅ **Prêt pour extension**

- Tables Maisons et Alertes déjà créées
- Code disponible pour Jardins et Chatbot
- Architecture modulaire facile à étendre

---

## 📞 BESOIN D'AIDE ?

Consultez dans l'ordre:

1. 🚀 **DEMARRAGE_RAPIDE.md** (ce fichier) - Vue rapide
2. 📖 **README_INTEGRATION.md** - Code complet et détaillé
3. ✅ **CHECKLIST_INTEGRATION.md** - Référence des widgets UI
4. 🏗️ **ARCHITECTURE_PROJET.md** - Structure du projet

**Temps total estimé: 30 minutes**

Bon courage! 🎉

---

**Dernière mise à jour**: 1er décembre 2025  
**Version**: 1.0  
**Status**: ✅ Prêt à utiliser
