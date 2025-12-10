-- ═══════════════════════════════════════════════════════════════════
-- SCRIPT RAPIDE : Créer uniquement l'utilisateur HIBA
-- À exécuter en tant que SYSTEM ou SYS
-- ═══════════════════════════════════════════════════════════════════

-- Supprimer l'utilisateur s'il existe (avec toutes ses tables)
BEGIN
   EXECUTE IMMEDIATE 'DROP USER HIBA CASCADE';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

-- Créer l'utilisateur HIBA
CREATE USER HIBA IDENTIFIED BY esprit18
    DEFAULT TABLESPACE USERS
    TEMPORARY TABLESPACE TEMP
    QUOTA UNLIMITED ON USERS;

-- Donner tous les privilèges nécessaires
GRANT CONNECT TO HIBA;
GRANT RESOURCE TO HIBA;
GRANT CREATE VIEW TO HIBA;
GRANT CREATE SEQUENCE TO HIBA;
GRANT CREATE TRIGGER TO HIBA;
GRANT CREATE TABLE TO HIBA;
GRANT UNLIMITED TABLESPACE TO HIBA;

-- Vérification
SELECT 'Utilisateur HIBA créé avec succès !' AS MESSAGE FROM DUAL;
SELECT 'Mot de passe : esprit18' AS INFO FROM DUAL;
SELECT 'Vous pouvez maintenant vous connecter avec cet utilisateur.' AS INFO FROM DUAL;

-- Afficher les privilèges accordés
SELECT PRIVILEGE FROM DBA_SYS_PRIVS WHERE GRANTEE = 'HIBA';

COMMIT;
