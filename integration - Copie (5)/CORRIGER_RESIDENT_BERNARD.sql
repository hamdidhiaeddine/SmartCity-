-- Corriger le résident Bernard qui n'a pas de maison valide

UPDATE GEST_RESIDENT 
SET ID_MAISON = NULL 
WHERE NOM = 'Bernard';

COMMIT;

-- Vérifier le résultat
SELECT 
    r.ID as ID_RESIDENT,
    r.NOM || ' ' || r.PRENOM as RESIDENT,
    r.ID_MAISON,
    m.ADRESSE as ADRESSE_MAISON
FROM GEST_RESIDENT r
LEFT JOIN GEST_MAISON m ON r.ID_MAISON = m.ID
ORDER BY r.ID;

PROMPT;
PROMPT Résident Bernard corrigé - ID_MAISON mis à NULL;
