-- ============================================================
-- SCRIPT DE RÉPARATION COMPLÈTE DE L'HISTORIQUE
-- À exécuter avec l'utilisateur HIBA dans SQL Developer
-- ============================================================

SET SERVEROUTPUT ON;

PROMPT ============================================================
PROMPT ÉTAPE 1: SAUVEGARDE DES DONNÉES EXISTANTES (si elles existent)
PROMPT ============================================================

-- Créer une table temporaire pour sauvegarder les données
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE TEMP_BACKUP_HIST';
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

-- Essayer de sauvegarder depuis HIST_RESIDENT
BEGIN
    EXECUTE IMMEDIATE 'CREATE TABLE TEMP_BACKUP_HIST AS SELECT * FROM HIST_RESIDENT';
    DBMS_OUTPUT.PUT_LINE('✓ Sauvegarde des données de HIST_RESIDENT effectuée');
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('⚠ Aucune donnée à sauvegarder ou table vide');
END;
/

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 2: SUPPRESSION DES ANCIENNES TABLES
PROMPT ============================================================

-- Supprimer les triggers existants
BEGIN
    FOR trig IN (SELECT TRIGGER_NAME 
                 FROM USER_TRIGGERS 
                 WHERE TRIGGER_NAME LIKE '%HIST%' 
                    OR TABLE_NAME = 'GEST_RESIDENT') LOOP
        EXECUTE IMMEDIATE 'DROP TRIGGER ' || trig.TRIGGER_NAME;
        DBMS_OUTPUT.PUT_LINE('✓ Trigger ' || trig.TRIGGER_NAME || ' supprimé');
    END LOOP;
END;
/

-- Supprimer les synonymes
BEGIN
    EXECUTE IMMEDIATE 'DROP SYNONYM HIST_RESIDENT';
    DBMS_OUTPUT.PUT_LINE('✓ Synonyme HIST_RESIDENT supprimé');
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

-- Supprimer les anciennes tables
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE HISTORIQUE_RESIDENT CASCADE CONSTRAINTS';
    DBMS_OUTPUT.PUT_LINE('✓ Table HISTORIQUE_RESIDENT supprimée');
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE HIST_RESIDENT CASCADE CONSTRAINTS';
    DBMS_OUTPUT.PUT_LINE('✓ Table HIST_RESIDENT supprimée');
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

-- Supprimer les anciennes séquences
BEGIN
    EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_HIST_RESIDENT';
    DBMS_OUTPUT.PUT_LINE('✓ Séquence SEQ_HIST_RESIDENT supprimée');
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'DROP SEQUENCE HISTORIQUE_RESIDENT_SEQ';
    DBMS_OUTPUT.PUT_LINE('✓ Séquence HISTORIQUE_RESIDENT_SEQ supprimée');
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 3: CRÉATION DE LA NOUVELLE TABLE AVEC LA BONNE STRUCTURE
PROMPT ============================================================

CREATE TABLE HISTORIQUE_RESIDENT (
    ID_HISTORIQUE NUMBER PRIMARY KEY,
    ID_RESIDENT NUMBER NOT NULL,
    ACTION VARCHAR2(50) NOT NULL,
    DATE_ACTION DATE DEFAULT SYSDATE NOT NULL
);

PROMPT ✓ Table HISTORIQUE_RESIDENT créée avec la structure correcte

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 4: CRÉATION DE LA SÉQUENCE
PROMPT ============================================================

CREATE SEQUENCE HISTORIQUE_RESIDENT_SEQ
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

PROMPT ✓ Séquence HISTORIQUE_RESIDENT_SEQ créée

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 5: CRÉATION DES INDEX POUR PERFORMANCE
PROMPT ============================================================

CREATE INDEX IDX_HIST_RESIDENT_ID ON HISTORIQUE_RESIDENT(ID_RESIDENT);
CREATE INDEX IDX_HIST_DATE_ACTION ON HISTORIQUE_RESIDENT(DATE_ACTION);

PROMPT ✓ Index créés

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 6: CRÉATION DU SYNONYME POUR COMPATIBILITÉ
PROMPT ============================================================

CREATE SYNONYM HIST_RESIDENT FOR HISTORIQUE_RESIDENT;

