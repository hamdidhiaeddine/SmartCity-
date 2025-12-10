-- =====================================================
-- SCRIPT DE VÉRIFICATION DE LA CONNEXION
-- Utilisateur : SOUMAYA
-- =====================================================

-- 1. Vérifier l'utilisateur connecté
SELECT USER FROM DUAL;

-- 2. Vérifier les tables existantes
SELECT TABLE_NAME 
FROM USER_TABLES 
WHERE TABLE_NAME LIKE 'GEST_%' OR TABLE_NAME LIKE 'HIST%'
ORDER BY TABLE_NAME;

-- 3. Vérifier le nombre de résidents
SELECT COUNT(*) AS NB_RESIDENTS FROM GEST_RESIDENT;

-- 4. Vérifier les privilèges de l'utilisateur
SELECT * FROM USER_SYS_PRIVS ORDER BY PRIVILEGE;

-- 5. Vérifier les triggers
SELECT TRIGGER_NAME, STATUS FROM USER_TRIGGERS ORDER BY TRIGGER_NAME;

-- 6. Test d'insertion simple
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (999, 'Test', 'Connexion', TO_DATE('2000-01-01', 'YYYY-MM-DD'), '1 Rue Test', '+33111111111', 'test@test.com', 'Actif', 'Célibataire');

-- Vérifier que l'insertion a fonctionné
SELECT * FROM GEST_RESIDENT WHERE ID = 999;

-- Nettoyer
DELETE FROM GEST_RESIDENT WHERE ID = 999;

COMMIT;

SELECT 'Test de connexion réussi !' AS MESSAGE FROM DUAL;
