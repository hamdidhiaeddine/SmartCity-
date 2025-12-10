-- ============================================================
-- VÉRIFICATION DE LA STRUCTURE DE LA TABLE GEST_VEHICULE
-- ============================================================

PROMPT ============================================================
PROMPT VÉRIFICATION DE LA STRUCTURE GEST_VEHICULE
PROMPT ============================================================
PROMPT

-- 1. Vérifier si la table existe
PROMPT 1. Existence de la table...
SELECT 
    CASE 
        WHEN COUNT(*) > 0 THEN '✓ Table GEST_VEHICULE existe'
        ELSE '✗ Table GEST_VEHICULE n''existe PAS !'
    END AS STATUT
FROM USER_TABLES 
WHERE TABLE_NAME = 'GEST_VEHICULE';

PROMPT

-- 2. Structure de la table
PROMPT 2. Structure de la table GEST_VEHICULE:
SELECT 
    COLUMN_ID AS "#",
    COLUMN_NAME AS "Colonne",
    DATA_TYPE AS "Type",
    DATA_LENGTH AS "Longueur",
    NULLABLE AS "Nullable"
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
ORDER BY COLUMN_ID;

PROMPT

-- 3. Contraintes
PROMPT 3. Contraintes de la table:
SELECT 
    CONSTRAINT_NAME AS "Nom",
    CONSTRAINT_TYPE AS "Type",
    SEARCH_CONDITION AS "Condition",
    STATUS AS "Statut"
FROM USER_CONSTRAINTS
WHERE TABLE_NAME = 'GEST_VEHICULE'
ORDER BY CONSTRAINT_TYPE, CONSTRAINT_NAME;

PROMPT

-- 4. Vérifier la colonne ID
PROMPT 4. Vérification de la colonne ID:
SELECT 
    COLUMN_NAME,
    DATA_TYPE,
    CASE 
        WHEN DATA_TYPE = 'NUMBER' THEN '✓ Type NUMBER'
        ELSE '⚠️ Type: ' || DATA_TYPE
    END AS COMMENTAIRE
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
AND COLUMN_NAME = 'ID';

PROMPT

-- 5. Vérifier la colonne IMMATRICULATION
PROMPT 5. Vérification de la colonne IMMATRICULATION:
SELECT 
    COLUMN_NAME,
    DATA_TYPE,
    DATA_LENGTH,
    CASE 
        WHEN DATA_TYPE = 'VARCHAR2' THEN '✓ Type VARCHAR2 - Accepte les lettres'
        WHEN DATA_TYPE = 'NUMBER' THEN '⚠️ Type NUMBER - N''accepte PAS les lettres'
        ELSE 'Type: ' || DATA_TYPE
    END AS COMMENTAIRE
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
AND COLUMN_NAME = 'IMMATRICULATION';

PROMPT

-- 6. Vérifier la colonne DATE
PROMPT 6. Colonne DATE de maintenance:
SELECT 
    COLUMN_NAME,
    DATA_TYPE
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
AND (COLUMN_NAME LIKE '%MAINT%' OR COLUMN_NAME LIKE '%DATE%')
ORDER BY COLUMN_NAME;

PROMPT

-- 7. Compter les véhicules
PROMPT 7. Nombre de véhicules dans la table:
SELECT COUNT(*) AS "Nombre de véhicules" FROM GEST_VEHICULE;

PROMPT

-- 8. Afficher quelques exemples
PROMPT 8. Exemples de véhicules (5 premiers):
SELECT * FROM (
    SELECT ID, IMMATRICULATION, MARQUE, MODELE, TYPE, ETAT, SERVICE, 
           COALESCE(DATE_MAINTENANCE, DATEMAINTENCE) AS DATE_MAINT
    FROM GEST_VEHICULE
    ORDER BY ID DESC
) WHERE ROWNUM <= 5;

PROMPT
PROMPT ============================================================
PROMPT FIN DE LA VÉRIFICATION
PROMPT ============================================================