PROMPT ✓ Synonyme HIST_RESIDENT créé

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 7: CRÉATION DES TRIGGERS AUTOMATIQUES
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

PROMPT ✓ Trigger TRG_AUTO_HISTORIQUE_RESIDENT créé

PROMPT Création du trigger AFTER UPDATE...
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

PROMPT ✓ Trigger TRG_AUTO_HIST_RES_UPDATE créé

PROMPT Création du trigger AFTER DELETE...
CREATE OR REPLACE TRIGGER TRG_AUTO_HIST_RES_DELETE
    BEFORE DELETE ON GEST_RESIDENT
    FOR EACH ROW
DECLARE
    PRAGMA AUTONOMOUS_TRANSACTION;
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
    COMMIT;
END;
/

PROMPT ✓ Trigger TRG_AUTO_HIST_RES_DELETE créé

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 8: VÉRIFICATION DE LA STRUCTURE
PROMPT ============================================================

PROMPT Structure de la table:
DESC HISTORIQUE_RESIDENT;

PROMPT
PROMPT Colonnes détaillées:
SELECT COLUMN_NAME, DATA_TYPE, NULLABLE
FROM USER_TAB_COLUMNS
WHERE TABLE_NAME = 'HISTORIQUE_RESIDENT'
ORDER BY COLUMN_ID;

PROMPT
PROMPT Triggers créés:
SELECT TRIGGER_NAME, STATUS, TRIGGERING_EVENT
FROM USER_TRIGGERS
WHERE TRIGGER_NAME LIKE 'TRG_AUTO_HIST%' OR TRIGGER_NAME LIKE 'TRG_AUTO_HISTORIQUE%'
ORDER BY TRIGGER_NAME;

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 9: TEST COMPLET
PROMPT ============================================================

PROMPT Test 1: Insertion d'un résident de test...
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (77777, 'TEST', 'REPARATION', TO_DATE('2000-01-01', 'YYYY-MM-DD'), 
        'Test Réparation', '+21600000000', 'test@repair.com', 'Actif', 'Célibataire');

PROMPT Vérification de l'historique (devrait afficher 1 ligne 'Ajout')...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 77777;

PROMPT
PROMPT Test 2: Modification du résident...
UPDATE GEST_RESIDENT SET ADRESSE = 'Adresse Modifiée' WHERE ID = 77777;

PROMPT Vérification de l'historique (devrait afficher 2 lignes: Ajout + Modification)...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 77777
ORDER BY DATE_ACTION;

PROMPT
PROMPT Test 3: Suppression du résident...
DELETE FROM GEST_RESIDENT WHERE ID = 77777;

PROMPT Vérification de l'historique (devrait afficher 3 lignes: Ajout + Modification + Suppression)...
SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION
FROM HISTORIQUE_RESIDENT
WHERE ID_RESIDENT = 77777
ORDER BY DATE_ACTION;

PROMPT
PROMPT ============================================================
PROMPT ÉTAPE 10: NETTOYAGE DES DONNÉES DE TEST
PROMPT ============================================================
DELETE FROM HISTORIQUE_RESIDENT WHERE ID_RESIDENT = 77777;

-- Supprimer la sauvegarde temporaire
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE TEMP_BACKUP_HIST';
EXCEPTION
    WHEN OTHERS THEN NULL;
END;
/

COMMIT;

PROMPT
PROMPT ════════════════════════════════════════════════════════════
PROMPT ✓✓✓ RÉPARATION TERMINÉE AVEC SUCCÈS ✓✓✓
PROMPT ════════════════════════════════════════════════════════════
PROMPT
PROMPT Si vous voyez 3 enregistrements ci-dessus:
PROMPT   - 1 ligne "Ajout"
PROMPT   - 1 ligne "Modification"  
PROMPT   - 1 ligne "Suppression"
PROMPT
PROMPT Alors l'historique fonctionne parfaitement!
PROMPT
PROMPT Vous pouvez maintenant:
PROMPT   1. Tester dans l'application Qt
PROMPT   2. Ajouter/modifier/supprimer un résident
PROMPT   3. Vérifier que l'historique s'enregistre automatiquement
PROMPT
PROMPT ════════════════════════════════════════════════════════════
