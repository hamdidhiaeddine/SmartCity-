-- ============================================================
-- VÉRIFICATION DE LA CONTRAINTE CHK_JARDIN_TYPE
-- ============================================================

PROMPT ============================================================
PROMPT VÉRIFICATION DE LA CONTRAINTE CHK_JARDIN_TYPE
PROMPT ============================================================
PROMPT

-- Afficher la définition exacte de la contrainte
SELECT 
    CONSTRAINT_NAME,
    SEARCH_CONDITION,
    STATUS
FROM USER_CONSTRAINTS
WHERE CONSTRAINT_NAME = 'CHK_JARDIN_TYPE'
   OR CONSTRAINT_NAME LIKE '%JARDIN_TYPE%';

PROMPT
PROMPT ============================================================
PROMPT TEST DES VALEURS
PROMPT ============================================================

-- Tester chaque valeur acceptée
PROMPT Test avec 'Publique'...
BEGIN
    INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
    VALUES (9999, 'TEST', 1, 'TEST', 0, 'Publique');
    ROLLBACK;
    DBMS_OUTPUT.PUT_LINE('✓ ''Publique'' est accepté');
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('✗ ''Publique'' est rejeté: ' || SQLERRM);
END;
/

PROMPT Test avec 'Privee'...
BEGIN
    INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
    VALUES (9998, 'TEST', 1, 'TEST', 0, 'Privee');
    ROLLBACK;
    DBMS_OUTPUT.PUT_LINE('✓ ''Privee'' est accepté');
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('✗ ''Privee'' est rejeté: ' || SQLERRM);
END;
/

PROMPT Test avec 'Autres'...
BEGIN
    INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
    VALUES (9997, 'TEST', 1, 'TEST', 0, 'Autres');
    ROLLBACK;
    DBMS_OUTPUT.PUT_LINE('✓ ''Autres'' est accepté');
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('✗ ''Autres'' est rejeté: ' || SQLERRM);
END;
/

PROMPT
PROMPT ============================================================
PROMPT VALEURS ACTUELLES DANS LA TABLE
PROMPT ============================================================

SELECT 
    ID_JARDIN,
    TYPE_CHOIX,
    LENGTH(TYPE_CHOIX) AS LONGUEUR,
    DUMP(TYPE_CHOIX) AS DUMP_VALEUR
FROM GEST_JARDIN
ORDER BY ID_JARDIN;

PROMPT
PROMPT ============================================================
PROMPT FIN DE LA VÉRIFICATION
PROMPT ============================================================

