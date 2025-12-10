-- Script SQL SIMPLIFIÉ pour corriger les numéros de téléphone
-- Exécutez ce script dans l'ordre indiqué

-- ============================================
-- ÉTAPE 1: Voir les numéros à corriger
-- ============================================
SELECT ID, NOM, PRENOM, TELEPHONE 
FROM HIBA.GEST_RESIDENT 
WHERE (TELEPHONE LIKE '+28%' OR TELEPHONE LIKE '+29%')
  AND LENGTH(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', '')) = 8;

-- ============================================
-- ÉTAPE 2: Corriger +28XXXXXX et +29XXXXXX en +21628XXXXXX et +21629XXXXXX
-- ============================================
UPDATE HIBA.GEST_RESIDENT
SET TELEPHONE = '+216' || SUBSTR(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', ''), 1)
WHERE (TELEPHONE LIKE '+28%' OR TELEPHONE LIKE '+29%')
  AND LENGTH(REPLACE(REPLACE(REPLACE(TELEPHONE, '+', ''), ' ', ''), '-', '')) = 8;

-- ============================================
-- ÉTAPE 3: Vérifier les résultats
-- ============================================
SELECT ID, NOM, PRENOM, TELEPHONE 
FROM HIBA.GEST_RESIDENT 
WHERE TELEPHONE LIKE '+216%'
ORDER BY ID;

