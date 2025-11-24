-- =====================================================
-- Table: FACE_ID (Face Recognition Storage)
-- Version alternative : Crée la table dans le schéma de l'utilisateur connecté
-- =====================================================
-- This table stores face recognition data for authentication

-- Créer la table dans le schéma de l'utilisateur connecté
CREATE TABLE FACE_ID (
    ID NUMBER PRIMARY KEY,
    FACE_HASH VARCHAR2(64) NOT NULL UNIQUE,
    DATE_CREATION DATE DEFAULT SYSDATE,
    DATE_DERNIERE_UTILISATION DATE
);

-- Create sequence for auto-increment ID
CREATE SEQUENCE SEQ_FACE_ID
    START WITH 1
    INCREMENT BY 1
    NOCACHE;

-- Create trigger to auto-increment ID
CREATE OR REPLACE TRIGGER TRG_FACE_ID
    BEFORE INSERT ON FACE_ID
    FOR EACH ROW
BEGIN
    IF :NEW.ID IS NULL THEN
        SELECT SEQ_FACE_ID.NEXTVAL INTO :NEW.ID FROM DUAL;
    END IF;
END;
/

-- Add comments for documentation
COMMENT ON TABLE FACE_ID IS 'Table for storing face recognition data';
COMMENT ON COLUMN FACE_ID.ID IS 'Unique identifier for the face record';
COMMENT ON COLUMN FACE_ID.FACE_HASH IS 'SHA256 hash of the face image';
COMMENT ON COLUMN FACE_ID.DATE_CREATION IS 'Date when the face was registered';
COMMENT ON COLUMN FACE_ID.DATE_DERNIERE_UTILISATION IS 'Date when the face was last used for authentication';

