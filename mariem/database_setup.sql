-- ============================================================
-- Script de création de la base de données Smart City
-- Utilisateur Oracle: mariem
-- Base: XE (Oracle Express Edition)
-- Date: 6 décembre 2025
-- ============================================================

-- ============================================================
-- 1. SUPPRESSION DES TABLES EXISTANTES (si nécessaire)
-- ============================================================

-- Supprimer les tables dans l'ordre inverse des dépendances
DROP TABLE GEST_ALERTES CASCADE CONSTRAINTS;
DROP TABLE GEST_MAISON CASCADE CONSTRAINTS;

-- Supprimer les séquences si elles existent
DROP SEQUENCE SEQ_ALERTES;
DROP SEQUENCE SEQ_MAISON;

-- ============================================================
-- 2. CRÉATION DE LA TABLE GEST_MAISON
-- ============================================================

CREATE TABLE GEST_MAISON (
    ID              NUMBER PRIMARY KEY,
    ASRESSE         VARCHAR2(255) NOT NULL,  -- Note: garder l'orthographe du code existant
    SECURITE        NUMBER NOT NULL CHECK (SECURITE BETWEEN 1 AND 5),
    STATUS          VARCHAR2(50) DEFAULT 'Actif' CHECK (STATUS IN ('Actif', 'Inactif', 'En maintenance', 'Vendu')),
    TYPE            VARCHAR2(100),
    NBRDESPIECES    NUMBER DEFAULT 0,
    DATE_CREATION   DATE DEFAULT SYSDATE
);

-- Créer une séquence pour l'auto-incrémentation de l'ID
CREATE SEQUENCE SEQ_MAISON
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

-- Commentaires sur les colonnes
COMMENT ON TABLE GEST_MAISON IS 'Table des maisons de la smart city';
COMMENT ON COLUMN GEST_MAISON.ID IS 'Identifiant unique de la maison';
COMMENT ON COLUMN GEST_MAISON.ASRESSE IS 'Adresse complète de la maison';
COMMENT ON COLUMN GEST_MAISON.SECURITE IS 'Niveau de sécurité (1-5)';
COMMENT ON COLUMN GEST_MAISON.STATUS IS 'Statut actuel de la maison';
COMMENT ON COLUMN GEST_MAISON.TYPE IS 'Type de maison (villa, appartement, etc.)';
COMMENT ON COLUMN GEST_MAISON.NBRDESPIECES IS 'Nombre de pièces';

-- ============================================================
-- 3. CRÉATION DE LA TABLE GEST_ALERTES
-- ============================================================

CREATE TABLE GEST_ALERTES (
    ID              NUMBER PRIMARY KEY,
    ID_MAISON       NUMBER NOT NULL,
    ZONE            VARCHAR2(200),
    NIVEAU          NUMBER CHECK (NIVEAU BETWEEN 1 AND 5),
    STATUT          VARCHAR2(50) DEFAULT 'En attente' CHECK (STATUT IN ('En attente', 'En cours', 'Traitee', 'Resolue', 'Annulee')),
    DATE_ALERTE     DATE DEFAULT SYSDATE,
    DESCRIPTION     VARCHAR2(500),
    CONSTRAINT FK_ALERTE_MAISON FOREIGN KEY (ID_MAISON) REFERENCES GEST_MAISON(ID) ON DELETE CASCADE
);

-- Créer une séquence pour l'auto-incrémentation de l'ID
CREATE SEQUENCE SEQ_ALERTES
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

-- Commentaires sur les colonnes
COMMENT ON TABLE GEST_ALERTES IS 'Table des alertes de securite';
COMMENT ON COLUMN GEST_ALERTES.ID IS 'Identifiant unique de l alerte';
COMMENT ON COLUMN GEST_ALERTES.ID_MAISON IS 'Reference vers la maison concernee';
COMMENT ON COLUMN GEST_ALERTES.ZONE IS 'Zone geographique de l alerte';
COMMENT ON COLUMN GEST_ALERTES.NIVEAU IS 'Niveau de gravite (1-5)';
COMMENT ON COLUMN GEST_ALERTES.STATUT IS 'Etat de traitement de l alerte';

-- ============================================================
-- 4. CRÉATION DES INDEX POUR OPTIMISATION
-- ============================================================

