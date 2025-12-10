-- Script SQL pour corriger les numéros de téléphone mal formatés dans la base de données
-- Ce script corrige les numéros tunisiens mal formatés (commençant par +28 ou +29 avec 8 chiffres)

-- ============================================
-- 1. Afficher les numéros à corriger AVANT la correction
-- ============================================
SELECT 
    ID,
    NOM,
    PRENOM,
    TELEPHONE AS "Numéro actuel",
    CASE 
        WHEN TELEPHONE LIKE '+28%' AND LENGTH(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', ''), '.', '')) = 8 
        THEN '+216' || SUBSTR(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', ''), '.', ''), 1)
        WHEN TELEPHONE LIKE '+29%' AND LENGTH(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', ''), '.', '')) = 8 
        THEN '+216' || SUBSTR(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', ''), '.', ''), 1)
        ELSE TELEPHONE
    END AS "Numéro corrigé"
FROM HIBA.GEST_RESIDENT
WHERE (TELEPHONE LIKE '+28%' OR TELEPHONE LIKE '+29%')
  AND LENGTH(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', ''), '.', '')) = 8
ORDER BY ID;

-- ============================================
-- 2. Corriger les numéros tunisiens mal formatés (+28XXXXXX -> +21628XXXXXX)
-- ============================================
UPDATE HIBA.GEST_RESIDENT
SET TELEPHONE = '+216' || SUBSTR(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', ''), '.', ''), 1)
WHERE (TELEPHONE LIKE '+28%' OR TELEPHONE LIKE '+29%')
  AND LENGTH(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', ''), '.', '')) = 8;

-- ============================================
-- 3. Corriger les numéros sans préfixe + (28500XXX -> +21628500XXX)
-- ============================================
UPDATE HIBA.GEST_RESIDENT
SET TELEPHONE = '+216' || TELEPHONE
WHERE TELEPHONE NOT LIKE '+%'
  AND LENGTH(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, ' ', ''), '-', ''), '.', ''), '(', '')) = 8
  AND (TELEPHONE LIKE '28%' OR TELEPHONE LIKE '29%' OR TELEPHONE LIKE '50%' OR TELEPHONE LIKE '90%');

-- ============================================
-- 4. Corriger les numéros commençant par 0 (028500XXX -> +21628500XXX)
-- ============================================
UPDATE HIBA.GEST_RESIDENT
SET TELEPHONE = '+216' || SUBSTR(TELEPHONE, 2)
WHERE TELEPHONE LIKE '0%'
  AND LENGTH(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, ' ', ''), '-', ''), '.', ''), '(', '')) = 9
  AND (TELEPHONE LIKE '028%' OR TELEPHONE LIKE '029%' OR TELEPHONE LIKE '050%' OR TELEPHONE LIKE '090%');

-- ============================================
-- 5. Nettoyer les numéros (supprimer espaces, tirets, points, parenthèses)
-- ============================================
UPDATE HIBA.GEST_RESIDENT
SET TELEPHONE = REPLACE(REPLACE(REPLACE(REPLACE(REPLACE(REPLACE(TELEPHONE, ' ', ''), '-', ''), '.', ''), '(', ''), ')', ''), '+', '')
WHERE TELEPHONE IS NOT NULL
  AND (TELEPHONE LIKE '% %' OR TELEPHONE LIKE '%-%' OR TELEPHONE LIKE '%.%' OR TELEPHONE LIKE '%(%' OR TELEPHONE LIKE '%)%');

-- Puis ajouter le + si nécessaire
UPDATE HIBA.GEST_RESIDENT
SET TELEPHONE = '+' || TELEPHONE
WHERE TELEPHONE IS NOT NULL
  AND TELEPHONE NOT LIKE '+%'
  AND LENGTH(TELEPHONE) >= 10;

-- ============================================
-- 6. Vérifier les résultats APRÈS la correction
-- ============================================
SELECT 
    ID,
    NOM,
    PRENOM,
    TELEPHONE AS "Numéro corrigé",
    CASE 
        WHEN TELEPHONE LIKE '+216%' AND LENGTH(REPLACE(TELEPHONE, '+', '')) = 11 THEN 'Valide (Tunisie)'
        WHEN TELEPHONE LIKE '+33%' AND LENGTH(REPLACE(TELEPHONE, '+', '')) >= 11 THEN 'Valide (France)'
        WHEN TELEPHONE LIKE '+212%' AND LENGTH(REPLACE(TELEPHONE, '+', '')) >= 11 THEN 'Valide (Maroc)'
        WHEN TELEPHONE LIKE '+213%' AND LENGTH(REPLACE(TELEPHONE, '+', '')) >= 11 THEN 'Valide (Algérie)'
        WHEN TELEPHONE LIKE '+%' AND LENGTH(REPLACE(TELEPHONE, '+', '')) >= 10 THEN 'Valide (International)'
        ELSE 'À vérifier'
    END AS "Statut"
FROM HIBA.GEST_RESIDENT
WHERE TELEPHONE IS NOT NULL
ORDER BY ID;

-- ============================================
-- 7. Statistiques des numéros corrigés
-- ============================================
SELECT 
    COUNT(*) AS "Total résidents",
    COUNT(CASE WHEN TELEPHONE LIKE '+216%' THEN 1 END) AS "Numéros tunisiens",
    COUNT(CASE WHEN TELEPHONE LIKE '+33%' THEN 1 END) AS "Numéros français",
    COUNT(CASE WHEN TELEPHONE LIKE '+212%' THEN 1 END) AS "Numéros marocains",
    COUNT(CASE WHEN TELEPHONE LIKE '+213%' THEN 1 END) AS "Numéros algériens",
    COUNT(CASE WHEN TELEPHONE NOT LIKE '+%' OR LENGTH(REPLACE(TELEPHONE, '+', '')) < 10 THEN 1 END) AS "Numéros invalides"
FROM HIBA.GEST_RESIDENT
WHERE TELEPHONE IS NOT NULL;

-- ============================================
-- NOTES IMPORTANTES:
-- ============================================
-- 1. Exécutez d'abord la requête #1 pour voir les numéros qui seront corrigés
-- 2. Vérifiez les résultats avant d'exécuter les UPDATE
-- 3. Faites une sauvegarde de la base de données avant d'exécuter ce script
-- 4. Adaptez les codes pays selon vos besoins (216=Tunisie, 33=France, etc.)
-- 5. Les numéros doivent être au format international: +CODE_PAYS + NUMERO
-- 6. Exemple: +21628500392 (Tunisie), +33123456789 (France)

