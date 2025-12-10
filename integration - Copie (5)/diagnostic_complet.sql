-- ═══════════════════════════════════════════════════════════════
-- DIAGNOSTIC COMPLET DE LA BASE DE DONNÉES
-- Utilisateur : SOUMAYA
-- ═══════════════════════════════════════════════════════════════

PROMPT ═══════════════════════════════════════════════════════════════
PROMPT   DIAGNOSTIC SMARTCITY - Gestion des Résidents
PROMPT ═══════════════════════════════════════════════════════════════

-- 1. Vérifier l'utilisateur connecté
PROMPT
PROMPT 1️⃣ UTILISATEUR CONNECTÉ :
PROMPT ─────────────────────────────────────────────────────────────
SELECT USER AS UTILISATEUR_ACTUEL FROM DUAL;

-- 2. Vérifier les tables existantes
PROMPT
PROMPT 2️⃣ TABLES EXISTANTES :
PROMPT ─────────────────────────────────────────────────────────────
SELECT TABLE_NAME, NUM_ROWS
FROM USER_TABLES
WHERE TABLE_NAME LIKE 'GEST_%' OR TABLE_NAME LIKE 'HIST%'
ORDER BY TABLE_NAME;

-- 3. Vérifier le contenu de GEST_RESIDENT
PROMPT
PROMPT 3️⃣ CONTENU DE LA TABLE GEST_RESIDENT :
PROMPT ─────────────────────────────────────────────────────────────
SELECT COUNT(*) AS NOMBRE_TOTAL_RESIDENTS FROM GEST_RESIDENT;

-- 4. Afficher les 5 premiers résidents
PROMPT
PROMPT 4️⃣ PREMIERS RÉSIDENTS (5 max) :
PROMPT ─────────────────────────────────────────────────────────────
SELECT ID, NOM, PRENOM, EMAIL, STATUT
FROM GEST_RESIDENT
WHERE ROWNUM <= 5
ORDER BY ID;

-- 5. Vérifier la structure de la table
PROMPT
PROMPT 5️⃣ STRUCTURE DE LA TABLE GEST_RESIDENT :
PROMPT ─────────────────────────────────────────────────────────────
DESCRIBE GEST_RESIDENT;

-- 6. Vérifier les triggers
PROMPT
PROMPT 6️⃣ TRIGGERS ACTIFS :
PROMPT ─────────────────────────────────────────────────────────────
SELECT TRIGGER_NAME, STATUS, TRIGGERING_EVENT
FROM USER_TRIGGERS
WHERE TABLE_NAME = 'GEST_RESIDENT'
ORDER BY TRIGGER_NAME;

-- 7. Vérifier l'historique
PROMPT
PROMPT 7️⃣ HISTORIQUE DES ACTIONS :
PROMPT ─────────────────────────────────────────────────────────────
SELECT ACTION, COUNT(*) AS NOMBRE
FROM HISTORIQUE_RESIDENT
GROUP BY ACTION
ORDER BY ACTION;

-- 8. Test de connexion et de lecture
PROMPT
PROMPT 8️⃣ TEST DE LECTURE SQL (exactement comme Qt) :
PROMPT ─────────────────────────────────────────────────────────────
SELECT ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE
FROM GEST_RESIDENT
ORDER BY ID;

-- 9. Vérifier les contraintes
PROMPT
PROMPT 9️⃣ CONTRAINTES SUR GEST_RESIDENT :
PROMPT ─────────────────────────────────────────────────────────────
SELECT CONSTRAINT_NAME, CONSTRAINT_TYPE, STATUS
FROM USER_CONSTRAINTS
WHERE TABLE_NAME = 'GEST_RESIDENT'
ORDER BY CONSTRAINT_TYPE, CONSTRAINT_NAME;

