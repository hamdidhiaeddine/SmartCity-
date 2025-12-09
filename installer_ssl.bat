@echo off
REM ========================================================
REM Script de copie automatique des DLL OpenSSL pour Qt
REM ========================================================

echo.
echo ========================================================
echo   INSTALLATION DLL OPENSSL POUR QT SMS
echo ========================================================
echo.

REM Définir les chemins
set BUILD_DIR=C:\Users\Raouf\Desktop\build-projetsmartcity-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug
set OPENSSL_32=C:\OpenSSL-Win32\bin
set OPENSSL_64=C:\OpenSSL-Win64\bin

echo Dossier de build: %BUILD_DIR%
echo.

REM Vérifier que le dossier de build existe
if not exist "%BUILD_DIR%" (
    echo [ERREUR] Le dossier de build n'existe pas !
    echo Chemin: %BUILD_DIR%
    echo.
    echo Verifiez que vous avez compile le projet au moins une fois.
    pause
    exit /b 1
)

echo.
echo Recherche des DLL OpenSSL...
echo.

REM Essayer OpenSSL 32-bit
if exist "%OPENSSL_32%\libeay32.dll" (
    echo [OK] OpenSSL 32-bit trouve dans %OPENSSL_32%
    echo.
    echo Copie de libeay32.dll...
    copy "%OPENSSL_32%\libeay32.dll" "%BUILD_DIR%\" /Y
    echo Copie de ssleay32.dll...
    copy "%OPENSSL_32%\ssleay32.dll" "%BUILD_DIR%\" /Y
    echo.
    echo [SUCCES] DLL OpenSSL 32-bit copiees !
    goto :success
)

REM Essayer OpenSSL 64-bit
if exist "%OPENSSL_64%\libeay32.dll" (
    echo [OK] OpenSSL 64-bit trouve dans %OPENSSL_64%
    echo.
    echo Copie de libeay32.dll...
    copy "%OPENSSL_64%\libeay32.dll" "%BUILD_DIR%\" /Y
    echo Copie de ssleay32.dll...
    copy "%OPENSSL_64%\ssleay32.dll" "%BUILD_DIR%\" /Y
    echo.
    echo [SUCCES] DLL OpenSSL 64-bit copiees !
    goto :success
)

REM OpenSSL non trouvé
echo [ERREUR] OpenSSL non trouve !
echo.
echo Veuillez installer OpenSSL :
echo   1. Telechargez depuis : https://slproweb.com/products/Win32OpenSSL.html
echo   2. Choisissez "Win32 OpenSSL v1.0.2u Light" (32-bit)
echo   3. Installez dans C:\OpenSSL-Win32\
echo   4. Relancez ce script
echo.
pause
exit /b 1

:success
echo.
echo ========================================================
echo   VERIFICATION
echo ========================================================
echo.

dir "%BUILD_DIR%\*.dll" | findstr /i "eay ssl"

echo.
echo ========================================================
echo   INSTALLATION TERMINEE !
echo ========================================================
echo.
echo Les DLL OpenSSL ont ete copiees dans :
echo %BUILD_DIR%
echo.
echo Vous pouvez maintenant lancer l'application :
echo   - Dans Qt Creator : Ctrl+R
echo   - Ou double-clic sur Atelier_Connexion.exe
echo.
echo Le SMS devrait fonctionner correctement !
echo.
pause
