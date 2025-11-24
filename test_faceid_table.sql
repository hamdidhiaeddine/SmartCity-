-- Script de test pour vérifier que la table FACE_ID est bien créée
-- Exécutez cette requête dans SQL Developer

-- Test 1 : Vérifier que la table existe
SELECT * FROM "SYSTEM"."FACE_ID";

-- Test 2 : Vérifier la structure de la table
SELECT COLUMN_NAME, DATA_TYPE, DATA_LENGTH, NULLABLE 
FROM ALL_TAB_COLUMNS 
WHERE OWNER = 'SYSTEM' AND TABLE_NAME = 'FACE_ID'
ORDER BY COLUMN_ID;

-- Test 3 : Vérifier que la séquence existe
SELECT SEQUENCE_NAME, LAST_NUMBER 
FROM ALL_SEQUENCES 
WHERE SEQUENCE_OWNER = 'SYSTEM' AND SEQUENCE_NAME = 'SEQ_FACE_ID';

-- Test 4 : Vérifier que le trigger existe
SELECT TRIGGER_NAME, STATUS 
FROM ALL_TRIGGERS 
WHERE OWNER = 'SYSTEM' AND TRIGGER_NAME = 'TRG_FACE_ID';

