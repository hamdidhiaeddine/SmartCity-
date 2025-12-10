@echo off
chcp 65001 >nul
echo ============================================================
echo    DIAGNOSTIC DU MODULE VÉHICULE
echo ============================================================
echo.

cd /d "%~dp0"

echo Exécution du script de diagnostic...
echo.

sqlplus hiba/esprit18@XE @DIAGNOSTIC_VEHICULE.sql

echo.
echo ============================================================
echo    DIAGNOSTIC TERMINÉ
echo ============================================================
echo.
echo Consultez les résultats ci-dessus pour identifier le problème.
echo.
echo Si IMMATRICULATION est NUMBER, exécutez:
echo    CORRIGER_TABLE_VEHICULE.bat
echo.
pause

