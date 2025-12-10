-- Ajouter la colonne PASSWORD à la table EMPLOYES
ALTER TABLE EMPLOYES ADD PASSWORD VARCHAR2(100);

-- Mettre un mot de passe par défaut pour les employés existants
UPDATE EMPLOYES SET PASSWORD = 'password123' WHERE PASSWORD IS NULL;

-- Vérifier la modification
SELECT COLUMN_NAME, DATA_TYPE, DATA_LENGTH 
FROM USER_TAB_COLUMNS 
WHERE TABLE_NAME = 'EMPLOYES' 
ORDER BY COLUMN_ID;

-- Afficher les employés avec leur nouveau mot de passe
SELECT ID_EMPLOYE, NOM, PRENOM, EMAIL, PASSWORD 
FROM EMPLOYES;

COMMIT;
EXIT;
