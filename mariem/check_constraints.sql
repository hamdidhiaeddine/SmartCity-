-- Vérifier les contraintes de la table GEST_MAISON
SELECT constraint_name, constraint_type, search_condition
FROM user_constraints
WHERE table_name = 'GEST_MAISON'
AND constraint_name = 'SYS_C009139';

-- Voir toutes les contraintes CHECK
SELECT constraint_name, search_condition
FROM user_constraints
WHERE table_name = 'GEST_MAISON'
AND constraint_type = 'C'
AND search_condition IS NOT NULL;

EXIT;