-- Index sur les colonnes fréquemment recherchées
CREATE INDEX IDX_MAISON_SECURITE ON GEST_MAISON(SECURITE);
CREATE INDEX IDX_MAISON_STATUS ON GEST_MAISON(STATUS);
CREATE INDEX IDX_ALERTE_MAISON ON GEST_ALERTES(ID_MAISON);
CREATE INDEX IDX_ALERTE_STATUT ON GEST_ALERTES(STATUT);
CREATE INDEX IDX_ALERTE_ZONE ON GEST_ALERTES(ZONE);
CREATE INDEX IDX_ALERTE_DATE ON GEST_ALERTES(DATE_ALERTE);

-- ============================================================
-- 5. INSERTION DE DONNÉES DE TEST
-- ============================================================

-- Insertion de maisons
INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (SEQ_MAISON.NEXTVAL, '12 Avenue Mohamed V, Tunis', 3, 'Actif', 'Villa', 5);

INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (SEQ_MAISON.NEXTVAL, '45 Rue de la République, Ariana', 2, 'Actif', 'Appartement', 3);

INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (SEQ_MAISON.NEXTVAL, '78 Boulevard Habib Bourguiba, Sousse', 4, 'Actif', 'Villa', 7);

INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (SEQ_MAISON.NEXTVAL, '23 Rue Ibn Khaldoun, La Marsa', 5, 'Actif', 'Villa de luxe', 10);

INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (SEQ_MAISON.NEXTVAL, '56 Avenue de Carthage, Carthage', 1, 'En maintenance', 'Appartement', 2);

INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (SEQ_MAISON.NEXTVAL, '89 Rue de Palestine, Sfax', 3, 'Actif', 'Maison', 4);

INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (SEQ_MAISON.NEXTVAL, '34 Avenue de la Liberté, Bizerte', 2, 'Actif', 'Appartement', 3);

INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (SEQ_MAISON.NEXTVAL, '67 Rue des Jasmins, Hammamet', 4, 'Actif', 'Villa', 6);

-- Insertion d'alertes (en utilisant les IDs des maisons)
INSERT INTO GEST_ALERTES (ID, ID_MAISON, ZONE, NIVEAU, STATUT, DESCRIPTION) 
VALUES (SEQ_ALERTES.NEXTVAL, 1, 'Tunis Centre', 2, 'En attente', 'Detection de mouvement suspect');

INSERT INTO GEST_ALERTES (ID, ID_MAISON, ZONE, NIVEAU, STATUT, DESCRIPTION) 
VALUES (SEQ_ALERTES.NEXTVAL, 3, 'Sousse Nord', 3, 'En cours', 'Alarme incendie declenchee');

INSERT INTO GEST_ALERTES (ID, ID_MAISON, ZONE, NIVEAU, STATUT, DESCRIPTION) 
VALUES (SEQ_ALERTES.NEXTVAL, 4, 'La Marsa', 1, 'Traitee', 'Porte principale ouverte');

INSERT INTO GEST_ALERTES (ID, ID_MAISON, ZONE, NIVEAU, STATUT, DESCRIPTION) 
VALUES (SEQ_ALERTES.NEXTVAL, 2, 'Ariana', 4, 'En attente', 'Intrusion detectee');

INSERT INTO GEST_ALERTES (ID, ID_MAISON, ZONE, NIVEAU, STATUT, DESCRIPTION) 
VALUES (SEQ_ALERTES.NEXTVAL, 6, 'Sfax Centre', 2, 'En attente', 'Fenetre brisee signalee');

INSERT INTO GEST_ALERTES (ID, ID_MAISON, ZONE, NIVEAU, STATUT, DESCRIPTION) 
VALUES (SEQ_ALERTES.NEXTVAL, 8, 'Hammamet Zone Touristique', 5, 'En cours', 'Urgence securite - cambriolage');

-- Valider les insertions
COMMIT;

-- ============================================================
-- 6. CRÉATION DE VUES UTILES
-- ============================================================

-- Vue des maisons avec compteur d'alertes
CREATE OR REPLACE VIEW V_MAISONS_ALERTES AS
SELECT 
    m.ID,
    m.ASRESSE,
    m.SECURITE,
    m.STATUS,
    m.TYPE,
    m.NBRDESPIECES,
    COUNT(a.ID) AS NBR_ALERTES,
    COUNT(CASE WHEN a.STATUT = 'En attente' THEN 1 END) AS ALERTES_EN_ATTENTE
FROM GEST_MAISON m
LEFT JOIN GEST_ALERTES a ON m.ID = a.ID_MAISON
GROUP BY m.ID, m.ASRESSE, m.SECURITE, m.STATUS, m.TYPE, m.NBRDESPIECES;

