# Script pour lister les drivers ODBC Oracle disponibles
Write-Host "=== DRIVERS ODBC INSTALLES (32-bit) ===" -ForegroundColor Cyan
Get-OdbcDriver -Platform "32-bit" | Where-Object { $_.Name -like "*Oracle*" } | Format-Table Name, Platform -AutoSize

Write-Host "`n=== DRIVERS ODBC INSTALLES (64-bit) ===" -ForegroundColor Cyan
Get-OdbcDriver -Platform "64-bit" | Where-Object { $_.Name -like "*Oracle*" } | Format-Table Name, Platform -AutoSize

Write-Host "`n=== SOURCES DE DONNEES UTILISATEUR (32-bit) ===" -ForegroundColor Green
Get-OdbcDsn -DsnType "User" -Platform "32-bit" | Where-Object { $_.DriverName -like "*Oracle*" } | Format-Table Name, DriverName -AutoSize

Write-Host "`n=== SOURCES DE DONNEES UTILISATEUR (64-bit) ===" -ForegroundColor Green
Get-OdbcDsn -DsnType "User" -Platform "64-bit" | Where-Object { $_.DriverName -like "*Oracle*" } | Format-Table Name, DriverName -AutoSize
