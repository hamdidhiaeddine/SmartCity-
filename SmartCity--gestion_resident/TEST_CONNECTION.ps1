# =====================================================
# Test de Connexion à Oracle - PowerShell
# =====================================================

# 1. Vérifier si Oracle client est installé
Write-Host "=== Vérification Oracle Client ===" -ForegroundColor Cyan
$oracleHome = [System.Environment]::GetEnvironmentVariable("ORACLE_HOME")
Write-Host "ORACLE_HOME: $oracleHome"

# 2. Tester la connexion avec SQL*Plus (si installé)
Write-Host "`n=== Test SQL*Plus ===" -ForegroundColor Cyan
try {
    & sqlplus -v
    Write-Host "✓ SQL*Plus trouvé"
} catch {
    Write-Host "✗ SQL*Plus non trouvé ou pas installé" -ForegroundColor Red
}

# 3. Vérifier les sources ODBC (Windows)
Write-Host "`n=== Sources ODBC Disponibles ===" -ForegroundColor Cyan
Get-OdbcDsn | ForEach-Object {
    Write-Host "Nom: $($_.Name)"
    Write-Host "Pilote: $($_.DriverName)"
    Write-Host "---"
}

# 4. Si vous avez une source ODBC "SmartCityBD", vérifier ses détails
Write-Host "`n=== Détails SmartCityBD ===" -ForegroundColor Cyan
$odbcDsn = Get-OdbcDsn -Name "SmartCityBD" -ErrorAction SilentlyContinue
if ($odbcDsn) {
    Write-Host "✓ Source trouvée"
    Write-Host "Pilote: $($odbcDsn.DriverName)"
    Write-Host "Description: $($odbcDsn.Description)"
} else {
    Write-Host "✗ Source 'SmartCityBD' non trouvée" -ForegroundColor Red
    Write-Host "Créez-la avec ODBC Data Source Administrator"
}

# 5. Chercher les fichiers tnsnames.ora
Write-Host "`n=== Fichiers tnsnames.ora ===" -ForegroundColor Cyan
$tnsFiles = Get-ChildItem -Path "C:\oracle" -Name "tnsnames.ora" -Recurse -ErrorAction SilentlyContinue
if ($tnsFiles) {
    Write-Host "Fichiers trouvés:"
    $tnsFiles | ForEach-Object { Write-Host "  - $_" }
} else {
    Write-Host "Aucun fichier tnsnames.ora trouvé" -ForegroundColor Yellow
}

Write-Host "`n=== Résumé ===" -ForegroundColor Green
Write-Host "1. Vérifiez que la source ODBC 'SmartCityBD' existe"
Write-Host "2. Vérifiez que Oracle Database est en cours d'exécution"
Write-Host "3. Vérifiez le fichier connection.cpp pour les identifiants corrects"
