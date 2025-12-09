SET LINESIZE 200
SET PAGESIZE 100
COL constraint_name FORMAT A20
COL search_condition FORMAT A60

-- Voir toutes les contraintes CHECK
SELECT constraint_name, search_condition
FROM user_constraints
WHERE table_name = 'GEST_MAISON'
AND constraint_type = 'C'
AND search_condition IS NOT NULL;

EXIT;
