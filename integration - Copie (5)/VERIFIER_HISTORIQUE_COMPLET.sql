-- ============================================================
-- SCRIPT DE VÉRIFICATION COMPLÈTE DE L'HISTORIQUE
-- À exécuter avec l'utilisateur HIBA dans SQL Developer
-- ============================================================

PROMPT ============================================================
PROMPT 1. VÉRIFICATION DE L'UTILISATEUR CONNECTÉ
PROMPT ============================================================
SELECT USER AS UTILISATEUR_CONNECTE FROM DUAL;

PROMPT
PROMPT ============================================================
PROMPT 2. VÉRIFICATION DE LA TABLE GEST_RESIDENT
PROMPT ============================================================
SELECT TABLE_NAME, NUM_ROWS, OWNER
FROM ALL_TABLES 
WHERE TABLE_NAME = 'GEST_RESIDENT'
  AND OWNER = 'HIBA';

SELECT COUNT(*) AS NOMBRE_RESIDENTS FROM HIBA.GEST_RESIDENT;

PROMPT
PROMPT ============================================================
PROMPT 3. VÉRIFICATION DE LA TABLE HISTORIQUE
PROMPT ============================================================
SELECT TABLE_NAME, NUM_ROWS, OWNER
FROM ALL_TABLES 
WHERE TABLE_NAME IN ('HIST_RESIDENT', 'HISTORIQUE_RESIDENT')
  AND OWNER = 'HIBA';

-- Essayer de compter les enregistrements
BEGIN
    EXECUTE IMMEDIATE 'SELECT COUNT(*) FROM HIBA.HISTORIQUE_RESIDENT' 
    INTO :v_count;
    DBMS_OUTPUT.PUT_LINE('Nombre d''enregistrements dans HISTORIQUE_RESIDENT: ' || :v_count);
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('ERREUR: La table HISTORIQUE_RESIDENT n''existe pas!');
END;
/

PROMPT
PROMPT ============================================================
PROMPT 4. VÉRIFICATION DES TRIGGERS
PROMPT ============================================================
SELECT 
    TRIGGER_NAME,
    STATUS,
    TRIGGER_TYPE,
    TRIGGERING_EVENT,
    TABLE_NAME,
    TABLE_OWNER
FROM ALL_TRIGGERS
WHERE (TABLE_NAME = 'GEST_RESIDENT' OR TRIGGER_NAME LIKE '%HISTORIQUE%')
  AND TABLE_OWNER = 'HIBA'
ORDER BY TRIGGER_NAME;

PROMPT
PROMPT ============================================================
PROMPT 5. VÉRIFICATION DES SÉQUENCES
PROMPT ============================================================
SELECT SEQUENCE_NAME, LAST_NUMBER, INCREMENT_BY, SEQUENCE_OWNER
FROM ALL_SEQUENCES
WHERE SEQUENCE_NAME LIKE '%HISTORIQUE%'
   OR SEQUENCE_NAME LIKE '%HIST_RESIDENT%'
ORDER BY SEQUENCE_NAME;

PROMPT
PROMPT ============================================================
PROMPT 6. VÉRIFICATION DU SYNONYME
PROMPT ============================================================
SELECT SYNONYM_NAME, TABLE_OWNER, TABLE_NAME
FROM ALL_SYNONYMS
WHERE SYNONYM_NAME = 'HIST_RESIDENT'
  AND OWNER = 'HIBA';

PROMPT
PROMPT ============================================================
PROMPT 7. TEST D'INSERTION MANUELLE
PROMPT ============================================================
PROMPT Test 1: Insertion d'un résident
INSERT INTO HIBA.GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (99999, 'TEST', 'HISTORIQUE', TO_DATE('2000-01-01', 'YYYY-MM-DD'), 
        'Adresse Test', '+21612345678', 'test@test.com', 'Actif', 'Célibataire');

PROMPT Vérification si le trigger a créé un enregistrement d'historique...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, DATE_ACTION
FROM HIBA.HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 99999;

PROMPT
PROMPT Test 2: Modification du résident
UPDATE HIBA.GEST_RESIDENT 
SET ADRESSE = 'Nouvelle Adresse'
WHERE ID = 99999;

PROMPT Vérification si le trigger de modification a fonctionné...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, DATE_ACTION
FROM HIBA.HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 99999
ORDER BY DATE_ACTION;

PROMPT
PROMPT Test 3: Suppression du résident
DELETE FROM HIBA.GEST_RESIDENT WHERE ID = 99999;

PROMPT Vérification si le trigger de suppression a fonctionné...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, DATE_ACTION
FROM HIBA.HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 99999
ORDER BY DATE_ACTION;

PROMPT
PROMPT ============================================================
PROMPT 8. NETTOYAGE DES DONNÉES DE TEST
PROMPT ============================================================
DELETE FROM HIBA.HISTORIQUE_RESIDENT WHERE ID_RESIDENT = 99999;
COMMIT;

PROMPT
PROMPT ============================================================
PROMPT 9. RÉSUMÉ FINAL
PROMPT ============================================================
PROMPT Si vous voyez 3 enregistrements (Ajout, Modification, Suppression)
PROMPT dans le test ci-dessus, les triggers fonctionnent correctement!
PROMPT
PROMPT Sinon, exécutez: create_trigger_historique.sql
PROMPT ============================================================
