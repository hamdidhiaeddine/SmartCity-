-- =====================================================
-- Script simplifié pour créer la table FACE_ID
-- Exécutez ce script dans SQL Developer
-- =====================================================

-- Étape 1 : Créer la table (exécutez cette ligne)
CREATE TABLE "SYSTEM"."FACE_ID" (
    ID NUMBER PRIMARY KEY,
    FACE_HASH VARCHAR2(64) NOT NULL UNIQUE,
    DATE_CREATION DATE DEFAULT SYSDATE,
    DATE_DERNIERE_UTILISATION DATE
);

-- Étape 2 : Créer la séquence (exécutez cette ligne)
CREATE SEQUENCE "SYSTEM"."SEQ_FACE_ID"
    START WITH 1
    INCREMENT BY 1
    NOCACHE;

-- Étape 3 : Créer le trigger (exécutez cette ligne)
CREATE OR REPLACE TRIGGER "SYSTEM"."TRG_FACE_ID"
    BEFORE INSERT ON "SYSTEM"."FACE_ID"
    FOR EACH ROW
BEGIN
    IF :NEW.ID IS NULL THEN
        SELECT "SYSTEM"."SEQ_FACE_ID".NEXTVAL INTO :NEW.ID FROM DUAL;
    END IF;
END;
/

-- Vérification : Testez avec cette requête
-- SELECT * FROM "SYSTEM"."FACE_ID";

