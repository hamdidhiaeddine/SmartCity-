-- ============================================================
-- DONNÉES DE TEST POUR SMART CITY
-- ============================================================

-- Insérer des utilisateurs de test
INSERT INTO USERS (USERNAME, PASSWORD, ROLE) VALUES ('admin', 'admin123', 'ADMIN');
INSERT INTO USERS (USERNAME, PASSWORD, ROLE) VALUES ('soumaya', 'soumaya123', 'USER');
INSERT INTO USERS (USERNAME, PASSWORD, ROLE) VALUES ('hiba', 'hiba123', 'USER');

-- Insérer des maisons de test
INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (1, '123 Rue de la Paix', 5, 'Occupée', 'Villa', 5);

INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (2, '45 Avenue des Jardins', 2, 'Libre', 'Appartement', 3);

INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (3, '78 Boulevard Central', 4, 'Occupée', 'Maison', 4);

INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (4, '12 Rue du Marché', 1, 'En rénovation', 'Studio', 2);

INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) 
VALUES (5, '89 Place de la Liberté', 3, 'Occupée', 'Duplex', 6);

-- Insérer des résidents de test
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATE_NAISSANCE, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATION)
VALUES ('R001', 'Dupont', 'Jean', TO_DATE('1985-05-15', 'YYYY-MM-DD'), '123 Rue de la Paix', '0612345678', 'jean.dupont@email.com', 'Actif', 'Marié');

INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATE_NAISSANCE, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATION)
VALUES ('R002', 'Martin', 'Sophie', TO_DATE('1990-08-22', 'YYYY-MM-DD'), '123 Rue de la Paix', '0623456789', 'sophie.martin@email.com', 'Actif', 'Célibataire');

INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATE_NAISSANCE, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATION)
VALUES ('R003', 'Bernard', 'Pierre', TO_DATE('1978-12-10', 'YYYY-MM-DD'), '78 Boulevard Central', '0634567890', 'pierre.bernard@email.com', 'Actif', 'Marié');

INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATE_NAISSANCE, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATION)
VALUES ('R004', 'Dubois', 'Marie', TO_DATE('1995-03-18', 'YYYY-MM-DD'), '89 Place de la Liberté', '0645678901', 'marie.dubois@email.com', 'Actif', 'Divorcé');

-- Insérer des employés de test
INSERT INTO GEST_EMPLOYE (ID, NOM, PRENOM, POSTE, SALAIRE, DATE_EMBAUCHE, TELEPHONE, EMAIL)
VALUES (SEQ_EMPLOYE.NEXTVAL, 'Lemoine', 'Thomas', 'Gardien', 2500, TO_DATE('2020-01-15', 'YYYY-MM-DD'), '0656789012', 'thomas.lemoine@smartcity.com');

INSERT INTO GEST_EMPLOYE (ID, NOM, PRENOM, POSTE, SALAIRE, DATE_EMBAUCHE, TELEPHONE, EMAIL)
VALUES (SEQ_EMPLOYE.NEXTVAL, 'Rousseau', 'Claire', 'Technicien', 3000, TO_DATE('2019-06-01', 'YYYY-MM-DD'), '0667890123', 'claire.rousseau@smartcity.com');

INSERT INTO GEST_EMPLOYE (ID, NOM, PRENOM, POSTE, SALAIRE, DATE_EMBAUCHE, TELEPHONE, EMAIL)
VALUES (SEQ_EMPLOYE.NEXTVAL, 'Moreau', 'Luc', 'Manager', 4500, TO_DATE('2018-03-10', 'YYYY-MM-DD'), '0678901234', 'luc.moreau@smartcity.com');

-- Insérer des véhicules de test
INSERT INTO GEST_VEHICULE (ID, IMMATRICULATION, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINTENANCE)
VALUES (SEQ_VEHICULE.NEXTVAL, 'AB-123-CD', 'Renault', 'Master', 'Utilitaire', 'Bon', 'Maintenance', TO_DATE('2024-11-01', 'YYYY-MM-DD'));

INSERT INTO GEST_VEHICULE (ID, IMMATRICULATION, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINTENANCE)
VALUES (SEQ_VEHICULE.NEXTVAL, 'EF-456-GH', 'Peugeot', '208', 'Berline', 'Excellent', 'Surveillance', TO_DATE('2024-10-15', 'YYYY-MM-DD'));

INSERT INTO GEST_VEHICULE (ID, IMMATRICULATION, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINTENANCE)
VALUES (SEQ_VEHICULE.NEXTVAL, 'IJ-789-KL', 'Citroën', 'Berlingo', 'Utilitaire', 'Moyen', 'Nettoyage', TO_DATE('2024-09-20', 'YYYY-MM-DD'));

-- Les alertes seront créées automatiquement par le trigger pour les maisons avec sécurité < 3

COMMIT;

-- Afficher un résumé
SELECT 'Données de test insérées!' AS MESSAGE FROM DUAL;
SELECT COUNT(*) AS "Nombre de maisons" FROM GEST_MAISON;
SELECT COUNT(*) AS "Nombre de résidents" FROM GEST_RESIDENT;
SELECT COUNT(*) AS "Nombre d'employés" FROM GEST_EMPLOYE;
SELECT COUNT(*) AS "Nombre de véhicules" FROM GEST_VEHICULE;
SELECT COUNT(*) AS "Nombre d'alertes" FROM GEST_ALERTES;
