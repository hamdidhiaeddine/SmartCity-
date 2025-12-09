-- ========================================
-- SCRIPT SQL - BASE DE DONNÉES SMARTCITY
-- Utilisateur: wala
-- Mot de passe: esprit18
-- ========================================

-- 1. Créer l'utilisateur (à exécuter en tant que SYSTEM ou DBA)
-- Si l'utilisateur existe déjà, cette étape sera ignorée
BEGIN
    EXECUTE IMMEDIATE 'CREATE USER wala IDENTIFIED BY esprit18';
    DBMS_OUTPUT.PUT_LINE('Utilisateur wala créé avec succès');
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE = -1920 THEN
            DBMS_OUTPUT.PUT_LINE('Utilisateur wala existe déjà');
        ELSE
            RAISE;
        END IF;
END;
/

-- 2. Accorder les privilèges nécessaires
GRANT CONNECT, RESOURCE TO wala;
GRANT CREATE SESSION TO wala;
GRANT CREATE TABLE TO wala;
GRANT CREATE SEQUENCE TO wala;
GRANT UNLIMITED TABLESPACE TO wala;

-- ========================================
-- Se connecter avec l'utilisateur wala
-- ========================================

-- 3. Créer la séquence pour l'auto-incrémentation
DECLARE
    seq_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO seq_count 
    FROM user_sequences 
    WHERE sequence_name = 'SEQ_VEHI';
    
    IF seq_count = 0 THEN
        EXECUTE IMMEDIATE 'CREATE SEQUENCE SEQ_VEHI START WITH 1 INCREMENT BY 1 NOCACHE NOCYCLE';
        DBMS_OUTPUT.PUT_LINE('Séquence SEQ_VEHI créée');
    ELSE
        DBMS_OUTPUT.PUT_LINE('Séquence SEQ_VEHI existe déjà');
    END IF;
END;
/

-- 4. Créer la table VEHICULE
DECLARE
    table_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO table_count 
    FROM user_tables 
    WHERE table_name = 'VEHICULE';
    
    IF table_count = 0 THEN
        EXECUTE IMMEDIATE 'CREATE TABLE VEHICULE (
            ID_VEHI       NUMBER PRIMARY KEY,
            IMMAT         VARCHAR2(20) NOT NULL UNIQUE,
            MARQUE        VARCHAR2(50) NOT NULL,
            MODELE        VARCHAR2(50) NOT NULL,
            TYPE          VARCHAR2(30) NOT NULL,
            ETAT          VARCHAR2(20) NOT NULL CHECK (ETAT IN (''Neuf'', ''Use'', ''En panne'')),
            SERVICE       VARCHAR2(50) NOT NULL,
            DATE_MAINT    DATE NOT NULL
        )';
        DBMS_OUTPUT.PUT_LINE('Table VEHICULE créée');
    ELSE
        DBMS_OUTPUT.PUT_LINE('Table VEHICULE existe déjà');
    END IF;
END;
/

-- 5. Créer des index pour améliorer les performances
DECLARE
    PROCEDURE create_index_if_not_exists(p_index_name VARCHAR2, p_create_sql VARCHAR2) IS
        idx_count NUMBER;
    BEGIN
        SELECT COUNT(*) INTO idx_count 
        FROM user_indexes 
        WHERE index_name = p_index_name;
        
        IF idx_count = 0 THEN
            EXECUTE IMMEDIATE p_create_sql;
            DBMS_OUTPUT.PUT_LINE('Index ' || p_index_name || ' créé');
        ELSE
            DBMS_OUTPUT.PUT_LINE('Index ' || p_index_name || ' existe déjà');
        END IF;
    END;
BEGIN
    -- Note: Un index sur IMMAT existe déjà automatiquement (UNIQUE constraint)
    create_index_if_not_exists('IDX_VEHICULE_TYPE', 'CREATE INDEX idx_vehicule_type ON VEHICULE(TYPE)');
    create_index_if_not_exists('IDX_VEHICULE_ETAT', 'CREATE INDEX idx_vehicule_etat ON VEHICULE(ETAT)');
    create_index_if_not_exists('IDX_VEHICULE_DATE', 'CREATE INDEX idx_vehicule_date ON VEHICULE(DATE_MAINT)');
END;
/

