@echo off
chcp 65001 >nul
echo ============================================================
echo    CORRECTION DU TRIGGER MUTATING TABLE
echo ============================================================
echo.
echo Ce script corrige le trigger TRG_HIST_UPDATE pour éviter
echo l'erreur "ORA-04091: table is mutating"
echo.
echo ⚠️  ATTENTION: Ce script va modifier le trigger dans la base
echo.
pause

cd /d "%~dp0"

echo.
echo Exécution du script de correction...
echo.

sqlplus hiba/esprit18@XE @CORRIGER_TRIGGER_MUTATING_TABLE.sql

echo.
echo ============================================================
echo    CORRECTION TERMINÉE
echo ============================================================
echo.
echo Le trigger TRG_HIST_UPDATE a été corrigé.
echo.
echo Prochaines étapes:
echo 1. Testez la suppression d'une maison
echo 2. Si le problème persiste, vérifiez les logs
echo.
pause

