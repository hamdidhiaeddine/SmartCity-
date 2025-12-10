-- ═══════════════════════════════════════════════════════════════
-- TEST SMS - Envoi vers +216510912755
-- ═══════════════════════════════════════════════════════════════

PROMPT ═══════════════════════════════════════════════════════════════
PROMPT   CONFIGURATION DU NUMÉRO DE TEST
PROMPT ═══════════════════════════════════════════════════════════════

-- Vérifier si un résident existe déjà avec ce numéro
SELECT ID, NOM, PRENOM, TELEPHONE
FROM GEST_RESIDENT
WHERE TELEPHONE = '+216510912755';

PROMPT
PROMPT ═══════════════════════════════════════════════════════════════
PROMPT   Si aucun résident n'existe avec ce numéro, en créer un ?
PROMPT ═══════════════════════════════════════════════════════════════

-- Créer un résident de test avec votre numéro (si vous le souhaitez)
-- Décommentez les lignes suivantes :

/*
INSERT INTO GEST_RESIDENT (
    ID, 
    NOM, 
    PRENOM, 
    DATENAISS, 
    ADRESSE, 
    TELEPHONE, 
    EMAIL, 
    STATUT, 
    SITUATIONFAMILIALE
)
VALUES (
    9999,                              -- ID de test
    'TEST',                            -- Nom
    'SMS',                             -- Prénom
    TO_DATE('2000-01-01', 'YYYY-MM-DD'), -- Date de naissance
    'Tunisie',                         -- Adresse
    '+216510912755',                   -- VOTRE NUMÉRO
    'test.sms@smartcity.tn',          -- Email
    'Actif',                           -- Statut
    'Célibataire'                      -- Situation familiale
);

COMMIT;

PROMPT ✓ Résident de test créé avec le numéro +216510912755
*/

PROMPT
PROMPT ═══════════════════════════════════════════════════════════════
PROMPT   INSTRUCTIONS D'UTILISATION
PROMPT ═══════════════════════════════════════════════════════════════
PROMPT
PROMPT 3 façons d'envoyer un SMS de test à +216510912755 :
PROMPT
PROMPT 1️⃣ AU DÉMARRAGE DE L'APPLICATION :
PROMPT    - Lancez l'application Qt
PROMPT    - À la connexion réussie, cliquez OUI pour le SMS de test
PROMPT
PROMPT 2️⃣ VIA LE BOUTON "ENVOYER SMS" :
PROMPT    - Allez dans "Gestion des résidents"
PROMPT    - Cliquez sur le bouton "Envoyer SMS"
PROMPT    - Choisissez OUI pour envoyer au numéro de test
PROMPT
PROMPT 3️⃣ EN AJOUTANT UN RÉSIDENT :
PROMPT    - Créez un résident avec le numéro +216510912755
PROMPT    - L'ajout déclenchera automatiquement un SMS de confirmation
PROMPT
PROMPT ═══════════════════════════════════════════════════════════════

-- Afficher tous les résidents avec leurs numéros pour vérification
PROMPT
PROMPT Résidents existants avec numéros de téléphone :
SELECT ID, NOM, PRENOM, TELEPHONE
FROM GEST_RESIDENT
WHERE TELEPHONE IS NOT NULL
ORDER BY ID;
