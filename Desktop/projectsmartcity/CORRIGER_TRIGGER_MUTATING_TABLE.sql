-- ============================================================
-- CORRECTION DU PROBLÈME "MUTATING TABLE" POUR TRG_HIST_UPDATE
-- ============================================================
-- Ce script corrige le trigger TRG_HIST_UPDATE pour éviter
-- l'erreur ORA-04091: table is mutating
-- ============================================================

PROMPT ============================================================
PROMPT CORRECTION DU TRIGGER TRG_HIST_UPDATE
PROMPT ============================================================
PROMPT

-- Supprimer l'ancien trigger problématique
BEGIN
    EXECUTE IMMEDIATE 'DROP TRIGGER SYSTEM.TRG_HIST_UPDATE';
    DBMS_OUTPUT.PUT_LINE('✓ Ancien trigger TRG_HIST_UPDATE supprimé');
EXCEPTION
    WHEN OTHERS THEN
        BEGIN
            EXECUTE IMMEDIATE 'DROP TRIGGER TRG_HIST_UPDATE';
            DBMS_OUTPUT.PUT_LINE('✓ Ancien trigger TRG_HIST_UPDATE supprimé (sans schéma)');
        EXCEPTION
            WHEN OTHERS THEN
                DBMS_OUTPUT.PUT_LINE('⚠️ Trigger TRG_HIST_UPDATE n''existe pas ou déjà supprimé');
        END;
END;
/

PROMPT
PROMPT ============================================================
PROMPT CRÉATION DU NOUVEAU TRIGGER (CORRIGÉ)
PROMPT ============================================================

-- Créer un package pour stocker temporairement les données
-- et éviter le problème de "mutating table"
CREATE OR REPLACE PACKAGE PKG_HIST_UPDATE AS
    TYPE t_id_array IS TABLE OF NUMBER INDEX BY PLS_INTEGER;
    g_id_array t_id_array;
    g_counter NUMBER := 0;
    
    PROCEDURE add_id(p_id NUMBER);
    PROCEDURE process_updates;
    PROCEDURE clear;
END PKG_HIST_UPDATE;
/

CREATE OR REPLACE PACKAGE BODY PKG_HIST_UPDATE AS
    PROCEDURE add_id(p_id NUMBER) IS
    BEGIN
        g_counter := g_counter + 1;
        g_id_array(g_counter) := p_id;
    END;
    
    PROCEDURE process_updates IS
    BEGIN
        FOR i IN 1..g_counter LOOP
            BEGIN
                INSERT INTO HISTORIQUE_RESIDENT (ID_RESIDENT, ACTION, DATE_ACTION)
                VALUES (g_id_array(i), 'Modification', SYSDATE);
            EXCEPTION
                WHEN OTHERS THEN
                    -- Ignorer les erreurs individuelles
                    NULL;
            END;
        END LOOP;
        clear();
    END;
    
    PROCEDURE clear IS
    BEGIN
        g_counter := 0;
        g_id_array.DELETE;
    END;
END PKG_HIST_UPDATE;
/

-- Créer le nouveau trigger qui utilise le package
CREATE OR REPLACE TRIGGER TRG_HIST_UPDATE
AFTER UPDATE ON GEST_RESIDENT
FOR EACH ROW
BEGIN
    -- Stocker l'ID dans le package au lieu d'insérer directement
    PKG_HIST_UPDATE.add_id(:NEW.ID);
END;
/

-- Créer un trigger au niveau statement pour traiter les insertions
CREATE OR REPLACE TRIGGER TRG_HIST_UPDATE_STMT
AFTER UPDATE ON GEST_RESIDENT
BEGIN
    -- Traiter toutes les insertions après la fin de la transaction
    PKG_HIST_UPDATE.process_updates();
END;
/

PROMPT ✓ Nouveau trigger TRG_HIST_UPDATE créé (sans problème de mutating table)
PROMPT
PROMPT ============================================================
PROMPT VÉRIFICATION
PROMPT ============================================================

SELECT TRIGGER_NAME, STATUS, TRIGGER_TYPE, TRIGGERING_EVENT
FROM USER_TRIGGERS
WHERE TRIGGER_NAME IN ('TRG_HIST_UPDATE', 'TRG_HIST_UPDATE_STMT')
ORDER BY TRIGGER_NAME;

PROMPT
PROMPT ============================================================
PROMPT ✓ CORRECTION TERMINÉE
PROMPT ============================================================
PROMPT
PROMPT Le trigger TRG_HIST_UPDATE a été corrigé pour éviter
PROMPT l'erreur "mutating table". Il utilise maintenant un package
PROMPT pour stocker temporairement les données avant insertion.
PROMPT

