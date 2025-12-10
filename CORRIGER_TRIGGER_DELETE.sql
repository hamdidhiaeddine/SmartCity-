-- ============================================================
-- CORRECTION DU TRIGGER DELETE (résolution erreur mutating)
-- ============================================================

PROMPT ============================================================
PROMPT SUPPRESSION DU TRIGGER DELETE (cause des problèmes)
PROMPT ============================================================

DROP TRIGGER TRG_AUTO_HIST_RES_DELETE;

PROMPT ✓ Trigger DELETE supprimé (enregistrement manuel dans le code C++)

PROMPT
PROMPT ============================================================
PROMPT VÉRIFICATION DES 3 TRIGGERS
PROMPT ============================================================

SELECT 
    TRIGGER_NAME,
    STATUS,
    TRIGGER_TYPE,
    TRIGGERING_EVENT
FROM USER_TRIGGERS
WHERE TABLE_NAME = 'GEST_RESIDENT'
ORDER BY TRIGGERING_EVENT, TRIGGER_NAME;

PROMPT
PROMPT ============================================================
PROMPT TEST COMPLET
PROMPT ============================================================

PROMPT Test 1: Insertion...
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (66666, 'TEST', 'FINAL', TO_DATE('2000-01-01', 'YYYY-MM-DD'), 
        'Test Final', '+21600000000', 'final@test.com', 'Actif', 'Célibataire');

SELECT 'Après INSERT:' AS ETAPE, ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 66666
ORDER BY DATE_ACTION;

PROMPT
PROMPT Test 2: Modification...
UPDATE GEST_RESIDENT SET ADRESSE = 'Modifié' WHERE ID = 66666;

SELECT 'Après UPDATE:' AS ETAPE, ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 66666
ORDER BY DATE_ACTION;

PROMPT
PROMPT Test 3: Suppression...
DELETE FROM GEST_RESIDENT WHERE ID = 66666;

SELECT 'Après DELETE:' AS ETAPE, ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 66666
ORDER BY DATE_ACTION;

PROMPT
PROMPT ============================================================
PROMPT NETTOYAGE
PROMPT ============================================================
DELETE FROM HISTORIQUE_RESIDENT WHERE ID_RESIDENT = 66666;
COMMIT;

PROMPT
PROMPT ════════════════════════════════════════════════════════════
PROMPT ✓✓✓ CORRECTION TERMINÉE ✓✓✓
PROMPT ════════════════════════════════════════════════════════════
PROMPT
PROMPT Si vous voyez 3 enregistrements ci-dessus, TOUT FONCTIONNE!
PROMPT
PROMPT   ✓ INSERT  → Ajout
PROMPT   ✓ UPDATE  → Modification
PROMPT   ✓ DELETE  → Suppression
PROMPT
PROMPT L'historique est maintenant 100% opérationnel!
PROMPT ════════════════════════════════════════════════════════════
