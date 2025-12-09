-- =====================================================
-- TEST DE VÉRIFICATION - ORACLE
-- =====================================================

-- 1. Vérifier si la table CABINET existe
SELECT table_name FROM user_tables WHERE table_name = 'CABINET';

-- 2. Compter le nombre de lignes
SELECT COUNT(*) AS total_cabinets FROM cabinet;

-- 3. Afficher toutes les lignes
SELECT * FROM cabinet;

-- 4. Vérifier les colonnes de la table
DESC cabinet;

-- 5. Vérifier la séquence
SELECT * FROM user_sequences WHERE sequence_name = 'CABINET_SEQ';

-- 6. Vérifier le trigger
SELECT trigger_name, trigger_type FROM user_triggers WHERE table_name = 'CABINET';

-- 7. Si la table est vide, insérer les données manuellement
-- DÉCOMMENTER SI NÉCESSAIRE:
/*
INSERT INTO cabinet (NOM, PRENOM, EMAIL, SPECIALITE, SALAIRE, HORAIRES, TELEPHONE)
VALUES ('Dupont', 'Marie', 'marie.dupont@clinic.fr', 'Cardiologue', 4500.00, '09:00-17:00', '01-23-45-67-89');

INSERT INTO cabinet (NOM, PRENOM, EMAIL, SPECIALITE, SALAIRE, HORAIRES, TELEPHONE)
VALUES ('Martin', 'Jean', 'jean.martin@clinic.fr', 'Neurologue', 4200.00, '08:30-16:30', '01-23-45-67-90');

-- ... et les 13 autres insertions ...

COMMIT;
*/

-- 8. Si problème de permissions, vérifier les droits
SELECT * FROM user_role_privs;
SELECT * FROM user_tab_privs WHERE table_name = 'CABINET';
