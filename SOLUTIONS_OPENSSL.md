# 🔧 SOLUTIONS POUR INSTALLER OPENSSL

## ❌ Problème : OpenSSL non trouvé

Vous avez 3 solutions possibles :

---

## ✅ SOLUTION 1 : Installation manuelle (RECOMMANDÉ - 2 minutes)

### **Étape 1 : Télécharger**

Allez sur : https://github.com/kiyolee/openssl102u-win-build/releases

**Téléchargez :** `openssl-1.0.2u-win32-mingw.zip` (environ 2 MB)

### **Étape 2 : Extraire**

- Extrayez le fichier ZIP
- Cherchez les fichiers :
  - `libeay32.dll`
  - `ssleay32.dll`

### **Étape 3 : Copier**

Copiez ces 2 fichiers dans :

```
C:\Users\Raouf\Desktop\build-projetsmartcity-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug\
```

**C'est tout ! ✅**

---

## ✅ SOLUTION 2 : Script automatique (essayez d'abord)

```cmd
cd c:\Users\Raouf\Desktop\ayooo
telecharger_ssl.bat
```

Ce script télécharge et installe automatiquement les DLL.

---

## ✅ SOLUTION 3 : Installation OpenSSL complète

### **Télécharger :**

https://slproweb.com/products/Win32OpenSSL.html

**Choisissez :** "Win32 OpenSSL v1.0.2u Light" (environ 3 MB)

### **Installer :**

- Exécutez le fichier `.exe`
- Installez dans `C:\OpenSSL-Win32\`
- Cochez "Copy OpenSSL DLLs to Windows system directory" (optionnel)

### **Copier les DLL :**

```cmd
cd C:\Users\Raouf\Desktop\build-projetsmartcity-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug\

copy C:\OpenSSL-Win32\bin\libeay32.dll .
copy C:\OpenSSL-Win32\bin\ssleay32.dll .
```

---

## ✅ SOLUTION 4 : Téléchargement direct (le plus rapide)

### **Liens directs vers les DLL :**

**Option A - GitHub :**

1. https://github.com/kiyolee/openssl102u-win-build/releases/download/v1.0.2u/openssl-1.0.2u-win32-mingw.zip

**Option B - DLL-files.com (vérifiez que c'est version 1.0.2) :**

1. https://www.dll-files.com/libeay32.dll.html
2. https://www.dll-files.com/ssleay32.dll.html

⚠️ **Attention :** Assurez-vous de télécharger la version **32-bit** et **1.0.2** !

---

## 📂 Où placer les DLL ?

**Les 2 fichiers doivent être dans :**

```
C:\Users\Raouf\Desktop\build-projetsmartcity-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug\

Fichiers requis :
├── Atelier_Connexion.exe
├── libeay32.dll    ← ICI
├── ssleay32.dll    ← ICI
├── Qt5Core.dll
└── ...
```

---

## 🔍 Vérification après installation

### **1. Vérifier que les DLL sont présentes :**

```cmd
cd C:\Users\Raouf\Desktop\build-projetsmartcity-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug\
dir *.dll | findstr /i "eay ssl"
```

**Résultat attendu :**

```
libeay32.dll
ssleay32.dll
```

### **2. Relancer l'application :**

```cmd
Atelier_Connexion.exe
```

### **3. Vérifier la console Qt :**

**AVANT (avec erreur) :**

```
qt.network.ssl: QSslSocket: cannot call unresolved function SSLv23_client_method
Error creating SSL context ()
Code HTTP: 0
```

**APRÈS (sans erreur) :**

```
Code HTTP: 201
SMS envoyé avec succès! Status: queued
SID: SM1234567890abcdef
✓ SMS de test envoyé avec succès
```

---

## 🚀 Méthode EXPRESS (copier-coller)

**Ouvrez PowerShell en tant qu'administrateur et exécutez :**

```powershell
# Télécharger le ZIP
$url = "https://github.com/kiyolee/openssl102u-win-build/releases/download/v1.0.2u/openssl-1.0.2u-win32-mingw.zip"
$zip = "$env:TEMP\openssl.zip"
$dest = "C:\Users\Raouf\Desktop\build-projetsmartcity-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug"

Invoke-WebRequest -Uri $url -OutFile $zip
Expand-Archive -Path $zip -DestinationPath "$env:TEMP\openssl" -Force

# Copier les DLL
Copy-Item "$env:TEMP\openssl\bin\libeay32.dll" -Destination $dest -Force
Copy-Item "$env:TEMP\openssl\bin\ssleay32.dll" -Destination $dest -Force

Write-Host "Installation terminée !" -ForegroundColor Green
Write-Host "Les DLL sont dans : $dest" -ForegroundColor Cyan
```

---

## ❓ FAQ

### **Q: Pourquoi ai-je besoin d'OpenSSL ?**

R: Twilio utilise HTTPS (SSL/TLS) pour sécuriser les communications. Qt a besoin d'OpenSSL pour gérer ces connexions.

### **Q: Quelle version d'OpenSSL ?**

R: **Version 1.0.2** (pas 1.1.x ou 3.x). Qt 5.9.9 est compatible uniquement avec OpenSSL 1.0.2.

### **Q: 32-bit ou 64-bit ?**

R: **32-bit** car vous utilisez Qt MinGW 32-bit.

### **Q: Les DLL sont-elles sûres ?**

R: Oui, si vous téléchargez depuis :

- https://github.com/kiyolee/openssl102u-win-build (GitHub officiel)
- https://slproweb.com/products/Win32OpenSSL.html (Site officiel)

### **Q: Puis-je utiliser OpenSSL 1.1 ou 3.0 ?**

R: Non ! Qt 5.9.9 nécessite spécifiquement OpenSSL 1.0.2.

---

## 🎯 Récapitulatif

**Problème :** Qt ne trouve pas OpenSSL → Pas de HTTPS → SMS échoue  
**Solution :** Copier `libeay32.dll` + `ssleay32.dll` dans le dossier debug  
**Temps requis :** 2-5 minutes  
**Résultat :** SMS fonctionne ! 📱✅

---

**Choisissez la méthode qui vous convient et installez les DLL ! 🚀**
