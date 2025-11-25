-- =====================================================
-- Script SQL complet pour la gestion du Face ID
-- Exécuter ce script dans SQL Developer ou SQL*Plus
-- Connexion requise : SYSTEM (ou utilisateur avec privilèges)
-- =====================================================

SET SERVEROUTPUT ON;

-- =====================================================
-- PARTIE 1: CRÉATION DE LA TABLE FACE_ID
-- =====================================================

-- Supprimer la table si elle existe déjà
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE "SYSTEM"."FACE_ID" CASCADE CONSTRAINTS';
    DBMS_OUTPUT.PUT_LINE('✓ Table FACE_ID supprimée');
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE = -942 THEN
            DBMS_OUTPUT.PUT_LINE('ℹ Table FACE_ID n''existe pas encore');
        ELSE
            DBMS_OUTPUT.PUT_LINE('⚠ Erreur lors de la suppression: ' || SQLERRM);
        END IF;
END;
/

-- Supprimer la séquence si elle existe
BEGIN
    EXECUTE IMMEDIATE 'DROP SEQUENCE "SYSTEM"."SEQ_FACE_ID"';
    DBMS_OUTPUT.PUT_LINE('✓ Séquence SEQ_FACE_ID supprimée');
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE = -2289 THEN
            DBMS_OUTPUT.PUT_LINE('ℹ Séquence SEQ_FACE_ID n''existe pas encore');
        ELSE
            DBMS_OUTPUT.PUT_LINE('⚠ Erreur lors de la suppression séquence: ' || SQLERRM);
        END IF;
END;
/

-- Supprimer le trigger si il existe
BEGIN
    EXECUTE IMMEDIATE 'DROP TRIGGER "SYSTEM"."TRG_FACE_ID"';
    DBMS_OUTPUT.PUT_LINE('✓ Trigger TRG_FACE_ID supprimé');
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE = -4080 THEN
            DBMS_OUTPUT.PUT_LINE('ℹ Trigger TRG_FACE_ID n''existe pas encore');
        ELSE
            DBMS_OUTPUT.PUT_LINE('⚠ Erreur lors de la suppression trigger: ' || SQLERRM);
        END IF;
END;
/

-- Créer la table FACE_ID
CREATE TABLE "SYSTEM"."FACE_ID" (
    "ID" NUMBER PRIMARY KEY,
    "FACE_HASH" VARCHAR2(500) NOT NULL,
    "DATE_CREATION" DATE NOT NULL,
    "DATE_DERNIERE_UTILISATION" DATE
);

BEGIN
    DBMS_OUTPUT.PUT_LINE('✓ Table FACE_ID créée avec succès');
END;
/

-- Créer la séquence pour l'ID auto-incrémenté
CREATE SEQUENCE "SYSTEM"."SEQ_FACE_ID"
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

BEGIN
    DBMS_OUTPUT.PUT_LINE('✓ Séquence SEQ_FACE_ID créée avec succès');
END;
/

-- Créer le trigger pour auto-incrémenter l'ID
CREATE OR REPLACE TRIGGER "SYSTEM"."TRG_FACE_ID"
    BEFORE INSERT ON "SYSTEM"."FACE_ID"
    FOR EACH ROW
    WHEN (NEW."ID" IS NULL)
BEGIN
    SELECT "SYSTEM"."SEQ_FACE_ID".NEXTVAL INTO :NEW."ID" FROM DUAL;
END;
/

BEGIN
    DBMS_OUTPUT.PUT_LINE('✓ Trigger TRG_FACE_ID créé avec succès');
END;
/

-- Valider les changements
COMMIT;

BEGIN
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('========================================');
    DBMS_OUTPUT.PUT_LINE('✅ Table FACE_ID créée avec succès!');
    DBMS_OUTPUT.PUT_LINE('========================================');
    DBMS_OUTPUT.PUT_LINE('');
END;
/

-- =====================================================
-- PARTIE 2: PROCÉDURES POUR GÉRER LE FACE ID
-- =====================================================

-- Procédure pour sauvegarder un Face ID
-- Usage: EXEC SAVE_FACE_ID('votre_signature_face_id');
CREATE OR REPLACE PROCEDURE "SYSTEM"."SAVE_FACE_ID" (
    p_face_hash IN VARCHAR2
) AS
    v_count NUMBER;
BEGIN
    -- Supprimer l'ancien Face ID s'il existe
    DELETE FROM "SYSTEM"."FACE_ID";
    
    -- Insérer le nouveau Face ID
    INSERT INTO "SYSTEM"."FACE_ID" ("FACE_HASH", "DATE_CREATION")
    VALUES (p_face_hash, SYSDATE);
    
    COMMIT;
    
    DBMS_OUTPUT.PUT_LINE('✅ Face ID sauvegardé avec succès');
    DBMS_OUTPUT.PUT_LINE('   Date de création: ' || TO_CHAR(SYSDATE, 'DD/MM/YYYY HH24:MI:SS'));
