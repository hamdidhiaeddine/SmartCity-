@echo off
echo ============================================================
echo CREATION DE LA TABLE USERS POUR L'AUTHENTIFICATION
echo ============================================================
echo.

sqlplus hiba/esprit18@XE @CREATE_TABLE_USERS.sql

echo.
echo ============================================================
echo Script termine. Appuyez sur une touche pour fermer.
echo ============================================================
pause
