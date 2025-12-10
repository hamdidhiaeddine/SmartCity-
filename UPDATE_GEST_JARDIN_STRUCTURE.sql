-- =====================================================
-- MISE À JOUR DE LA TABLE GEST_JARDIN
-- Ajout de la colonne TEMPERATURE_MOYENNE_SOL
-- =====================================================

-- Vérifier si la table existe et la supprimer si nécessaire
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE GEST_JARDIN CASCADE CONSTRAINTS';
   DBMS_OUTPUT.PUT_LINE('Table GEST_JARDIN supprimée');
EXCEPTION
   WHEN OTHERS THEN
      IF SQLCODE != -942 THEN
         RAISE;
      END IF;
      DBMS_OUTPUT.PUT_LINE('Table GEST_JARDIN n''existe pas encore');
END;
/

-- Créer la table GEST_JARDIN avec la nouvelle structure
CREATE TABLE GEST_JARDIN (
    ID_JARDIN NUMBER PRIMARY KEY,
    EMPLACEMENT VARCHAR2(200 BYTE) NOT NULL,
    SUPERFICIE NUMBER(10,2) NOT NULL,
    TYPE_SOL VARCHAR2(100 BYTE) NOT NULL,
    TEMPERATURE_MOYENNE_SOL NUMBER(5,2) DEFAULT 0,
    TYPE_CHOIX VARCHAR2(50 BYTE) NOT NULL,
    CONSTRAINT CHK_JARDIN_SUPERFICIE CHECK (SUPERFICIE > 0),
    CONSTRAINT CHK_JARDIN_TYPE CHECK (TYPE_CHOIX IN ('Publique', 'Privee', 'Autres'))
);

COMMIT;

-- Afficher la structure de la table
SELECT column_name, data_type, data_length, nullable
FROM user_tab_columns
WHERE table_name = 'GEST_JARDIN'
ORDER BY column_id;

-- Insérer quelques données de test
INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
VALUES (1, 'Jardin Central - Avenue Mohammed V', 150.50, 'Argileux', 22.5, 'Publique');

INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
VALUES (2, 'Jardin Résidentiel - Quartier Agdal', 85.00, 'Sableux', 24.0, 'Privee');

INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
VALUES (3, 'Espace Vert - Zone Industrielle', 320.75, 'Limoneux', 21.8, 'Publique');

INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
VALUES (4, 'Jardin Communautaire - Hay Riad', 200.00, 'Humifère', 23.2, 'Autres');

INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX)
VALUES (5, 'Parc Municipal - Centre Ville', 500.00, 'Calcaire', 22.0, 'Publique');

COMMIT;

-- Afficher les données insérées
SELECT * FROM GEST_JARDIN ORDER BY ID_JARDIN;

DBMS_OUTPUT.PUT_LINE('Table GEST_JARDIN créée avec succès avec ' || SQL%ROWCOUNT || ' enregistrements de test');
