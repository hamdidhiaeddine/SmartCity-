-- =====================================================
-- Table: JARDINS (Gardens Management)
-- =====================================================
-- This table stores information about gardens in the SmartCity system

CREATE TABLE JARDINS (
    IDJARDIN NUMBER PRIMARY KEY,
    EMPLACEMENT VARCHAR2(255) NOT NULL,
    SUPERFICIE VARCHAR2(100),
    TYPESOL VARCHAR2(100),
    TYPECHOIX VARCHAR2(100),
    DATE_CREATION DATE DEFAULT SYSDATE,
    DATE_MODIFICATION DATE
);

-- Create sequence for auto-increment ID
CREATE SEQUENCE SEQ_JARDIN
    START WITH 1
    INCREMENT BY 1
    NOCACHE;

-- Create trigger to auto-increment ID
CREATE OR REPLACE TRIGGER TRG_JARDIN_ID
    BEFORE INSERT ON JARDINS
    FOR EACH ROW
BEGIN
    IF :NEW.IDJARDIN IS NULL THEN
        SELECT SEQ_JARDIN.NEXTVAL INTO :NEW.IDJARDIN FROM DUAL;
    END IF;
END;
/

-- Create trigger for update timestamp
CREATE OR REPLACE TRIGGER TRG_JARDIN_UPDATE
    BEFORE UPDATE ON JARDINS
    FOR EACH ROW
BEGIN
    :NEW.DATE_MODIFICATION := SYSDATE;
END;
/

-- Add indexes for better performance
CREATE INDEX IDX_JARDIN_EMPLACEMENT ON JARDINS(EMPLACEMENT);
CREATE INDEX IDX_JARDIN_TYPECHOIX ON JARDINS(TYPECHOIX);

-- Add comments for documentation
COMMENT ON TABLE JARDINS IS 'Table for storing garden information in SmartCity';
COMMENT ON COLUMN JARDINS.IDJARDIN IS 'Unique identifier for the garden';
COMMENT ON COLUMN JARDINS.EMPLACEMENT IS 'Location/placement of the garden';
COMMENT ON COLUMN JARDINS.SUPERFICIE IS 'Area/size of the garden';
COMMENT ON COLUMN JARDINS.TYPESOL IS 'Type of soil in the garden';
COMMENT ON COLUMN JARDINS.TYPECHOIX IS 'Type of garden (Potager, Ornemental, etc.)';
COMMENT ON COLUMN JARDINS.DATE_CREATION IS 'Date when the garden record was created';
COMMENT ON COLUMN JARDINS.DATE_MODIFICATION IS 'Date when the garden record was last modified';

-- =====================================================
-- Sample Data (optional)
-- =====================================================
INSERT INTO JARDINS (EMPLACEMENT, SUPERFICIE, TYPESOL, TYPECHOIX) 
VALUES ('Nord-Est', '50m²', 'Argileux', 'Potager');

INSERT INTO JARDINS (EMPLACEMENT, SUPERFICIE, TYPESOL, TYPECHOIX) 
VALUES ('Entrée principale', '30m²', 'Sableux', 'Ornemental');

INSERT INTO JARDINS (EMPLACEMENT, SUPERFICIE, TYPESOL, TYPECHOIX) 
VALUES ('Derrière bâtiment', '100m²', 'Riche', 'Mixte');

COMMIT;

-- =====================================================
-- Verification Query
-- =====================================================
SELECT * FROM JARDINS;
