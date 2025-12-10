@echo off
echo ===============================================
echo RECREATION DE LA TABLE GEST_JARDIN
echo ===============================================
echo.

sqlplus SMARTCITY_GESTION/hiba@localhost:1521/XE @UPDATE_GEST_JARDIN_STRUCTURE.sql

echo.
echo ===============================================
echo Terminé ! Vérifiez les messages ci-dessus.
echo ===============================================
pause
