-- Ajouter la colonne PHOTO pour stocker les images faciales des employés
ALTER TABLE EMPLOYES ADD (PHOTO BLOB);

-- Vérifier la structure de la table
DESC EMPLOYES;

COMMIT;