-- 6. Ajouter des données de test (uniquement si la table est vide)
DECLARE
    row_count NUMBER;
    PROCEDURE insert_if_not_exists(p_immat VARCHAR2, p_marque VARCHAR2, p_modele VARCHAR2, 
                                    p_type VARCHAR2, p_etat VARCHAR2, p_service VARCHAR2, p_date VARCHAR2) IS
        immat_count NUMBER;
    BEGIN
        SELECT COUNT(*) INTO immat_count FROM VEHICULE WHERE IMMAT = p_immat;
        IF immat_count = 0 THEN
            EXECUTE IMMEDIATE 'INSERT INTO VEHICULE (ID_VEHI, IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT)
                              VALUES (SEQ_VEHI.NEXTVAL, :1, :2, :3, :4, :5, :6, TO_DATE(:7, ''YYYY-MM-DD''))'
                              USING p_immat, p_marque, p_modele, p_type, p_etat, p_service, p_date;
            DBMS_OUTPUT.PUT_LINE('Véhicule ' || p_immat || ' inséré');
        ELSE
            DBMS_OUTPUT.PUT_LINE('Véhicule ' || p_immat || ' existe déjà');
        END IF;
    END;
BEGIN
    SELECT COUNT(*) INTO row_count FROM VEHICULE;
    
    IF row_count = 0 THEN
        insert_if_not_exists('123-TN-456', 'Renault', 'Clio', 'Voiture', 'Neuf', 'Transport', '2024-01-15');
        insert_if_not_exists('789-TN-012', 'Mercedes', 'Sprinter', 'Camion', 'Use', 'Livraison', '2024-03-20');
        insert_if_not_exists('456-TN-789', 'Peugeot', 'Partner', 'Utilitaire', 'En panne', 'Maintenance', '2024-05-10');
        insert_if_not_exists('321-TN-654', 'Iveco', 'Daily', 'Camion', 'Neuf', 'Collecte', '2024-07-25');
        insert_if_not_exists('147-TN-258', 'Citroën', 'Berlingo', 'Utilitaire', 'Use', 'Inspection', '2024-09-08');
        COMMIT;
        DBMS_OUTPUT.PUT_LINE('Données de test insérées avec succès');
    ELSE
        DBMS_OUTPUT.PUT_LINE('La table contient déjà ' || row_count || ' véhicule(s), insertion ignorée');
    END IF;
END;
/

-- 7. Vérifier les données insérées
SELECT * FROM VEHICULE ORDER BY ID_VEHI;

-- ========================================
-- REQUÊTES UTILES
-- ========================================

-- Compter le nombre de véhicules par état
SELECT ETAT, COUNT(*) AS NOMBRE
FROM VEHICULE
GROUP BY ETAT;

-- Compter le nombre de véhicules par type
SELECT TYPE, COUNT(*) AS NOMBRE
FROM VEHICULE
GROUP BY TYPE;

-- Lister les véhicules nécessitant une maintenance urgente (plus de 6 mois)
SELECT IMMAT, MARQUE, MODELE, DATE_MAINT,
       TRUNC(SYSDATE - DATE_MAINT) AS JOURS_DEPUIS_MAINT
FROM VEHICULE
WHERE DATE_MAINT < ADD_MONTHS(SYSDATE, -6)
ORDER BY DATE_MAINT;

-- ========================================
-- PROCÉDURES DE MAINTENANCE
-- ========================================

-- Supprimer toutes les données (ATTENTION!)
-- DELETE FROM VEHICULE;
-- COMMIT;

-- Réinitialiser la séquence
-- DROP SEQUENCE SEQ_VEHI;
-- CREATE SEQUENCE SEQ_VEHI START WITH 1 INCREMENT BY 1;

-- Supprimer la table
-- DROP TABLE VEHICULE;

-- ========================================
-- CONFIGURATION ODBC (Windows)
-- ========================================
/*
Pour configurer la source de données ODBC "source_projet2A":

1. Ouvrir "Administrateur de sources de données ODBC" (odbcad32.exe)
2. Onglet "DSN Système" ou "DSN Utilisateur"
3. Cliquer "Ajouter"
4. Sélectionner "Oracle in OraClient..." ou "Microsoft ODBC for Oracle"
5. Configurer:
   - Data Source Name: source_projet2A
   - TNS Service Name: localhost/XE (ou votre service Oracle)
   - User ID: wala
   
6. Tester la connexion
7. OK pour enregistrer
*/
