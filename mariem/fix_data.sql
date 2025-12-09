-- Vérifier les données existantes dans GEST_MAISON
SELECT ID, ASRESSE, SECURITE, STATUS FROM GEST_MAISON ORDER BY ID;

-- Vérifier les contraintes
SELECT constraint_name, search_condition
FROM user_constraints
WHERE table_name = 'GEST_MAISON'
AND constraint_type = 'C'
AND constraint_name LIKE '%009139%';

-- Corriger les données invalides
UPDATE GEST_MAISON SET SECURITE = 1 WHERE SECURITE < 1;
UPDATE GEST_MAISON SET SECURITE = 5 WHERE SECURITE > 5;
COMMIT;

-- Vérifier après correction
SELECT ID, ASRESSE, SECURITE, STATUS FROM GEST_MAISON ORDER BY ID;
