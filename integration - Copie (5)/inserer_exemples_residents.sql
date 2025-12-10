-- =====================================================
-- INSERTION D'EXEMPLES DE RÉSIDENTS
-- Base de données SMARTCITY
-- Utilisateur : SOUMAYA
-- =====================================================

-- Supprimer les données de test existantes (optionnel)
-- DELETE FROM GEST_RESIDENT WHERE ID BETWEEN 1 AND 50;
-- COMMIT;

-- =====================================================
-- RÉSIDENTS EXEMPLES - DIVERSITÉ D'ÂGES ET SITUATIONS
-- =====================================================

-- Résident 1 : Jeune actif célibataire
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (1, 'Dupont', 'Jean', TO_DATE('1995-03-15', 'YYYY-MM-DD'), '12 Rue Victor Hugo', '+33612345678', 'jean.dupont@email.fr', 'Actif', 'Célibataire');

-- Résident 2 : Jeune couple
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (2, 'Martin', 'Sophie', TO_DATE('1992-07-22', 'YYYY-MM-DD'), '45 Avenue des Fleurs', '+33623456789', 'sophie.martin@gmail.com', 'Actif', 'Marié');

-- Résident 3 : Famille avec enfants
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (3, 'Bernard', 'Luc', TO_DATE('1985-11-08', 'YYYY-MM-DD'), '78 Boulevard de la Paix', '+33634567890', 'luc.bernard@yahoo.fr', 'Actif', 'Marié');

-- Résident 4 : Senior actif
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (4, 'Dubois', 'Marie', TO_DATE('1965-02-14', 'YYYY-MM-DD'), '23 Rue des Lilas', '+33645678901', 'marie.dubois@orange.fr', 'Actif', 'Veuf');

-- Résident 5 : Étudiant
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (5, 'Lambert', 'Pierre', TO_DATE('2001-09-30', 'YYYY-MM-DD'), '5 Résidence Universitaire', '+33656789012', 'pierre.lambert@student.fr', 'Actif', 'Célibataire');

-- Résident 6 : Divorcé
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (6, 'Moreau', 'Claire', TO_DATE('1988-05-17', 'YYYY-MM-DD'), '67 Allée des Roses', '+33667890123', 'claire.moreau@hotmail.com', 'Actif', 'Divorcé');

-- Résident 7 : Jeune marié
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (7, 'Petit', 'Thomas', TO_DATE('1993-12-25', 'YYYY-MM-DD'), '89 Rue de la République', '+33678901234', 'thomas.petit@live.fr', 'Actif', 'Marié');

-- Résident 8 : Senior retraité
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (8, 'Robert', 'Françoise', TO_DATE('1958-04-03', 'YYYY-MM-DD'), '34 Avenue Jean Jaurès', '+33689012345', 'francoise.robert@free.fr', 'Actif', 'Marié');

-- Résident 9 : Jeune actif
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (9, 'Richard', 'Marc', TO_DATE('1996-08-19', 'YYYY-MM-DD'), '56 Rue Gambetta', '+33690123456', 'marc.richard@gmail.com', 'Actif', 'Célibataire');

-- Résident 10 : Famille nombreuse
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (10, 'Simon', 'Julie', TO_DATE('1987-01-11', 'YYYY-MM-DD'), '12 Impasse du Moulin', '+33601234567', 'julie.simon@laposte.net', 'Actif', 'Marié');

-- Résident 11 : Jeune entrepreneur
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (11, 'Michel', 'Alexandre', TO_DATE('1990-06-28', 'YYYY-MM-DD'), '78 Boulevard Voltaire', '+33612345679', 'alex.michel@entrepreneur.fr', 'Actif', 'Célibataire');

-- Résident 12 : Couple de retraités
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (12, 'Lefebvre', 'Jacques', TO_DATE('1960-10-05', 'YYYY-MM-DD'), '90 Rue Pasteur', '+33623456780', 'jacques.lefebvre@wanadoo.fr', 'Actif', 'Marié');

-- Résident 13 : Jeune famille
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (13, 'Leroy', 'Émilie', TO_DATE('1994-03-20', 'YYYY-MM-DD'), '23 Chemin des Vignes', '+33634567891', 'emilie.leroy@yahoo.com', 'Actif', 'Marié');

-- Résident 14 : Senior veuf
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (14, 'Garnier', 'Henri', TO_DATE('1955-07-12', 'YYYY-MM-DD'), '45 Avenue de la Liberté', '+33645678902', 'henri.garnier@club-internet.fr', 'Actif', 'Veuf');

