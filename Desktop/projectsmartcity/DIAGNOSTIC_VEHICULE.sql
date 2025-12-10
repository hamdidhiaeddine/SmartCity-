-- ============================================================
-- SCRIPT DE DIAGNOSTIC POUR LE MODULE VÉHICULE
-- ============================================================
-- Exécutez ce script pour vérifier la structure de la table
-- ============================================================

PROMPT ============================================================
PROMPT DIAGNOSTIC TABLE GEST_VEHICULE
PROMPT ============================================================
PROMPT

-- 1. Vérifier si la table existe
PROMPT 1. Vérification de l'existence de la table...
SELECT 
    CASE 
        WHEN COUNT(*) > 0 THEN '✓ Table GEST_VEHICULE existe'
        ELSE '✗ Table GEST_VEHICULE n''existe PAS !'
    END AS STATUT
FROM USER_TABLES 
WHERE TABLE_NAME = 'GEST_VEHICULE';

PROMPT

-- 2. Afficher la structure de la table
PROMPT 2. Structure de la table GEST_VEHICULE:
SELECT 
    COLUMN_ID AS "#",
    COLUMN_NAME AS "Colonne",
    DATA_TYPE AS "Type",
    DATA_LENGTH AS "Longueur",
    NULLABLE AS "Nullable",
    DATA_DEFAULT AS "Valeur par défaut"
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
ORDER BY COLUMN_ID;

PROMPT

-- 3. Vérifier le type de IMMATRICULATION
PROMPT 3. Type de la colonne IMMATRICULATION:
SELECT 
    COLUMN_NAME,
    DATA_TYPE,
    DATA_LENGTH,
    CASE 
        WHEN DATA_TYPE = 'NUMBER' THEN '⚠️ PROBLÈME: NUMBER ne permet pas les lettres (ex: AB-123-CD)'
        WHEN DATA_TYPE = 'VARCHAR2' THEN '✓ VARCHAR2 permet les immatriculations avec lettres'
        ELSE 'Type inconnu'
    END AS COMMENTAIRE
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
AND COLUMN_NAME = 'IMMATRICULATION';

PROMPT

-- 4. Vérifier le nom de la colonne DATE
PROMPT 4. Colonne DATE de maintenance:
SELECT 
    COLUMN_NAME,
    DATA_TYPE
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
AND (COLUMN_NAME LIKE '%MAINT%' OR COLUMN_NAME LIKE '%DATE%')
ORDER BY COLUMN_NAME;

PROMPT

-- 5. Compter les véhicules existants
PROMPT 5. Nombre de véhicules dans la table:
SELECT COUNT(*) AS "Nombre de véhicules" FROM GEST_VEHICULE;

PROMPT

-- 6. Afficher quelques exemples
PROMPT 6. Exemples de véhicules (5 premiers):
SELECT 
    IMMATRICULATION,
    MARQUE,
    MODELE,
    TYPE,
    ETAT
FROM GEST_VEHICULE 
WHERE ROWNUM <= 5
ORDER BY IMMATRICULATION;

PROMPT

-- 7. Vérifier les contraintes
PROMPT 7. Contraintes sur la table:
SELECT 
    CONSTRAINT_NAME AS "Nom",
    CONSTRAINT_TYPE AS "Type",
    STATUS AS "Statut"
FROM USER_CONSTRAINTS 
WHERE TABLE_NAME = 'GEST_VEHICULE'
ORDER BY CONSTRAINT_TYPE, CONSTRAINT_NAME;

PROMPT

-- 8. Test d'insertion (ne sera pas commité)
PROMPT 8. Test d'insertion (ROLLBACK automatique)...
PROMPT    Tentative d'insertion d'un véhicule de test...

SAVEPOINT test_insert;

INSERT INTO GEST_VEHICULE (
    IMMATRICULATION, 
    MARQUE, 
    MODELE, 
    TYPE, 
    ETAT, 
    SERVICE, 
    DATEMAINTENCE
) VALUES (
    'TEST-123',
    'Test',
    'Model',
    'Type',
    'Bon',
    'Service',
    SYSDATE
);

-- Vérifier si l'insertion a réussi
IF SQL%ROWCOUNT > 0 THEN
    DBMS_OUTPUT.PUT_LINE('✓ Test d''insertion RÉUSSI');
ELSE
    DBMS_OUTPUT.PUT_LINE('✗ Test d''insertion ÉCHOUÉ');
END IF;

ROLLBACK TO test_insert;

PROMPT
PROMPT ============================================================
PROMPT RÉSUMÉ
PROMPT ============================================================
PROMPT
PROMPT Si IMMATRICULATION est NUMBER:
PROMPT   → Exécutez: @CORRIGER_TABLE_VEHICULE.sql
PROMPT
PROMPT Si la colonne DATE s'appelle DATE_MAINTENANCE:
PROMPT   → Le code gère automatiquement les deux noms
PROMPT
PROMPT Si la table n'existe pas:
PROMPT   → Exécutez: @create_database_complete_fixed.sql
PROMPT
PROMPT ============================================================

