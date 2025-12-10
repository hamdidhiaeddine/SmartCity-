-- ============================================================
-- SCRIPT DE CORRECTION DE LA TABLE GEST_VEHICULE
-- ============================================================
-- Ce script corrige la structure de la table pour permettre
-- l'utilisation d'immatriculations avec des lettres (ex: AB-123-CD)
-- ============================================================

PROMPT ============================================================
PROMPT CORRECTION DE LA TABLE GEST_VEHICULE
PROMPT ============================================================
PROMPT

-- Vérifier la structure actuelle
PROMPT Vérification de la structure actuelle...
SELECT COLUMN_NAME, DATA_TYPE, DATA_LENGTH 
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
AND COLUMN_NAME = 'IMMATRICULATION';

PROMPT
PROMPT ============================================================
PROMPT CORRECTION 1: IMMATRICULATION en VARCHAR2
PROMPT ============================================================

-- Sauvegarder les données existantes
CREATE TABLE GEST_VEHICULE_BACKUP AS SELECT * FROM GEST_VEHICULE;

PROMPT ✓ Données sauvegardées dans GEST_VEHICULE_BACKUP

-- Supprimer la contrainte de clé primaire
BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE GEST_VEHICULE DROP CONSTRAINT SYS_C0012345';
EXCEPTION
    WHEN OTHERS THEN
        BEGIN
            EXECUTE IMMEDIATE 'ALTER TABLE GEST_VEHICULE DROP PRIMARY KEY';
        EXCEPTION
            WHEN OTHERS THEN
                DBMS_OUTPUT.PUT_LINE('Contrainte PK supprimée ou inexistante');
        END;
END;
/

-- Supprimer les contraintes de clé étrangère si elles existent
BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE GEST_VEHICULE DROP CONSTRAINT FK_VEHICULE_RESIDENT';
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Contrainte FK supprimée ou inexistante');
END;
/

-- Supprimer la table
DROP TABLE GEST_VEHICULE CASCADE CONSTRAINTS;

-- Recréer la table avec IMMATRICULATION en VARCHAR2
CREATE TABLE GEST_VEHICULE (
    IMMATRICULATION VARCHAR2(20) PRIMARY KEY,
    MARQUE VARCHAR2(50),
    MODELE VARCHAR2(50),
    TYPE VARCHAR2(50),
    ETAT VARCHAR2(50),
    SERVICE VARCHAR2(100),
    DATEMAINTENCE DATE,
    ID_RESIDENT NUMBER(10),
    CONSTRAINT FK_VEHICULE_RESIDENT FOREIGN KEY (ID_RESIDENT) 
        REFERENCES GEST_RESIDENT(ID) ON DELETE CASCADE
);

PROMPT ✓ Table GEST_VEHICULE recréée avec IMMATRICULATION VARCHAR2(20)

-- Restaurer les données (si elles étaient numériques, elles seront converties en texte)
INSERT INTO GEST_VEHICULE 
SELECT 
    TO_CHAR(IMMATRICULATION) AS IMMATRICULATION,
    MARQUE,
    MODELE,
    TYPE,
    ETAT,
    SERVICE,
    DATEMAINTENCE,
    ID_RESIDENT
FROM GEST_VEHICULE_BACKUP;

PROMPT ✓ Données restaurées

-- Supprimer la sauvegarde
DROP TABLE GEST_VEHICULE_BACKUP;

PROMPT
PROMPT ============================================================
PROMPT CORRECTION 2: Vérification du nom de la colonne DATE
PROMPT ============================================================

-- Vérifier si la colonne s'appelle DATEMAINTENCE ou DATE_MAINTENANCE
SELECT COLUMN_NAME 
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
AND COLUMN_NAME LIKE '%MAINT%';

PROMPT
PROMPT ============================================================
PROMPT VÉRIFICATION FINALE
PROMPT ============================================================

SELECT 
    'Structure de la table:' AS INFO,
    COLUMN_NAME,
    DATA_TYPE,
    DATA_LENGTH,
    NULLABLE
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'GEST_VEHICULE' 
ORDER BY COLUMN_ID;

PROMPT
PROMPT ============================================================
PROMPT ✓ CORRECTION TERMINÉE
PROMPT ============================================================
PROMPT
PROMPT La table GEST_VEHICULE utilise maintenant:
PROMPT   - IMMATRICULATION: VARCHAR2(20) (peut contenir des lettres)
PROMPT   - DATEMAINTENCE: DATE
PROMPT
PROMPT Vous pouvez maintenant ajouter des véhicules avec des
PROMPT immatriculations comme "AB-123-CD", "EF-456-GH", etc.
PROMPT

