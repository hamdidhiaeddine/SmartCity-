@echo off
echo ===============================================
echo CREATION DES TABLES MAINTENANCE ET RECOMMANDATION
echo ===============================================
echo.

sqlplus SMARTCITY_GESTION/hiba@localhost:1521/XE @CREATE_TABLES_JARDIN_MODULES.sql

echo.
echo ===============================================
echo Terminé ! Vérifiez les messages ci-dessus.
echo ===============================================
pause