-- Résident 15 : Jeune couple
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (15, 'Roux', 'Camille', TO_DATE('1997-11-01', 'YYYY-MM-DD'), '67 Rue du Commerce', '+33656789013', 'camille.roux@outlook.fr', 'Actif', 'Marié');

-- Résident 16 : Divorcée avec enfants
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (16, 'Vincent', 'Nathalie', TO_DATE('1982-04-27', 'YYYY-MM-DD'), '89 Place de la Mairie', '+33667890124', 'nathalie.vincent@sfr.fr', 'Actif', 'Divorcé');

-- Résident 17 : Jeune actif
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (17, 'Fournier', 'Julien', TO_DATE('1998-09-15', 'YYYY-MM-DD'), '34 Rue Nationale', '+33678901235', 'julien.fournier@gmail.com', 'Actif', 'Célibataire');

-- Résident 18 : Couple senior
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (18, 'Morel', 'Monique', TO_DATE('1963-12-08', 'YYYY-MM-DD'), '56 Avenue Foch', '+33689012346', 'monique.morel@numericable.fr', 'Actif', 'Marié');

-- Résident 19 : Jeune célibataire
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (19, 'Girard', 'Lucas', TO_DATE('1999-02-03', 'YYYY-MM-DD'), '12 Résidence des Chênes', '+33690123457', 'lucas.girard@protonmail.com', 'Actif', 'Célibataire');

-- Résident 20 : Famille monoparentale
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (20, 'Andre', 'Sylvie', TO_DATE('1989-06-18', 'YYYY-MM-DD'), '78 Impasse des Acacias', '+33601234568', 'sylvie.andre@bbox.fr', 'Actif', 'Divorcé');

-- =====================================================
-- VALIDATION
-- =====================================================

-- Afficher tous les résidents insérés
SELECT ID, NOM, PRENOM, 
       TO_CHAR(DATENAISS, 'DD/MM/YYYY') AS DATE_NAISSANCE,
       FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) AS AGE,
       SITUATIONFAMILIALE
FROM GEST_RESIDENT
ORDER BY ID;

-- Statistiques
SELECT 
    COUNT(*) AS TOTAL_RESIDENTS,
    COUNT(CASE WHEN SITUATIONFAMILIALE = 'Marié' THEN 1 END) AS MARIES,
    COUNT(CASE WHEN SITUATIONFAMILIALE = 'Célibataire' THEN 1 END) AS CELIBATAIRES,
    COUNT(CASE WHEN SITUATIONFAMILIALE = 'Divorcé' THEN 1 END) AS DIVORCES,
    COUNT(CASE WHEN SITUATIONFAMILIALE = 'Veuf' THEN 1 END) AS VEUFS
FROM GEST_RESIDENT;

-- Répartition par tranches d'âge
SELECT 
    CASE 
        WHEN FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) < 18 THEN '0-17 ans'
        WHEN FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) BETWEEN 18 AND 30 THEN '18-30 ans'
        WHEN FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) BETWEEN 31 AND 50 THEN '31-50 ans'
        WHEN FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) BETWEEN 51 AND 70 THEN '51-70 ans'
        ELSE '71+ ans'
    END AS TRANCHE_AGE,
    COUNT(*) AS NOMBRE
FROM GEST_RESIDENT
GROUP BY 
    CASE 
        WHEN FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) < 18 THEN '0-17 ans'
        WHEN FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) BETWEEN 18 AND 30 THEN '18-30 ans'
        WHEN FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) BETWEEN 31 AND 50 THEN '31-50 ans'
        WHEN FLOOR(MONTHS_BETWEEN(SYSDATE, DATENAISS) / 12) BETWEEN 51 AND 70 THEN '51-70 ans'
        ELSE '71+ ans'
    END
ORDER BY 1;

-- Vérifier l'historique (devrait contenir 20 enregistrements d'ajout)
SELECT 
    ACTION,
    COUNT(*) AS NOMBRE
FROM HISTORIQUE_RESIDENT
GROUP BY ACTION
ORDER BY ACTION;

COMMIT;

SELECT '✓ 20 résidents insérés avec succès !' AS MESSAGE FROM DUAL;
SELECT '✓ Vérifiez les statistiques ci-dessus' AS MESSAGE FROM DUAL;
SELECT '✓ Vous pouvez maintenant tester votre application Qt' AS MESSAGE FROM DUAL;