-- Vue des alertes actives avec détails maison
CREATE OR REPLACE VIEW V_ALERTES_ACTIVES AS
SELECT 
    a.ID,
    a.ID_MAISON,
    m.ASRESSE,
    a.ZONE,
    a.NIVEAU,
    a.STATUT,
    a.DATE_ALERTE,
    a.DESCRIPTION,
    m.SECURITE AS SECURITE_MAISON
FROM GEST_ALERTES a
JOIN GEST_MAISON m ON a.ID_MAISON = m.ID
WHERE a.STATUT IN ('En attente', 'En cours')
ORDER BY a.NIVEAU DESC, a.DATE_ALERTE DESC;

-- ============================================================
-- 7. STATISTIQUES INITIALES
-- ============================================================

-- Afficher le nombre de maisons par niveau de sécurité
SELECT 
    SECURITE AS "Niveau Sécurité",
    COUNT(*) AS "Nombre de Maisons"
FROM GEST_MAISON
GROUP BY SECURITE
ORDER BY SECURITE;

-- Afficher le nombre d'alertes par statut
SELECT 
    STATUT AS "Statut Alerte",
    COUNT(*) AS "Nombre"
FROM GEST_ALERTES
GROUP BY STATUT
ORDER BY COUNT(*) DESC;

-- Afficher les zones les plus alertées
SELECT 
    ZONE AS "Zone",
    COUNT(*) AS "Nombre Alertes",
    AVG(NIVEAU) AS "Niveau Moyen"
FROM GEST_ALERTES
GROUP BY ZONE
ORDER BY COUNT(*) DESC;

-- ============================================================
-- 8. TRIGGERS POUR AUTO-INCREMENT (Alternative à IDENTITY)
-- ============================================================

-- Trigger pour GEST_MAISON
CREATE OR REPLACE TRIGGER TRG_MAISON_ID
BEFORE INSERT ON GEST_MAISON
FOR EACH ROW
BEGIN
    IF :NEW.ID IS NULL THEN
        SELECT SEQ_MAISON.NEXTVAL INTO :NEW.ID FROM DUAL;
    END IF;
END;
/

-- Trigger pour GEST_ALERTES
CREATE OR REPLACE TRIGGER TRG_ALERTE_ID
BEFORE INSERT ON GEST_ALERTES
FOR EACH ROW
BEGIN
    IF :NEW.ID IS NULL THEN
        SELECT SEQ_ALERTES.NEXTVAL INTO :NEW.ID FROM DUAL;
    END IF;
END;
/

-- ============================================================
-- 9. PROCÉDURES STOCKÉES UTILES
-- ============================================================

-- Procédure pour marquer une alerte comme traitée
CREATE OR REPLACE PROCEDURE TRAITER_ALERTE(p_id_alerte IN NUMBER)
IS
BEGIN
    UPDATE GEST_ALERTES 
    SET STATUT = 'Traitee',
        DATE_ALERTE = SYSDATE
    WHERE ID = p_id_alerte;
    
    COMMIT;
    
    DBMS_OUTPUT.PUT_LINE('Alerte ' || p_id_alerte || ' marquee comme traitee');
EXCEPTION
    WHEN OTHERS THEN
        ROLLBACK;
        DBMS_OUTPUT.PUT_LINE('Erreur: ' || SQLERRM);
END;
/

-- Procédure pour créer une nouvelle alerte
CREATE OR REPLACE PROCEDURE CREER_ALERTE(
    p_id_maison IN NUMBER,
    p_zone IN VARCHAR2,
    p_niveau IN NUMBER,
    p_description IN VARCHAR2 DEFAULT NULL
)
IS
BEGIN
    INSERT INTO GEST_ALERTES (ID, ID_MAISON, ZONE, NIVEAU, STATUT, DESCRIPTION)
    VALUES (SEQ_ALERTES.NEXTVAL, p_id_maison, p_zone, p_niveau, 'En attente', p_description);
    
    COMMIT;
    
    DBMS_OUTPUT.PUT_LINE('Alerte creee avec succes pour la maison ' || p_id_maison);
EXCEPTION
    WHEN OTHERS THEN
        ROLLBACK;
        DBMS_OUTPUT.PUT_LINE('Erreur: ' || SQLERRM);
END;
/

-- ============================================================
-- FIN DU SCRIPT
-- ============================================================

-- Afficher un résumé final
SELECT 'Base de donnees Smart City creee avec succes!' AS MESSAGE FROM DUAL;
SELECT COUNT(*) || ' maisons inserees' AS INFO FROM GEST_MAISON;
SELECT COUNT(*) || ' alertes inserees' AS INFO FROM GEST_ALERTES;
