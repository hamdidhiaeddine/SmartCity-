-- Script de diagnostic pour vérifier l'historique dans la base de données
-- À exécuter dans SQL Developer ou SQL*Plus avec l'utilisateur HIBA

-- 1. Vérifier si la table existe
SELECT 'Vérification de l''existence de la table...' AS ETAPE FROM DUAL;

SELECT TABLE_NAME, OWNER 
FROM ALL_TABLES 
WHERE TABLE_NAME IN ('HIST_RESIDENT', 'HISTORIQUE_RESIDENT')
  AND OWNER = 'HIBA';

-- 2. Vérifier le synonyme
SELECT 'Vérification du synonyme...' AS ETAPE FROM DUAL;

SELECT SYNONYM_NAME, TABLE_OWNER, TABLE_NAME
FROM ALL_SYNONYMS
WHERE SYNONYM_NAME = 'HIST_RESIDENT'
  AND OWNER = 'HIBA';

-- 3. Vérifier la structure de la table
SELECT 'Structure de la table...' AS ETAPE FROM DUAL;

DESC HIBA.HISTORIQUE_RESIDENT;

-- 4. Compter les enregistrements
SELECT 'Nombre d''enregistrements...' AS ETAPE FROM DUAL;

SELECT COUNT(*) AS NOMBRE_TOTAL 
FROM HIBA.HISTORIQUE_RESIDENT;

-- 5. Afficher tous les enregistrements
SELECT 'Contenu de la table...' AS ETAPE FROM DUAL;

SELECT 
    ID_HISTORIQUE,
    ID_RESIDENT,
    ACTION,
    TO_CHAR(DATE_ACTION, 'DD/MM/YYYY HH24:MI:SS') AS DATE_ACTION_FORMATEE
FROM HIBA.HISTORIQUE_RESIDENT
ORDER BY DATE_ACTION DESC;

-- 6. Vérifier les triggers
SELECT 'Vérification des triggers...' AS ETAPE FROM DUAL;

SELECT 
    TRIGGER_NAME,
    STATUS,
    TRIGGER_TYPE,
    TRIGGERING_EVENT,
    TABLE_NAME
FROM USER_TRIGGERS
WHERE TRIGGER_NAME LIKE 'TRG_AUTO_HISTORIQUE%'
   OR TABLE_NAME = 'GEST_RESIDENT'
ORDER BY TRIGGER_NAME;

-- 7. Vérifier la séquence
SELECT 'Vérification de la séquence...' AS ETAPE FROM DUAL;

SELECT SEQUENCE_NAME, LAST_NUMBER, INCREMENT_BY
FROM USER_SEQUENCES
WHERE SEQUENCE_NAME LIKE '%HISTORIQUE%'
   OR SEQUENCE_NAME LIKE '%HIST_RESIDENT%';

-- 8. Test d'insertion manuelle (optionnel - à décommenter si nécessaire)
/*
INSERT INTO HIBA.HISTORIQUE_RESIDENT (ID_RESIDENT, ACTION, DATE_ACTION)
VALUES (1, 'Ajout', SYSDATE);
COMMIT;

SELECT 'Test d''insertion effectué!' AS MESSAGE FROM DUAL;
*/

-- 9. Vérifier les permissions
SELECT 'Vérification des permissions...' AS ETAPE FROM DUAL;

SELECT PRIVILEGE, TABLE_NAME
FROM USER_TAB_PRIVS
WHERE TABLE_NAME IN ('HIST_RESIDENT', 'HISTORIQUE_RESIDENT');




