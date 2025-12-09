@echo off
REM ========================================================
REM Téléchargement automatique des DLL OpenSSL pour Qt
REM ========================================================

echo.
echo ========================================================
echo   TELECHARGEMENT DLL OPENSSL POUR QT SMS
echo ========================================================
echo.

set BUILD_DIR=C:\Users\Raouf\Desktop\build-projetsmartcity-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug

echo Dossier de build: %BUILD_DIR%
echo.

REM Vérifier que le dossier de build existe
if not exist "%BUILD_DIR%" (
    echo [ERREUR] Le dossier de build n'existe pas !
    pause
    exit /b 1
)

echo.
echo ========================================================
echo   ETAPE 1 : Telechargement des DLL
echo ========================================================
echo.

REM Créer un dossier temporaire
if not exist "%TEMP%\openssl_qt" mkdir "%TEMP%\openssl_qt"
cd /d "%TEMP%\openssl_qt"

echo Telechargement de libeay32.dll...
powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/kiyolee/openssl102u-win-build/releases/download/v1.0.2u/openssl-1.0.2u-win32-mingw.zip' -OutFile 'openssl.zip' -UseBasicParsing}"

if not exist "openssl.zip" (
    echo.
    echo [ERREUR] Echec du telechargement !
    echo.
    echo SOLUTION MANUELLE :
    echo 1. Allez sur : https://github.com/kiyolee/openssl102u-win-build/releases
    echo 2. Telechargez : openssl-1.0.2u-win32-mingw.zip
    echo 3. Extraire les fichiers libeay32.dll et ssleay32.dll
    echo 4. Copiez-les dans : %BUILD_DIR%
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================================
echo   ETAPE 2 : Extraction des DLL
echo ========================================================
echo.

echo Extraction de l'archive...
powershell -Command "Expand-Archive -Path 'openssl.zip' -DestinationPath '.' -Force"

echo.
echo ========================================================
echo   ETAPE 3 : Copie vers le dossier de build
echo ========================================================
echo.

REM Chercher les DLL dans le dossier extrait
for /r %%f in (libeay32.dll) do (
    if exist "%%f" (
        echo Copie de libeay32.dll...
        copy "%%f" "%BUILD_DIR%\" /Y
    )
)

for /r %%f in (ssleay32.dll) do (
    if exist "%%f" (
        echo Copie de ssleay32.dll...
        copy "%%f" "%BUILD_DIR%\" /Y
    )
)

echo.
echo ========================================================
echo   VERIFICATION
echo ========================================================
echo.

if exist "%BUILD_DIR%\libeay32.dll" (
    echo [OK] libeay32.dll present
) else (
    echo [ERREUR] libeay32.dll manquant !
)

if exist "%BUILD_DIR%\ssleay32.dll" (
    echo [OK] ssleay32.dll present
) else (
    echo [ERREUR] ssleay32.dll manquant !
)

echo.
echo ========================================================
echo   INSTALLATION TERMINEE !
echo ========================================================
echo.
echo Les DLL OpenSSL ont ete copiees dans :
echo %BUILD_DIR%
echo.
echo Vous pouvez maintenant lancer l'application !
echo.
pause
