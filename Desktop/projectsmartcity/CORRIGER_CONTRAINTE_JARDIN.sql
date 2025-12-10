-- ============================================================
-- CORRECTION DE LA CONTRAINTE CHK_JARDIN_TYPE
-- ============================================================
-- Ce script supprime et recrée la contrainte pour s'assurer
-- qu'elle accepte exactement les valeurs: 'Publique', 'Privee', 'Autres'
-- ============================================================

PROMPT ============================================================
PROMPT CORRECTION DE LA CONTRAINTE CHK_JARDIN_TYPE
PROMPT ============================================================
PROMPT

-- Supprimer l'ancienne contrainte si elle existe
BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE GEST_JARDIN DROP CONSTRAINT CHK_JARDIN_TYPE';
    DBMS_OUTPUT.PUT_LINE('✓ Ancienne contrainte CHK_JARDIN_TYPE supprimée');
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE = -2443 THEN
            DBMS_OUTPUT.PUT_LINE('⚠️ Contrainte CHK_JARDIN_TYPE n''existe pas');
        ELSE
            RAISE;
        END IF;
END;
/

-- Supprimer aussi si elle existe dans le schéma SYSTEM
BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE SYSTEM.GEST_JARDIN DROP CONSTRAINT CHK_JARDIN_TYPE';
    DBMS_OUTPUT.PUT_LINE('✓ Ancienne contrainte SYSTEM.CHK_JARDIN_TYPE supprimée');
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE = -2443 THEN
            DBMS_OUTPUT.PUT_LINE('⚠️ Contrainte SYSTEM.CHK_JARDIN_TYPE n''existe pas');
        ELSE
            -- Ignorer si la table n'existe pas dans SYSTEM
            NULL;
        END IF;
END;
/

PROMPT
PROMPT Création de la nouvelle contrainte...
PROMPT

-- Créer la nouvelle contrainte avec les valeurs exactes
ALTER TABLE GEST_JARDIN
ADD CONSTRAINT CHK_JARDIN_TYPE CHECK (
    TYPE_CHOIX = 'Publique' OR 
    TYPE_CHOIX = 'Privee' OR 
    TYPE_CHOIX = 'Autres'
);

PROMPT ✓ Nouvelle contrainte CHK_JARDIN_TYPE créée
PROMPT
PROMPT ============================================================
PROMPT VÉRIFICATION
PROMPT ============================================================

-- Afficher la nouvelle définition
SELECT 
    CONSTRAINT_NAME,
    SEARCH_CONDITION,
    STATUS
FROM USER_CONSTRAINTS
WHERE CONSTRAINT_NAME = 'CHK_JARDIN_TYPE';

PROMPT
PROMPT ============================================================
PROMPT TEST DES VALEURS
PROMPT ============================================================

-- Tester chaque valeur
PROMPT Test avec 'Publique'...
BEGIN
    INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
    VALUES (9999, 'TEST_PUBLIQUE', 1, 'TEST', 0, 'Publique');
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
    VALUES (9998, 'TEST_PRIVEE', 1, 'TEST', 0, 'Privee');
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
    VALUES (9997, 'TEST_AUTRES', 1, 'TEST', 0, 'Autres');
    ROLLBACK;
    DBMS_OUTPUT.PUT_LINE('✓ ''Autres'' est accepté');
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('✗ ''Autres'' est rejeté: ' || SQLERRM);
END;
/

PROMPT
PROMPT ============================================================
PROMPT ✓ CORRECTION TERMINÉE
PROMPT ============================================================
PROMPT
PROMPT La contrainte CHK_JARDIN_TYPE a été recréée avec les valeurs:
PROMPT   - 'Publique'
PROMPT   - 'Privee'
PROMPT   - 'Autres'
PROMPT

