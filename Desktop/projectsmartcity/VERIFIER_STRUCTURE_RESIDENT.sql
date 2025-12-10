-- ============================================================
-- VÉRIFICATION DE LA STRUCTURE DE LA TABLE GEST_RESIDENT
-- ============================================================

PROMPT ============================================================
PROMPT VÉRIFICATION DE LA STRUCTURE GEST_RESIDENT
PROMPT ============================================================
PROMPT

-- 1. Vérifier si la table existe
PROMPT 1. Existence de la table...
SELECT 
    CASE 
        WHEN COUNT(*) > 0 THEN '✓ Table GEST_RESIDENT existe'
        ELSE '✗ Table GEST_RESIDENT n''existe PAS !'
    END AS STATUT
FROM USER_TABLES 
WHERE TABLE_NAME = 'GEST_RESIDENT';

PROMPT

-- 2. Structure de la table
PROMPT 2. Structure de la table GEST_RESIDENT:
SELECT 
    COLUMN_ID AS "#",
    COLUMN_NAME AS "Colonne",
    DATA_TYPE AS "Type",
    DATA_LENGTH AS "Longueur",
    NULLABLE AS "Nullable"
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_RESIDENT' 
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
WHERE TABLE_NAME = 'GEST_RESIDENT'
ORDER BY CONSTRAINT_TYPE, CONSTRAINT_NAME;

PROMPT

-- 4. Vérifier les colonnes NOT NULL
PROMPT 4. Colonnes obligatoires (NOT NULL):
SELECT 
    COLUMN_NAME AS "Colonne",
    DATA_TYPE AS "Type",
    'NOT NULL' AS "Contrainte"
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_RESIDENT' 
AND NULLABLE = 'N'
ORDER BY COLUMN_ID;

PROMPT

-- 5. Compter les résidents
PROMPT 5. Nombre de résidents dans la table:
SELECT COUNT(*) AS "Nombre de résidents" FROM GEST_RESIDENT;

PROMPT

-- 6. Afficher quelques exemples
PROMPT 6. Exemples de résidents (5 premiers):
SELECT * FROM (
    SELECT ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE
    FROM GEST_RESIDENT
    ORDER BY ID DESC
) WHERE ROWNUM <= 5;

PROMPT

-- 7. Vérifier les clés étrangères
PROMPT 7. Clés étrangères référençant GEST_RESIDENT:
SELECT 
    UC.CONSTRAINT_NAME AS "Nom contrainte",
    UC.TABLE_NAME AS "Table référençante",
    UCC.COLUMN_NAME AS "Colonne",
    UC.R_CONSTRAINT_NAME AS "Contrainte référencée"
FROM USER_CONSTRAINTS UC
JOIN USER_CONS_COLUMNS UCC ON UC.CONSTRAINT_NAME = UCC.CONSTRAINT_NAME
WHERE UC.CONSTRAINT_TYPE = 'R'
AND UC.R_CONSTRAINT_NAME IN (
    SELECT CONSTRAINT_NAME 
    FROM USER_CONSTRAINTS 
    WHERE TABLE_NAME = 'GEST_RESIDENT' 
    AND CONSTRAINT_TYPE = 'P'
);

PROMPT
PROMPT ============================================================
PROMPT FIN DE LA VÉRIFICATION
PROMPT ============================================================

