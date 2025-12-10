@echo off
chcp 65001 >nul
echo ============================================================
echo    CORRECTION DE LA TABLE GEST_VEHICULE
echo ============================================================
echo.
echo ⚠️  ATTENTION: Ce script va modifier la structure de la table
echo    Les données existantes seront sauvegardées automatiquement
echo.
echo Voulez-vous continuer ? (O/N)
set /p confirm=

if /i not "%confirm%"=="O" (
    echo Opération annulée.
    pause
    exit /b
)

echo.
echo Exécution du script de correction...
echo.

cd /d "%~dp0"

sqlplus hiba/esprit18@XE @CORRIGER_TABLE_VEHICULE.sql

echo.
echo ============================================================
echo    CORRECTION TERMINÉE
echo ============================================================
echo.
echo La table GEST_VEHICULE a été corrigée.
echo.
echo Prochaines étapes:
echo 1. Recompilez l'application Qt
echo 2. Testez l'ajout d'un véhicule
echo.
pause