-- 10. Vérifier les index
PROMPT
PROMPT 🔟 INDEX SUR GEST_RESIDENT :
PROMPT ─────────────────────────────────────────────────────────────
SELECT INDEX_NAME, COLUMN_NAME, INDEX_TYPE
FROM USER_IND_COLUMNS
WHERE TABLE_NAME = 'GEST_RESIDENT'
ORDER BY INDEX_NAME, COLUMN_POSITION;

-- 11. Vérifier les privilèges
PROMPT
PROMPT 1️⃣1️⃣ PRIVILÈGES DE L'UTILISATEUR :
PROMPT ─────────────────────────────────────────────────────────────
SELECT PRIVILEGE
FROM USER_SYS_PRIVS
ORDER BY PRIVILEGE;

-- 12. Test d'insertion et de suppression
PROMPT
PROMPT 1️⃣2️⃣ TEST D'INSERTION/SUPPRESSION :
PROMPT ─────────────────────────────────────────────────────────────

-- Insérer un résident de test
INSERT INTO GEST_RESIDENT (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE)
VALUES (999, 'TEST', 'Diagnostic', TO_DATE('2000-01-01', 'YYYY-MM-DD'), '1 Rue Test', '+33111111111', 'test@diagnostic.fr', 'Actif', 'Célibataire');

SELECT '✓ Insertion test réussie' AS RESULTAT FROM DUAL WHERE EXISTS (SELECT 1 FROM GEST_RESIDENT WHERE ID = 999);

-- Vérifier l'historique (trigger doit avoir ajouté une ligne)
SELECT '✓ Trigger fonctionne - Historique créé' AS RESULTAT 
FROM DUAL 
WHERE EXISTS (SELECT 1 FROM HISTORIQUE_RESIDENT WHERE ID_RESIDENT = 999);

-- Supprimer le test
DELETE FROM GEST_RESIDENT WHERE ID = 999;

SELECT '✓ Suppression test réussie' AS RESULTAT FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM GEST_RESIDENT WHERE ID = 999);

COMMIT;

-- 13. Résumé final
PROMPT
PROMPT ═══════════════════════════════════════════════════════════════
PROMPT   RÉSUMÉ DU DIAGNOSTIC
PROMPT ═══════════════════════════════════════════════════════════════

SELECT 
    'Base de données' AS ELEMENT,
    CASE 
        WHEN COUNT(*) > 0 THEN '✓ OK - ' || COUNT(*) || ' résidents trouvés'
        ELSE '✗ VIDE - Aucun résident'
    END AS STATUT
FROM GEST_RESIDENT

UNION ALL

SELECT 
    'Triggers',
    CASE 
        WHEN COUNT(*) >= 3 THEN '✓ OK - ' || COUNT(*) || ' triggers actifs'
        ELSE '✗ PROBLÈME - Triggers manquants'
    END
FROM USER_TRIGGERS
WHERE TABLE_NAME = 'GEST_RESIDENT' AND STATUS = 'ENABLED'

UNION ALL

SELECT 
    'Historique',
    CASE 
        WHEN COUNT(*) > 0 THEN '✓ OK - ' || COUNT(*) || ' enregistrements'
        ELSE '⚠️ VIDE - Pas d\'historique'
    END
FROM HISTORIQUE_RESIDENT;

PROMPT
PROMPT ═══════════════════════════════════════════════════════════════
PROMPT   ACTIONS À EFFECTUER SI PROBLÈME :
PROMPT ═══════════════════════════════════════════════════════════════
PROMPT
PROMPT   SI "✗ VIDE - Aucun résident" :
PROMPT   → Exécutez : inserer_exemples_residents.sql
PROMPT
PROMPT   SI "✗ PROBLÈME - Triggers manquants" :
PROMPT   → Exécutez : create_database_complete_fixed.sql
PROMPT
PROMPT   SI TABLES N'EXISTENT PAS :
PROMPT   → Vérifiez que vous êtes connecté en tant que 'soumaya'
PROMPT   → Exécutez : create_database_complete_fixed.sql
PROMPT
PROMPT ═══════════════════════════════════════════════════════════════
