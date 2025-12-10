@echo off
chcp 65001 >nul
echo ============================================================
echo    VÉRIFICATION DE LA CONTRAINTE CHK_JARDIN_TYPE
echo ============================================================
echo.
echo Ce script vérifie la définition exacte de la contrainte
echo CHK_JARDIN_TYPE dans la base de données.
echo.
pause

cd /d "%~dp0"

echo.
echo Exécution de la vérification...
echo.

sqlplus hiba/esprit18@XE @VERIFIER_CONTRAINTE_JARDIN.sql

echo.
echo ============================================================
echo    VÉRIFICATION TERMINÉE
echo ============================================================
echo.
pause