EXCEPTION
    WHEN OTHERS THEN
        ROLLBACK;
        DBMS_OUTPUT.PUT_LINE('❌ Erreur lors de la sauvegarde: ' || SQLERRM);
        RAISE;
END;
/

-- Procédure pour supprimer le Face ID sauvegardé
-- Usage: EXEC DELETE_FACE_ID;
CREATE OR REPLACE PROCEDURE "SYSTEM"."DELETE_FACE_ID" AS
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count FROM "SYSTEM"."FACE_ID";
    
    IF v_count > 0 THEN
        DELETE FROM "SYSTEM"."FACE_ID";
        COMMIT;
        DBMS_OUTPUT.PUT_LINE('✅ Face ID supprimé avec succès');
        DBMS_OUTPUT.PUT_LINE('   Nombre d''enregistrements supprimés: ' || v_count);
    ELSE
        DBMS_OUTPUT.PUT_LINE('ℹ Aucun Face ID à supprimer');
    END IF;
EXCEPTION
    WHEN OTHERS THEN
        ROLLBACK;
        DBMS_OUTPUT.PUT_LINE('❌ Erreur lors de la suppression: ' || SQLERRM);
        RAISE;
END;
/

-- Fonction pour vérifier si un Face ID existe
-- Usage: SELECT CHECK_FACE_ID_EXISTS FROM DUAL;
CREATE OR REPLACE FUNCTION "SYSTEM"."CHECK_FACE_ID_EXISTS" 
RETURN NUMBER AS
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count FROM "SYSTEM"."FACE_ID";
    RETURN v_count;
END;
/

-- Fonction pour récupérer le Face ID sauvegardé
-- Usage: SELECT GET_FACE_ID FROM DUAL;
CREATE OR REPLACE FUNCTION "SYSTEM"."GET_FACE_ID" 
RETURN VARCHAR2 AS
    v_face_hash VARCHAR2(500);
BEGIN
    SELECT "FACE_HASH" INTO v_face_hash 
    FROM "SYSTEM"."FACE_ID" 
    WHERE ROWNUM = 1
    ORDER BY "DATE_CREATION" DESC;
    
    RETURN v_face_hash;
EXCEPTION
    WHEN NO_DATA_FOUND THEN
        RETURN NULL;
    WHEN OTHERS THEN
        RETURN NULL;
END;
/

BEGIN
    DBMS_OUTPUT.PUT_LINE('✓ Procédures et fonctions créées avec succès');
    DBMS_OUTPUT.PUT_LINE('');
END;
/

-- =====================================================
-- PARTIE 3: EXEMPLES D'UTILISATION
-- =====================================================

-- Afficher les instructions d'utilisation
BEGIN
    DBMS_OUTPUT.PUT_LINE('========================================');
    DBMS_OUTPUT.PUT_LINE('📖 INSTRUCTIONS D''UTILISATION');
    DBMS_OUTPUT.PUT_LINE('========================================');
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('1. Sauvegarder un Face ID:');
    DBMS_OUTPUT.PUT_LINE('   EXEC SAVE_FACE_ID(''votre_signature_ici'');');
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('2. Supprimer le Face ID:');
    DBMS_OUTPUT.PUT_LINE('   EXEC DELETE_FACE_ID;');
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('3. Vérifier si un Face ID existe:');
    DBMS_OUTPUT.PUT_LINE('   SELECT CHECK_FACE_ID_EXISTS FROM DUAL;');
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('4. Récupérer le Face ID sauvegardé:');
    DBMS_OUTPUT.PUT_LINE('   SELECT GET_FACE_ID FROM DUAL;');
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('5. Voir tous les Face ID:');
    DBMS_OUTPUT.PUT_LINE('   SELECT * FROM "SYSTEM"."FACE_ID";');
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('========================================');
END;
/

-- Vérifier la structure de la table
BEGIN
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('📊 Structure de la table FACE_ID:');
END;
/

SELECT 
    COLUMN_NAME AS "Colonne",
    DATA_TYPE AS "Type",
    DATA_LENGTH AS "Taille",
    NULLABLE AS "Nullable"
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'FACE_ID'
ORDER BY COLUMN_ID;

-- Message final
BEGIN
    DBMS_OUTPUT.PUT_LINE('');
    DBMS_OUTPUT.PUT_LINE('✅ Configuration terminée avec succès!');
    DBMS_OUTPUT.PUT_LINE('La table FACE_ID est prête à être utilisée.');
    DBMS_OUTPUT.PUT_LINE('La colonne FACE_HASH accepte jusqu''à 500 caractères.');
END;
/

