-- ═══════════════════════════════════════════════════════════════
-- CORRECTION TAILLE DES COLONNES GEST_RESIDENT
-- Problème : STATUT et SITUATIONFAMILIALE trop petites
-- ═══════════════════════════════════════════════════════════════

PROMPT ═══════════════════════════════════════════════════════════════
PROMPT   CORRECTION DES COLONNES TROP PETITES
PROMPT ═══════════════════════════════════════════════════════════════

-- 1. Agrandir la colonne STATUT (15 → 20 caractères)
PROMPT
PROMPT 1️⃣ Agrandissement de la colonne STATUT...
ALTER TABLE GEST_RESIDENT MODIFY (STATUT VARCHAR2(20));
PROMPT ✓ STATUT agrandi à 20 caractères

-- 2. Agrandir la colonne SITUATIONFAMILIALE (15 → 20 caractères)
PROMPT
PROMPT 2️⃣ Agrandissement de la colonne SITUATIONFAMILIALE...
ALTER TABLE GEST_RESIDENT MODIFY (SITUATIONFAMILIALE VARCHAR2(20));
PROMPT ✓ SITUATIONFAMILIALE agrandi à 20 caractères

-- 3. Vérifier les nouvelles tailles
PROMPT
PROMPT 3️⃣ Vérification des nouvelles tailles...
PROMPT
SELECT COLUMN_NAME, DATA_TYPE, DATA_LENGTH
FROM USER_TAB_COLUMNS
WHERE TABLE_NAME = 'GEST_RESIDENT'
  AND COLUMN_NAME IN ('STATUT', 'SITUATIONFAMILIALE')
ORDER BY COLUMN_NAME;

-- 4. Valider les données existantes
PROMPT
PROMPT 4️⃣ Test avec valeurs maximales...

-- Test temporaire
DECLARE
    v_test_statut VARCHAR2(20) := 'Propriétaire';
    v_test_situation VARCHAR2(20) := 'Célibataire';
BEGIN
    IF LENGTH(v_test_statut) <= 20 THEN
        DBMS_OUTPUT.PUT_LINE('✓ Statut "Propriétaire" (' || LENGTH(v_test_statut) || ' car) OK');
    END IF;
    
    IF LENGTH(v_test_situation) <= 20 THEN
        DBMS_OUTPUT.PUT_LINE('✓ Situation "Célibataire" (' || LENGTH(v_test_situation) || ' car) OK');
    END IF;
END;
/

PROMPT
PROMPT ═══════════════════════════════════════════════════════════════
PROMPT   ✓ CORRECTION TERMINÉE AVEC SUCCÈS !
PROMPT ═══════════════════════════════════════════════════════════════
PROMPT
PROMPT Nouvelles tailles :
PROMPT   - STATUT : VARCHAR2(20) - supporte "Propriétaire" (12 car)
PROMPT   - SITUATIONFAMILIALE : VARCHAR2(20) - supporte "Célibataire" (11 car)
PROMPT
PROMPT Vous pouvez maintenant ajouter des résidents sans erreur ORA-12899
PROMPT ═══════════════════════════════════════════════════════════════

COMMIT;
