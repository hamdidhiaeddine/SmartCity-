-- Script SQL pour créer le trigger d'ajout automatique dans l'historique
-- Base de données Oracle

-- Supprimer le trigger s'il existe déjà
DROP TRIGGER HIBA.TRG_AUTO_HISTORIQUE_RESIDENT;

-- Créer le trigger AFTER INSERT sur la table GEST_RESIDENT
CREATE OR REPLACE TRIGGER HIBA.TRG_AUTO_HISTORIQUE_RESIDENT
    AFTER INSERT ON HIBA.GEST_RESIDENT
    FOR EACH ROW
BEGIN
    -- Insérer automatiquement une ligne dans HISTORIQUE_RESIDENT
    INSERT INTO HIBA.HISTORIQUE_RESIDENT (
        ID_HISTORIQUE,
        ID_RESIDENT,
        ACTION,
        DATE_ACTION
    ) VALUES (
        HIBA.HISTORIQUE_RESIDENT_SEQ.NEXTVAL,
        :NEW.ID,
        'Ajout',
        SYSDATE
    );
END;
/

-- Créer également un trigger pour la modification
CREATE OR REPLACE TRIGGER HIBA.TRG_AUTO_HISTORIQUE_RESIDENT_UPDATE
    AFTER UPDATE ON HIBA.GEST_RESIDENT
    FOR EACH ROW
BEGIN
    -- Insérer automatiquement une ligne dans HISTORIQUE_RESIDENT lors de la modification
    INSERT INTO HIBA.HISTORIQUE_RESIDENT (
        ID_HISTORIQUE,
        ID_RESIDENT,
        ACTION,
        DATE_ACTION
    ) VALUES (
        HIBA.HISTORIQUE_RESIDENT_SEQ.NEXTVAL,
        :NEW.ID,
        'Modification',
        SYSDATE
    );
END;
/

-- Créer également un trigger pour la suppression
CREATE OR REPLACE TRIGGER HIBA.TRG_AUTO_HISTORIQUE_RESIDENT_DELETE
    AFTER DELETE ON HIBA.GEST_RESIDENT
    FOR EACH ROW
BEGIN
    -- Insérer automatiquement une ligne dans HISTORIQUE_RESIDENT lors de la suppression
    INSERT INTO HIBA.HISTORIQUE_RESIDENT (
        ID_HISTORIQUE,
        ID_RESIDENT,
        ACTION,
        DATE_ACTION
    ) VALUES (
        HIBA.HISTORIQUE_RESIDENT_SEQ.NEXTVAL,
        :OLD.ID,
        'Suppression',
        SYSDATE
    );
END;
/

-- Afficher un message de confirmation
SELECT 'Triggers créés avec succès!' AS MESSAGE FROM DUAL;

