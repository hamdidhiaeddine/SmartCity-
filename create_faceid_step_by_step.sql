-- =====================================================
-- Script étape par étape pour créer la table FACE_ID
-- EXÉCUTEZ CHAQUE COMMANDE UNE PAR UNE
-- =====================================================

-- ÉTAPE 1 : Créer la table (Sélectionnez cette ligne et exécutez avec F9 ou le bouton "Execute Statement")
CREATE TABLE "SYSTEM"."FACE_ID" (
    ID NUMBER PRIMARY KEY,
    FACE_HASH VARCHAR2(64) NOT NULL UNIQUE,
    DATE_CREATION DATE DEFAULT SYSDATE,
    DATE_DERNIERE_UTILISATION DATE
);

-- ÉTAPE 2 : Vérifier que la table a été créée (exécutez cette requête)
SELECT COUNT(*) FROM USER_TABLES WHERE TABLE_NAME = 'FACE_ID';

-- ÉTAPE 3 : Créer la séquence (Sélectionnez cette ligne et exécutez avec F9)
CREATE SEQUENCE "SYSTEM"."SEQ_FACE_ID"
    START WITH 1
    INCREMENT BY 1
    NOCACHE;

-- ÉTAPE 4 : Créer le trigger (Sélectionnez TOUT le bloc BEGIN...END; et exécutez avec F9)
CREATE OR REPLACE TRIGGER "SYSTEM"."TRG_FACE_ID"
    BEFORE INSERT ON "SYSTEM"."FACE_ID"
    FOR EACH ROW
BEGIN
    IF :NEW.ID IS NULL THEN
        SELECT "SYSTEM"."SEQ_FACE_ID".NEXTVAL INTO :NEW.ID FROM DUAL;
    END IF;
END;
/

-- ÉTAPE 5 : Test final - Cette requête doit fonctionner maintenant
SELECT * FROM "SYSTEM"."FACE_ID";

