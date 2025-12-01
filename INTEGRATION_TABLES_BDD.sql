-- ============================================================
-- SCRIPT D'INTÉGRATION DES TABLES
-- Crée les tables EMPLOYES, VEHICULE, GEST_MAISON, GEST_ALERTES
-- dans le schéma HIBA pour l'intégration complète
-- ============================================================

PROMPT ============================================================
PROMPT CRÉATION DES TABLES POUR L'INTÉGRATION
PROMPT ============================================================

-- ============================================================
-- 1. TABLE EMPLOYES (Employés)
-- ============================================================

PROMPT Création de la table EMPLOYES...

-- Supprimer si existe
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE EMPLOYES CASCADE CONSTRAINTS';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -942 THEN
            RAISE;
        END IF;
END;
/

-- Créer la table
CREATE TABLE EMPLOYES (
    ID_EMPLOYE NUMBER PRIMARY KEY,
    NOM VARCHAR2(20),
    PRENOM VARCHAR2(20),
    EMAIL VARCHAR2(20),
    POSTE VARCHAR2(20),
    SALAIRE NUMBER,
    ADRESSE VARCHAR2(20),
    TELEPHONE NUMBER,
    ID_RES NUMBER -- FK vers GEST_RESIDENT (optionnel)
);

PROMPT ✓ Table EMPLOYES créée

-- ============================================================
-- 2. TABLE VEHICULE (Véhicules)
-- ============================================================

PROMPT Création de la table VEHICULE...

-- Supprimer la séquence si existe
BEGIN
    EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_VEHI';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -2289 THEN
            RAISE;
        END IF;
END;
/

-- Supprimer la table si existe
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE VEHICULE CASCADE CONSTRAINTS';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -942 THEN
            RAISE;
        END IF;
END;
/

-- Créer la séquence
CREATE SEQUENCE SEQ_VEHI START WITH 1 INCREMENT BY 1;

-- Créer la table
CREATE TABLE VEHICULE (
    ID_VEHI NUMBER PRIMARY KEY,
    IMMAT VARCHAR2(50),
    MARQUE VARCHAR2(50),
    MODELE VARCHAR2(50),
    TYPE VARCHAR2(50),
    ETAT VARCHAR2(50),
    SERVICE VARCHAR2(50),
    DATE_MAINT DATE
);

PROMPT ✓ Table VEHICULE et séquence SEQ_VEHI créées

-- ============================================================
-- 3. TABLE GEST_MAISON (Maisons)
-- ============================================================

PROMPT Création de la table GEST_MAISON...

-- Supprimer si existe
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE GEST_MAISON CASCADE CONSTRAINTS';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -942 THEN
            RAISE;
        END IF;
END;
/

-- Créer la table
CREATE TABLE GEST_MAISON (
    ID NUMBER PRIMARY KEY,
    ADRESSE VARCHAR2(200),
    SECURITE NUMBER,
    STATUS VARCHAR2(50),
    TYPE VARCHAR2(50),
    NBRDESPIECES NUMBER
);

PROMPT ✓ Table GEST_MAISON créée

-- ============================================================
-- 4. TABLE GEST_ALERTES (Alertes)
-- ============================================================

PROMPT Création de la table GEST_ALERTES...

-- Supprimer la séquence si existe
BEGIN
    EXECUTE IMMEDIATE 'DROP SEQUENCE GEST_ALERTES_SEQ';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -2289 THEN
            RAISE;
        END IF;
END;
/

-- Supprimer le trigger si existe
BEGIN
    EXECUTE IMMEDIATE 'DROP TRIGGER GEST_ALERTES_TRG';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -4080 THEN
            RAISE;
        END IF;
END;
/

-- Supprimer la table si existe
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE GEST_ALERTES CASCADE CONSTRAINTS';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -942 THEN
            RAISE;
        END IF;
END;
/

-- Créer la table
CREATE TABLE GEST_ALERTES (
    ID NUMBER PRIMARY KEY,
    ID_MAISON NUMBER NOT NULL,
    ZONE VARCHAR2(200),
    NIVEAU NUMBER,
    STATUT VARCHAR2(50) DEFAULT 'En attente',
    DATE_ALERTE DATE DEFAULT SYSDATE
);

-- Créer la séquence
CREATE SEQUENCE GEST_ALERTES_SEQ START WITH 1 INCREMENT BY 1;

