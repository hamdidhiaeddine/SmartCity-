@echo off
chcp 65001 >nul
echo ============================================================
echo    CORRECTION DE LA CONTRAINTE CHK_JARDIN_TYPE
echo ============================================================
echo.
echo Ce script supprime et recrée la contrainte CHK_JARDIN_TYPE
echo pour s'assurer qu'elle accepte exactement:
echo   - 'Publique'
echo   - 'Privee'
echo   - 'Autres'
echo.
echo ⚠️  ATTENTION: Ce script va modifier la contrainte dans la base
echo.
pause

cd /d "%~dp0"

echo.
echo Exécution de la correction...
echo.

sqlplus hiba/esprit18@XE @CORRIGER_CONTRAINTE_JARDIN.sql

echo.
echo ============================================================
echo    CORRECTION TERMINÉE
echo ============================================================
echo.
echo La contrainte CHK_JARDIN_TYPE a été corrigée.
echo.
echo Prochaines étapes:
echo 1. Testez l'ajout/modification d'un jardin
echo 2. Si le problème persiste, vérifiez les logs
echo.
pause

