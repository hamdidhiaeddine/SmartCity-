-- Script de mise à jour : Remplacer "Usé" par "Use" dans la table VEHICULE
-- Connectez-vous en tant que wala

-- 1. Mettre à jour les données existantes
UPDATE VEHICULE SET ETAT = 'Use' WHERE ETAT = 'Usé';
COMMIT;

-- 2. Supprimer l'ancienne contrainte CHECK
BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE VEHICULE DROP CONSTRAINT SYS_C009110';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -2443 THEN -- Ignore si la contrainte n'existe pas
            RAISE;
        END IF;
END;
/

-- 3. Ajouter la nouvelle contrainte CHECK avec "Use"
ALTER TABLE VEHICULE ADD CONSTRAINT CHK_ETAT 
    CHECK (ETAT IN ('Neuf', 'Use', 'En panne'));

COMMIT;

-- 4. Vérifier les changements
SELECT ETAT, COUNT(*) AS NOMBRE FROM VEHICULE GROUP BY ETAT;
