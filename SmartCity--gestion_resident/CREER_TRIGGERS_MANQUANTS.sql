-- ============================================================
-- CRÉATION DES TRIGGERS MANQUANTS (UPDATE ET DELETE)
-- Noms courts pour éviter l'erreur ORA-00972
-- ============================================================

PROMPT ============================================================
PROMPT SUPPRESSION DES ANCIENS TRIGGERS (si existants)
PROMPT ============================================================

BEGIN
    EXECUTE IMMEDIATE 'DROP TRIGGER TRG_AUTO_HISTORIQUE_RESIDENT_UPDATE';
    DBMS_OUTPUT.PUT_LINE('✓ Ancien trigger UPDATE supprimé');
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'DROP TRIGGER TRG_AUTO_HISTORIQUE_RESIDENT_DELETE';
    DBMS_OUTPUT.PUT_LINE('✓ Ancien trigger DELETE supprimé');
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

PROMPT
PROMPT ============================================================
PROMPT CRÉATION DES NOUVEAUX TRIGGERS (noms courts)
PROMPT ============================================================

PROMPT Création du trigger UPDATE (nom court: TRG_AUTO_HIST_RES_UPDATE)...
CREATE OR REPLACE TRIGGER TRG_AUTO_HIST_RES_UPDATE
    AFTER UPDATE ON GEST_RESIDENT
    FOR EACH ROW
BEGIN
    INSERT INTO HISTORIQUE_RESIDENT (
        ID_HISTORIQUE,
        ID_RESIDENT,
        ACTION,
        DATE_ACTION
    ) VALUES (
        HISTORIQUE_RESIDENT_SEQ.NEXTVAL,
        :NEW.ID,
        'Modification',
        SYSDATE
    );
END;
/

PROMPT ✓ Trigger TRG_AUTO_HIST_RES_UPDATE créé avec succès

PROMPT
PROMPT Création du trigger DELETE (nom court: TRG_AUTO_HIST_RES_DELETE)...
CREATE OR REPLACE TRIGGER TRG_AUTO_HIST_RES_DELETE
    AFTER DELETE ON GEST_RESIDENT
    FOR EACH ROW
BEGIN
    INSERT INTO HISTORIQUE_RESIDENT (
        ID_HISTORIQUE,
        ID_RESIDENT,
        ACTION,
        DATE_ACTION
    ) VALUES (
        HISTORIQUE_RESIDENT_SEQ.NEXTVAL,
        :OLD.ID,
        'Suppression',
        SYSDATE
    );
END;
/

PROMPT ✓ Trigger TRG_AUTO_HIST_RES_DELETE créé avec succès

PROMPT
PROMPT ============================================================
PROMPT VÉRIFICATION DES 3 TRIGGERS
PROMPT ============================================================

SELECT 
    TRIGGER_NAME,
    STATUS,
    TRIGGERING_EVENT,
    TABLE_NAME
FROM USER_TRIGGERS
WHERE TABLE_NAME = 'GEST_RESIDENT'
ORDER BY TRIGGERING_EVENT, TRIGGER_NAME;

PROMPT
PROMPT ============================================================
PROMPT TEST COMPLET DES 3 TRIGGERS
PROMPT ============================================================

PROMPT Test 1: Insertion d'un résident...
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (88888, 'TEST', 'TRIGGER', TO_DATE('2000-01-01', 'YYYY-MM-DD'), 
        'Test', '+21600000000', 'test@test.com', 'Actif', 'Célibataire');

SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 88888
ORDER BY DATE_ACTION;

PROMPT
PROMPT Test 2: Modification...
UPDATE GEST_RESIDENT SET ADRESSE = 'Modifié' WHERE ID = 88888;

SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 88888
ORDER BY DATE_ACTION;

PROMPT
PROMPT Test 3: Suppression...
DELETE FROM GEST_RESIDENT WHERE ID = 88888;

SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 88888
ORDER BY DATE_ACTION;

PROMPT
PROMPT Nettoyage...
DELETE FROM HISTORIQUE_RESIDENT WHERE ID_RESIDENT = 88888;
COMMIT;

PROMPT
PROMPT ════════════════════════════════════════════════════════════
PROMPT ✓✓✓ TRIGGERS CRÉÉS ET TESTÉS AVEC SUCCÈS ✓✓✓
PROMPT ════════════════════════════════════════════════════════════
PROMPT
PROMPT Vous devriez voir 3 lignes dans chaque test ci-dessus:
PROMPT   1. Ajout
PROMPT   2. Modification
PROMPT   3. Suppression
PROMPT
PROMPT Si vous voyez les 3 lignes, tout fonctionne parfaitement!
PROMPT ════════════════════════════════════════════════════════════
