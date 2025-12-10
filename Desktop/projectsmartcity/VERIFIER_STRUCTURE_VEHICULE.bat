@echo off
chcp 65001 >nul
echo ============================================================
echo    VÉRIFICATION DE LA STRUCTURE GEST_VEHICULE
echo ============================================================
echo.
echo Ce script vérifie la structure de la table GEST_VEHICULE
echo pour diagnostiquer les problèmes de CRUD.
echo.
pause

cd /d "%~dp0"

echo.
echo Exécution de la vérification...
echo.

sqlplus hiba/esprit18@XE @VERIFIER_STRUCTURE_VEHICULE.sql

echo.
echo ============================================================
echo    VÉRIFICATION TERMINÉE
echo ============================================================
echo.
pause

