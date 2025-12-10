@echo off
echo ========================================
echo CREATION DE LA BASE DE DONNEES COMPLETE
echo ========================================
echo.
echo Connexion a Oracle avec l'utilisateur: wala
echo.

cd /d "%~dp0"

sqlplus wala/esprit18@localhost:1521/xe @create_database_complete_fixed.sql

echo.
echo ========================================
echo Script termine!
echo ========================================
pause
