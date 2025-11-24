-- =====================================================
-- Script pour supprimer le Face ID enregistré
-- Exécutez ce script dans SQL Developer pour réinitialiser le Face ID
-- =====================================================

-- Supprimer tous les Face ID enregistrés
DELETE FROM "SYSTEM"."FACE_ID";

-- Vérifier que la table est vide
SELECT * FROM "SYSTEM"."FACE_ID";

-- Si vous voulez aussi réinitialiser la séquence (optionnel)
-- ALTER SEQUENCE "SYSTEM"."SEQ_FACE_ID" RESTART START WITH 1;

