@echo off
REM ============================================================
REM Script pour créer toutes les tables de Smart City
REM ============================================================

echo ========================================
echo Creation des tables Smart City
echo ========================================
echo.

REM Demander les informations de connexion
set /p USERNAME="Nom d'utilisateur Oracle: "
set /p PASSWORD="Mot de passe: "
set /p HOSTNAME="Hôte (localhost): "
if "%HOSTNAME%"=="" set HOSTNAME=localhost
set /p PORT="Port (1521): "
if "%PORT%"=="" set PORT=1521
set /p SERVICE="Service (XE): "
if "%SERVICE%"=="" set SERVICE=XE

echo.
echo Connexion à Oracle...
echo Utilisateur: %USERNAME%
echo Hôte: %HOSTNAME%:%PORT%/%SERVICE%
echo.

REM Exécuter le script SQL
sqlplus %USERNAME%/%PASSWORD%@%HOSTNAME%:%PORT%/%SERVICE% @CREATE_TABLES_COMPLETE.sql

echo.
echo ========================================
if %ERRORLEVEL% EQU 0 (
    echo Tables creees avec succes!
) else (
    echo Erreur lors de la creation des tables.
)
echo ========================================
echo.

pause
