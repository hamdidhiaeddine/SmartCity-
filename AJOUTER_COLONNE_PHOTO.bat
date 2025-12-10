@echo off
echo ========================================
echo Ajout de la colonne PHOTO à EMPLOYES
echo ========================================
echo.

set ORACLE_HOME=C:\oraclexe\app\oracle\product\11.2.0\server
set PATH=%ORACLE_HOME%\bin;%PATH%

echo Connexion à Oracle...
sqlplus wala/esprit18@localhost:1521/XE @add_photo_column.sql

echo.
echo ========================================
echo Opération terminée!
echo ========================================
pause
