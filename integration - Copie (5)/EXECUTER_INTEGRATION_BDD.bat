@echo off
echo ============================================================
echo INTEGRATION DES TABLES SMARTCITY
echo ============================================================
echo.
echo Ce script va créer les tables:
echo   - EMPLOYES
echo   - VEHICULE
echo   - GEST_MAISON
echo   - GEST_ALERTES
echo.
echo dans le schéma HIBA (utilisateur hiba/esprit18)
echo.
pause

echo.
echo Connexion à Oracle et exécution du script...
echo.

sqlplus hiba/esprit18@XE @INTEGRATION_TABLES_BDD.sql

echo.
echo ============================================================
echo Script terminé!
echo ============================================================
echo.
echo Prochaines étapes:
echo   1. Ouvrir Qt Creator
echo   2. Ouvrir projetsmartcity.pro
echo   3. Suivre le fichier README_INTEGRATION.md
echo.
pause
