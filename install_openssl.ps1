# Installation OpenSSL pour Qt SMS

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  INSTALLATION OPENSSL POUR QT SMS" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$url = "https://indy.fulgan.com/SSL/openssl-1.0.2u-i386-win32.zip"
$zip = "$env:TEMP\openssl_qt.zip"
$extract = "$env:TEMP\openssl_qt"
$dest = "C:\Users\Raouf\Desktop\build-projetsmartcity-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug"

# Activer TLS 1.2
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

Write-Host "Telechargement d'OpenSSL..." -ForegroundColor Yellow

try {
    Invoke-WebRequest -Uri $url -OutFile $zip -UseBasicParsing
    Write-Host "[OK] Telechargement reussi" -ForegroundColor Green
} catch {
    Write-Host "[ERREUR] Echec du telechargement" -ForegroundColor Red
    Write-Host "Erreur: $_" -ForegroundColor Red
    Write-Host ""
    Write-Host "SOLUTION MANUELLE :" -ForegroundColor Yellow
    Write-Host "1. Allez sur: https://indy.fulgan.com/SSL/" -ForegroundColor White
    Write-Host "2. Telechargez: openssl-1.0.2u-i386-win32.zip" -ForegroundColor White
    Write-Host "3. Extrayez les fichiers libeay32.dll et ssleay32.dll" -ForegroundColor White
    Write-Host "4. Copiez-les dans: $dest" -ForegroundColor White
    Write-Host ""
    pause
    exit 1
}

Write-Host ""
Write-Host "Extraction de l'archive..." -ForegroundColor Yellow

if (Test-Path $extract) {
    Remove-Item $extract -Recurse -Force
}

Expand-Archive -Path $zip -DestinationPath $extract -Force
Write-Host "[OK] Extraction reussie" -ForegroundColor Green

Write-Host ""
Write-Host "Copie des DLL vers le dossier de build..." -ForegroundColor Yellow

# Copier libeay32.dll
if (Test-Path "$extract\libeay32.dll") {
    Copy-Item "$extract\libeay32.dll" -Destination $dest -Force
    Write-Host "[OK] libeay32.dll copie" -ForegroundColor Green
} else {
    Write-Host "[ERREUR] libeay32.dll non trouve dans l'archive" -ForegroundColor Red
}

# Copier ssleay32.dll
if (Test-Path "$extract\ssleay32.dll") {
    Copy-Item "$extract\ssleay32.dll" -Destination $dest -Force
    Write-Host "[OK] ssleay32.dll copie" -ForegroundColor Green
} else {
    Write-Host "[ERREUR] ssleay32.dll non trouve dans l'archive" -ForegroundColor Red
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  VERIFICATION" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

if (Test-Path "$dest\libeay32.dll") {
    $size = (Get-Item "$dest\libeay32.dll").Length
    Write-Host "[OK] libeay32.dll present ($size bytes)" -ForegroundColor Green
} else {
    Write-Host "[ERREUR] libeay32.dll manquant !" -ForegroundColor Red
}

if (Test-Path "$dest\ssleay32.dll") {
    $size = (Get-Item "$dest\ssleay32.dll").Length
    Write-Host "[OK] ssleay32.dll present ($size bytes)" -ForegroundColor Green
} else {
    Write-Host "[ERREUR] ssleay32.dll manquant !" -ForegroundColor Red
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  INSTALLATION TERMINEE !" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Vous pouvez maintenant relancer votre application Qt." -ForegroundColor White
Write-Host "Le SMS devrait fonctionner correctement." -ForegroundColor White
Write-Host ""

# Nettoyer les fichiers temporaires
Remove-Item $zip -Force -ErrorAction SilentlyContinue
Remove-Item $extract -Recurse -Force -ErrorAction SilentlyContinue

pause
