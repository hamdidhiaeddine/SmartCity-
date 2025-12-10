-- ============================================================
-- SCRIPT DE CORRECTION DE L'HISTORIQUE
-- À exécuter avec l'utilisateur HIBA dans SQL Developer
-- ============================================================

PROMPT ============================================================
PROMPT ÉTAPE 1: CRÉATION DE LA TABLE HISTORIQUE (si inexistante)
PROMPT ============================================================

-- Vérifier si la table existe
DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count
    FROM USER_TABLES
    WHERE TABLE_NAME = 'HISTORIQUE_RESIDENT';
    
    IF v_count = 0 THEN
        DBMS_OUTPUT.PUT_LINE('Table HISTORIQUE_RESIDENT inexistante, création...');
        
        EXECUTE IMMEDIATE '
        CREATE TABLE HISTORIQUE_RESIDENT (
            ID_HISTORIQUE NUMBER PRIMARY KEY,
            ID_RESIDENT NUMBER NOT NULL,
            ACTION VARCHAR2(50) NOT NULL,
            DATE_ACTION DATE DEFAULT SYSDATE NOT NULL,
            CONSTRAINT FK_HIST_RESIDENT FOREIGN KEY (ID_RESIDENT) 
                REFERENCES GEST_RESIDENT(ID) ON DELETE CASCADE
        )';
        
        DBMS_OUTPUT.PUT_LINE('✓ Table HISTORIQUE_RESIDENT créée');
    ELSE
        DBMS_OUTPUT.PUT_LINE('✓ Table HISTORIQUE_RESIDENT existe déjà');
    END IF;
END;
/

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 2: CRÉATION DE LA SÉQUENCE
PROMPT ============================================================

DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count
    FROM USER_SEQUENCES
    WHERE SEQUENCE_NAME = 'HISTORIQUE_RESIDENT_SEQ';
    
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE '
        CREATE SEQUENCE HISTORIQUE_RESIDENT_SEQ
            START WITH 1
            INCREMENT BY 1
            NOCACHE
            NOCYCLE';
        DBMS_OUTPUT.PUT_LINE('✓ Séquence HISTORIQUE_RESIDENT_SEQ créée');
    ELSE
        DBMS_OUTPUT.PUT_LINE('✓ Séquence HISTORIQUE_RESIDENT_SEQ existe déjà');
    END IF;
END;
/

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 3: CRÉATION DU SYNONYME
PROMPT ============================================================

DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count
    FROM USER_SYNONYMS
    WHERE SYNONYM_NAME = 'HIST_RESIDENT';
    
    IF v_count > 0 THEN
        EXECUTE IMMEDIATE 'DROP SYNONYM HIST_RESIDENT';
    END IF;
    
    EXECUTE IMMEDIATE 'CREATE SYNONYM HIST_RESIDENT FOR HISTORIQUE_RESIDENT';
    DBMS_OUTPUT.PUT_LINE('✓ Synonyme HIST_RESIDENT créé');
END;
/

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 4: SUPPRESSION DES ANCIENS TRIGGERS (si existants)
PROMPT ============================================================

BEGIN
    FOR trig IN (SELECT TRIGGER_NAME 
                 FROM USER_TRIGGERS 
                 WHERE TRIGGER_NAME LIKE 'TRG%HISTORIQUE%' 
                    OR TRIGGER_NAME LIKE 'TRG%HIST%RES%') LOOP
        EXECUTE IMMEDIATE 'DROP TRIGGER ' || trig.TRIGGER_NAME;
        DBMS_OUTPUT.PUT_LINE('✓ Trigger ' || trig.TRIGGER_NAME || ' supprimé');
    END LOOP;
END;
/

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 5: CRÉATION DES TRIGGERS AUTOMATIQUES
PROMPT ============================================================

PROMPT Création du trigger AFTER INSERT...
CREATE OR REPLACE TRIGGER TRG_AUTO_HISTORIQUE_RESIDENT
    AFTER INSERT ON GEST_RESIDENT
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
        'Ajout',
        SYSDATE
    );
END;
/

PROMPT Création du trigger AFTER UPDATE...
CREATE OR REPLACE TRIGGER TRG_AUTO_HISTORIQUE_RESIDENT_UPDATE
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

PROMPT Création du trigger AFTER DELETE...
CREATE OR REPLACE TRIGGER TRG_AUTO_HISTORIQUE_RESIDENT_DELETE
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

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 6: VÉRIFICATION FINALE
PROMPT ============================================================

SELECT 'Table: ' || TABLE_NAME AS VERIFICATION
FROM USER_TABLES
WHERE TABLE_NAME IN ('HISTORIQUE_RESIDENT', 'GEST_RESIDENT')
UNION ALL
SELECT 'Séquence: ' || SEQUENCE_NAME
FROM USER_SEQUENCES
WHERE SEQUENCE_NAME = 'HISTORIQUE_RESIDENT_SEQ'
UNION ALL
SELECT 'Trigger: ' || TRIGGER_NAME || ' [' || STATUS || ']'
FROM USER_TRIGGERS
WHERE TRIGGER_NAME LIKE 'TRG_AUTO_HISTORIQUE%'
ORDER BY 1;

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 7: TEST RAPIDE
PROMPT ============================================================

PROMPT Insertion d'un résident test...
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (88888, 'TEST', 'TRIGGER', TO_DATE('2000-01-01', 'YYYY-MM-DD'), 
        'Test', '+21600000000', 'test@test.com', 'Actif', 'Célibataire');

PROMPT Vérification de l'historique...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 88888;

PROMPT Modification du résident test...
UPDATE GEST_RESIDENT SET ADRESSE = 'Modifié' WHERE ID = 88888;

PROMPT Vérification de l'historique après modification...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 88888
ORDER BY DATE_ACTION;

PROMPT Suppression du résident test...
DELETE FROM GEST_RESIDENT WHERE ID = 88888;

PROMPT Vérification de l'historique après suppression...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 88888
ORDER BY DATE_ACTION;

PROMPT Nettoyage...
DELETE FROM HISTORIQUE_RESIDENT WHERE ID_RESIDENT = 88888;
COMMIT;

PROMPT
PROMPT ============================================================
PROMPT ✓✓✓ CONFIGURATION TERMINÉE ✓✓✓
PROMPT ============================================================
PROMPT 
PROMPT Si vous voyez 3 enregistrements ci-dessus (Ajout, Modification, Suppression),
PROMPT les triggers fonctionnent parfaitement!
PROMPT
PROMPT Vous pouvez maintenant tester dans l'application Qt.
PROMPT ============================================================
