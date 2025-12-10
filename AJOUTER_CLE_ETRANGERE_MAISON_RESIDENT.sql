-- Script pour ajouter une clé étrangère entre GEST_RESIDENT et GEST_MAISON
-- Date: 2025-12-07

-- Étape 1: Ajouter la colonne ID_MAISON dans GEST_RESIDENT
ALTER TABLE GEST_RESIDENT ADD ID_MAISON NUMBER;

-- Étape 2: Mettre à jour les données existantes
-- Lier les résidents aux maisons basé sur l'adresse actuelle
UPDATE GEST_RESIDENT r
SET ID_MAISON = (
    SELECT m.ID 
    FROM GEST_MAISON m 
    WHERE m.ADRESSE = r.ADRESSE
);

-- Étape 3: Afficher le résultat de la mise à jour
SELECT ID, NOM, PRENOM, ADRESSE, ID_MAISON FROM GEST_RESIDENT ORDER BY ID;

-- Étape 4: Ajouter la contrainte de clé étrangère
ALTER TABLE GEST_RESIDENT 
ADD CONSTRAINT FK_RESIDENT_MAISON 
FOREIGN KEY (ID_MAISON) 
REFERENCES GEST_MAISON(ID)
ON DELETE SET NULL;

-- Étape 5: Créer un index pour optimiser les jointures
CREATE INDEX IDX_RESIDENT_MAISON ON GEST_RESIDENT(ID_MAISON);

-- Étape 6: Vérifier les contraintes
SELECT 
    constraint_name, 
    constraint_type, 
    r_constraint_name,
    delete_rule
FROM user_constraints 
WHERE table_name = 'GEST_RESIDENT' 
  AND constraint_type = 'R';

PROMPT;
PROMPT ========================================;
PROMPT Clé étrangère ajoutée avec succès !;
PROMPT ========================================;
PROMPT;
PROMPT Résidents liés aux maisons:;
SELECT 
    r.ID as ID_RESIDENT,
    r.NOM || ' ' || r.PRENOM as RESIDENT,
    r.ID_MAISON,
    m.ADRESSE as ADRESSE_MAISON
FROM GEST_RESIDENT r
LEFT JOIN GEST_MAISON m ON r.ID_MAISON = m.ID
ORDER BY r.ID;

COMMIT;
