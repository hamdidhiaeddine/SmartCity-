@echo off
chcp 65001 >nul
echo ===========================================
echo   Configuration Qt Creator - Guide
echo ===========================================
echo.
echo Ce script va vous guider pour configurer Qt Creator.
echo.
echo CHEMIN DE L'EXECUTABLE:
echo C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug\Atelier_Connexion.exe
echo.
echo REPERTOIRE DE TRAVAIL:
echo C:\ayooo
echo.
echo ===========================================
echo INSTRUCTIONS:
echo ===========================================
echo.
echo 1. Ouvrez Qt Creator
echo 2. Ouvrez le projet: projetsmartcity.pro
echo 3. Cliquez sur l'icone "Projets" (Ctrl+5)
echo 4. Cliquez sur l'onglet "RUN"
echo 5. Cliquez sur "..." a cote de "Executable"
echo 6. Naviguez vers le chemin ci-dessus
echo 7. Selectionnez Atelier_Connexion.exe
echo 8. Configurez le Working directory: C:\ayooo
echo 9. Cliquez sur "Apply"
echo 10. Cliquez sur "Run" (fleche verte)
echo.
echo ===========================================
echo.
echo Appuyez sur une touche pour ouvrir le dossier de l'executable...
pause >nul

start "" "C:\ayooo\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\debug"

echo.
echo Dossier ouvert! Vous pouvez maintenant copier le chemin
echo et le coller dans Qt Creator.
echo.
pause