-- Créer le trigger pour auto-increment
CREATE OR REPLACE TRIGGER GEST_ALERTES_TRG
BEFORE INSERT ON GEST_ALERTES
FOR EACH ROW
BEGIN
    IF :NEW.ID IS NULL THEN
        SELECT GEST_ALERTES_SEQ.NEXTVAL INTO :NEW.ID FROM DUAL;
    END IF;
END;
/

-- Créer les index
CREATE INDEX IDX_ALERTE_MAISON ON GEST_ALERTES(ID_MAISON);
CREATE INDEX IDX_ALERTE_STATUT ON GEST_ALERTES(STATUT);

PROMPT ✓ Table GEST_ALERTES, séquence et trigger créés

-- ============================================================
-- 5. DONNÉES DE TEST
-- ============================================================

PROMPT Insertion de données de test...

-- Employés de test
INSERT INTO EMPLOYES (ID_EMPLOYE, NOM, PRENOM, EMAIL, POSTE, SALAIRE, ADRESSE, TELEPHONE)
VALUES (1, 'Dupont', 'Jean', 'jdupont@test.com', 'Manager', 3500, '10 Rue Test', 21600000001);

INSERT INTO EMPLOYES (ID_EMPLOYE, NOM, PRENOM, EMAIL, POSTE, SALAIRE, ADRESSE, TELEPHONE)
VALUES (2, 'Martin', 'Sophie', 'smartin@test.com', 'Technicien', 2500, '20 Av Test', 21600000002);

-- Véhicules de test
INSERT INTO VEHICULE (ID_VEHI, IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT)
VALUES (SEQ_VEHI.NEXTVAL, '123TUN456', 'Peugeot', '208', 'Berline', 'Neuf', 'Transport', SYSDATE);

INSERT INTO VEHICULE (ID_VEHI, IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT)
VALUES (SEQ_VEHI.NEXTVAL, '789TUN012', 'Renault', 'Clio', 'Berline', 'Usé', 'Livraison', SYSDATE + 30);

-- Maisons de test
INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES)
VALUES (1, '5 Rue SmartCity', 3, 'Occupée', 'Villa', 5);

INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES)
VALUES (2, '12 Av Innovation', 2, 'Libre', 'Appartement', 3);

-- Alertes de test
INSERT INTO GEST_ALERTES (ID_MAISON, ZONE, NIVEAU, STATUT)
VALUES (1, 'Cuisine', 2, 'En attente');

INSERT INTO GEST_ALERTES (ID_MAISON, ZONE, NIVEAU, STATUT)
VALUES (2, 'Salon', 1, 'Traitée');

COMMIT;

PROMPT ✓ Données de test insérées

-- ============================================================
-- 6. VÉRIFICATION
-- ============================================================

PROMPT
PROMPT ============================================================
PROMPT VÉRIFICATION DES TABLES CRÉÉES
PROMPT ============================================================

SELECT 'EMPLOYES' AS TABLE_NAME, COUNT(*) AS NB_LIGNES FROM EMPLOYES
UNION ALL
SELECT 'VEHICULE', COUNT(*) FROM VEHICULE
UNION ALL
SELECT 'GEST_MAISON', COUNT(*) FROM GEST_MAISON
UNION ALL
SELECT 'GEST_ALERTES', COUNT(*) FROM GEST_ALERTES
UNION ALL
SELECT 'GEST_RESIDENT', COUNT(*) FROM GEST_RESIDENT
UNION ALL
SELECT 'HISTORIQUE_RESIDENT', COUNT(*) FROM HISTORIQUE_RESIDENT;

PROMPT
PROMPT ════════════════════════════════════════════════════════════
PROMPT ✓✓✓ INTÉGRATION RÉUSSIE ✓✓✓
PROMPT ════════════════════════════════════════════════════════════
PROMPT
PROMPT Tables créées:
PROMPT   - EMPLOYES (Gestion des employés)
PROMPT   - VEHICULE (Gestion des véhicules)
PROMPT   - GEST_MAISON (Gestion des maisons)
PROMPT   - GEST_ALERTES (Système d'alertes)
PROMPT
PROMPT Prochaines étapes:
PROMPT   1. Recompiler le projet Qt
PROMPT   2. Tester l'ajout/modification d'employés et véhicules
PROMPT   3. Vérifier l'historique des résidents
PROMPT ════════════════════════════════════════════════════════════
