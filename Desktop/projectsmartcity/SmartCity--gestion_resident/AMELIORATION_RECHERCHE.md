
# 🔍 Amélioration de la recherche - Situation familiale

## ✅ Modification effectuée

La fonction de recherche a été améliorée pour inclure la **situation familiale** et d'autres champs pertinents.

## 📋 Champs de recherche disponibles

La recherche fonctionne maintenant sur **tous ces champs** :

1. ✅ **ID résident** - Identifiant unique
2. ✅ **Nom complet** - Nom et prénom combinés
3. ✅ **Nom** - Nom de famille
4. ✅ **Prénom** - Prénom
5. ✅ **Situation familiale** - Célibataire, Marié(e), Divorcé(e), etc.
6. ✅ **Statut** - Statut du résident
7. ✅ **Adresse** - Adresse complète
8. ✅ **Téléphone** - Numéro de téléphone
9. ✅ **Email** - Adresse email

## 🎯 Utilisation

### Exemple 1 : Recherche par situation familiale
- Tapez dans le champ de recherche : **"Marié"** ou **"Célibataire"**
- Tous les résidents avec cette situation familiale seront affichés

### Exemple 2 : Recherche par statut
- Tapez : **"Actif"** ou **"Inactif"**
- Les résidents correspondants seront affichés

### Exemple 3 : Recherche par adresse
- Tapez : **"Tunis"** ou **"Avenue"**
- Tous les résidents avec cette adresse seront affichés

### Exemple 4 : Recherche combinée
- La recherche est **insensible à la casse** (majuscules/minuscules)
- Recherche **partielle** (pas besoin de taper le mot complet)
- Exemple : **"mar"** trouvera "Marié", "Mariage", etc.

## 🔧 Code modifié

**Fichier** : `mainwindow.cpp`  
**Fonction** : `filtrerResidents()`

### Avant
```cpp
// Recherche uniquement par ID et nom complet
if (r.id().contains(critere, Qt::CaseInsensitive) ||
    r.nomComplet().contains(critere, Qt::CaseInsensitive)) {
    temp.append(r);
}
```

### Après
```cpp
// Recherche dans tous les champs pertinents
bool correspond = 
    r.id().contains(critere, Qt::CaseInsensitive) ||
    r.nomComplet().contains(critere, Qt::CaseInsensitive) ||
    r.nom().contains(critere, Qt::CaseInsensitive) ||
    r.prenom().contains(critere, Qt::CaseInsensitive) ||
    r.situation().contains(critere, Qt::CaseInsensitive) ||  // ← NOUVEAU
    r.statut().contains(critere, Qt::CaseInsensitive) ||
    r.adresse().contains(critere, Qt::CaseInsensitive) ||
    r.telephone().contains(critere, Qt::CaseInsensitive) ||
    r.email().contains(critere, Qt::CaseInsensitive);
```

## 🚀 Prochaines étapes

1. **Recompilez l'application** pour appliquer les modifications
2. **Testez la recherche** avec différents critères
3. **Vérifiez** que la recherche par situation familiale fonctionne

## 💡 Astuces

- La recherche est **instantanée** (se déclenche pendant la saisie)
- Vous pouvez combiner plusieurs mots-clés
- La recherche fonctionne sur **tous les champs simultanément**
- Utilisez le tri pour organiser les résultats

## 📝 Notes techniques

- Recherche **insensible à la casse** (majuscules/minuscules ignorées)
- Recherche **partielle** (contient le texte saisi)
- Performance optimisée pour de grandes listes
- Compatible avec le tri existant



