@echo off
chcp 65001 >nul
echo ===========================================
echo    Lancement de l'application SmartCity
echo ===========================================
echo.

cd /d "%~dp0"

set EXECUTABLE=build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe
set FULL_PATH=%CD%\%EXECUTABLE%

echo Vérification de l'exécutable...
echo Chemin: %FULL_PATH%
echo.

if exist "%EXECUTABLE%" (
    echo ✓ Exécutable trouvé!
    echo.
    echo Lancement de l'application...
    echo.
    start "" "%EXECUTABLE%"
    echo.
    echo ✓ Application lancée avec succès!
    echo.
    echo Note: Si l'application ne démarre pas, vérifiez que toutes
    echo les DLL Qt sont dans le PATH ou dans le même dossier.
    echo.
    timeout /t 3 >nul
) else (
    echo ✗ ERREUR: L'exécutable n'existe pas!
    echo.
    echo Chemin recherché: %FULL_PATH%
    echo.
    echo Solutions possibles:
    echo 1. Compilez le projet dans Qt Creator (Build ^> Build Project)
    echo 2. Vérifiez que vous utilisez le kit Qt 6.7.3 MinGW 64-bit
    echo 3. Vérifiez le chemin de build dans les paramètres du projet
    echo.
    echo Appuyez sur une touche pour fermer...
    pause >nul
)

