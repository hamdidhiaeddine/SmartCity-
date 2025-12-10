-- ============================================================
-- VÉRIFICATION DE LA STRUCTURE DE LA TABLE GEST_MAISON
-- ============================================================

PROMPT ============================================================
PROMPT VÉRIFICATION DE LA STRUCTURE GEST_MAISON
PROMPT ============================================================
PROMPT

-- 1. Vérifier si la table existe
PROMPT 1. Existence de la table...
SELECT 
    CASE 
        WHEN COUNT(*) > 0 THEN '✓ Table GEST_MAISON existe'
        ELSE '✗ Table GEST_MAISON n''existe PAS !'
    END AS STATUT
FROM USER_TABLES 
WHERE TABLE_NAME = 'GEST_MAISON';

PROMPT

-- 2. Structure de la table
PROMPT 2. Structure de la table GEST_MAISON:
SELECT 
    COLUMN_ID AS "#",
    COLUMN_NAME AS "Colonne",
    DATA_TYPE AS "Type",
    DATA_LENGTH AS "Longueur",
    NULLABLE AS "Nullable"
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_MAISON' 
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
WHERE TABLE_NAME = 'GEST_MAISON'
ORDER BY CONSTRAINT_TYPE, CONSTRAINT_NAME;

PROMPT

-- 4. Vérifier la colonne ID/ID_MAISON
PROMPT 4. Vérification de la colonne ID:
SELECT 
    COLUMN_NAME,
    DATA_TYPE,
    CASE 
        WHEN COLUMN_NAME = 'ID' THEN '✓ Utilise ID'
        WHEN COLUMN_NAME = 'ID_MAISON' THEN '✓ Utilise ID_MAISON'
        ELSE 'Colonne: ' || COLUMN_NAME
    END AS COMMENTAIRE
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_MAISON' 
AND (COLUMN_NAME = 'ID' OR COLUMN_NAME = 'ID_MAISON');

PROMPT

-- 5. Compter les maisons
PROMPT 5. Nombre de maisons dans la table:
SELECT COUNT(*) AS "Nombre de maisons" FROM GEST_MAISON;

PROMPT

-- 6. Afficher quelques exemples
PROMPT 6. Exemples de maisons (5 premiers):
SELECT * FROM (
    SELECT * FROM GEST_MAISON
    ORDER BY 
        CASE 
            WHEN EXISTS (SELECT 1 FROM USER_TAB_COLUMNS WHERE TABLE_NAME = 'GEST_MAISON' AND COLUMN_NAME = 'ID') 
            THEN (SELECT ID FROM GEST_MAISON WHERE ROWNUM = 1)
            ELSE (SELECT ID_MAISON FROM GEST_MAISON WHERE ROWNUM = 1)
        END DESC
) WHERE ROWNUM <= 5;

PROMPT
PROMPT ============================================================
PROMPT FIN DE LA VÉRIFICATION
PROMPT ============================================================

