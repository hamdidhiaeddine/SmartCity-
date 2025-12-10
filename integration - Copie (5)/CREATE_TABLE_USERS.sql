-- ============================================================
-- CRÉATION DE LA TABLE USERS POUR L'AUTHENTIFICATION
-- ============================================================

PROMPT ============================================================
PROMPT CRÉATION DE LA TABLE USERS
PROMPT ============================================================

-- Supprimer la table si elle existe
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE USERS CASCADE CONSTRAINTS';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -942 THEN
            RAISE;
        END IF;
END;
/

-- Créer la table USERS
CREATE TABLE USERS (
    ID_USER NUMBER PRIMARY KEY,
    LOGIN VARCHAR2(50) UNIQUE NOT NULL,
    MOT_DE_PASSE VARCHAR2(100) NOT NULL,
    NOM VARCHAR2(50),
    PRENOM VARCHAR2(50),
    EMAIL VARCHAR2(100),
    ROLE VARCHAR2(20) DEFAULT 'USER',
    DATE_CREATION DATE DEFAULT SYSDATE,
    ACTIF NUMBER(1) DEFAULT 1
);

PROMPT ✓ Table USERS créée

-- Créer une séquence pour les IDs
CREATE SEQUENCE SEQ_USERS START WITH 1 INCREMENT BY 1;

PROMPT ✓ Séquence SEQ_USERS créée

-- ============================================================
-- INSERTION DES UTILISATEURS PAR DÉFAUT
-- ============================================================

PROMPT Insertion des utilisateurs par défaut...

-- Administrateur
INSERT INTO USERS (ID_USER, LOGIN, MOT_DE_PASSE, NOM, PRENOM, EMAIL, ROLE, ACTIF)
VALUES (SEQ_USERS.NEXTVAL, 'admin', 'admin', 'Administrateur', 'Système', 'admin@smartcity.tn', 'ADMIN', 1);

-- Utilisateur hiba (connexion à la base)
INSERT INTO USERS (ID_USER, LOGIN, MOT_DE_PASSE, NOM, PRENOM, EMAIL, ROLE, ACTIF)
VALUES (SEQ_USERS.NEXTVAL, 'hiba', 'esprit18', 'HIBA', 'User', 'hiba@smartcity.tn', 'ADMIN', 1);

-- Utilisateur test
INSERT INTO USERS (ID_USER, LOGIN, MOT_DE_PASSE, NOM, PRENOM, EMAIL, ROLE, ACTIF)
VALUES (SEQ_USERS.NEXTVAL, 'user', 'user123', 'Test', 'User', 'user@smartcity.tn', 'USER', 1);

-- Gestionnaire
INSERT INTO USERS (ID_USER, LOGIN, MOT_DE_PASSE, NOM, PRENOM, EMAIL, ROLE, ACTIF)
VALUES (SEQ_USERS.NEXTVAL, 'gestionnaire', 'gest123', 'Gestionnaire', 'SmartCity', 'gest@smartcity.tn', 'MANAGER', 1);

COMMIT;

PROMPT ✓ 4 utilisateurs insérés

-- ============================================================
-- VÉRIFICATION
-- ============================================================

PROMPT
PROMPT ============================================================
PROMPT VÉRIFICATION DE LA TABLE USERS
PROMPT ============================================================

SELECT ID_USER, LOGIN, NOM, PRENOM, ROLE, ACTIF 
FROM USERS 
ORDER BY ID_USER;

PROMPT
PROMPT ════════════════════════════════════════════════════════════
PROMPT ✓✓✓ TABLE USERS CRÉÉE AVEC SUCCÈS ✓✓✓
PROMPT ════════════════════════════════════════════════════════════
PROMPT
PROMPT Utilisateurs disponibles:
PROMPT   - admin / admin (Administrateur)
PROMPT   - hiba / esprit18 (Administrateur)
PROMPT   - user / user123 (Utilisateur)
PROMPT   - gestionnaire / gest123 (Gestionnaire)
PROMPT
PROMPT IMPORTANT: Ces mots de passe sont en clair pour la démo.
PROMPT Pour la production, utilisez un hashage (ex: DBMS_CRYPTO).
PROMPT ════════════════════════════════════════════════════════════
