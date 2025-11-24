-- =====================================================
-- Table: FACE_ID (Face Recognition Storage)
-- =====================================================
-- This table stores face recognition data for authentication

CREATE TABLE "SYSTEM"."FACE_ID" (
    ID NUMBER PRIMARY KEY,
    FACE_HASH VARCHAR2(64) NOT NULL UNIQUE,
    DATE_CREATION DATE DEFAULT SYSDATE,
    DATE_DERNIERE_UTILISATION DATE
);

-- Create sequence for auto-increment ID
CREATE SEQUENCE "SYSTEM"."SEQ_FACE_ID"
    START WITH 1
    INCREMENT BY 1
    NOCACHE;

-- Create trigger to auto-increment ID
CREATE OR REPLACE TRIGGER "SYSTEM"."TRG_FACE_ID"
    BEFORE INSERT ON "SYSTEM"."FACE_ID"
    FOR EACH ROW
BEGIN
    IF :NEW.ID IS NULL THEN
        SELECT "SYSTEM"."SEQ_FACE_ID".NEXTVAL INTO :NEW.ID FROM DUAL;
    END IF;
END;
/

-- Add comments for documentation
COMMENT ON TABLE "SYSTEM"."FACE_ID" IS 'Table for storing face recognition data';
COMMENT ON COLUMN "SYSTEM"."FACE_ID".ID IS 'Unique identifier for the face record';
COMMENT ON COLUMN "SYSTEM"."FACE_ID".FACE_HASH IS 'SHA256 hash of the face image';
COMMENT ON COLUMN "SYSTEM"."FACE_ID".DATE_CREATION IS 'Date when the face was registered';
COMMENT ON COLUMN "SYSTEM"."FACE_ID".DATE_DERNIERE_UTILISATION IS 'Date when the face was last used for authentication';

