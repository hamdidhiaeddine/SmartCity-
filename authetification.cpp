#include "authetification.h"
#include "ui_authentification.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRect>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCamera>
#include <QVideoWidget>
#include <QImageCapture>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QBuffer>
#include <QCryptographicHash>
#include <QTimer>

AUTHETIFICATION::AUTHETIFICATION(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Authentification");
    
    // Configurer le background avec l'image depuis les ressources
    QLabel *backgroundLabel = findChild<QLabel*>("label");
    if (backgroundLabel) {
        // Charger l'image depuis les ressources
        QPixmap bgPixmap(":/images/bg.png");
        if (!bgPixmap.isNull()) {
            // Configurer le label pour afficher l'image
            backgroundLabel->setPixmap(bgPixmap);
            backgroundLabel->setScaledContents(true);
            // Assurer que le label couvre toute la fenêtre
            backgroundLabel->setGeometry(0, 0, width(), height());
            // Mettre le background en arrière-plan (derrière tous les autres widgets)
            backgroundLabel->lower();
        } else {
            // Si l'image n'est pas trouvée, utiliser un fond de couleur
            backgroundLabel->setStyleSheet("background-color: qlineargradient(spread:repeat, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(9, 130, 230 ,10) stop:1 rgba(255, 255, 255, 255));");
        }
    }
    
    // Charger l'image utilisateur/mot de passe depuis les ressources
    QLabel *userIconLabel = findChild<QLabel*>("label_3");
    if (userIconLabel) {
        // Charger l'image phot.png depuis les ressources
        QPixmap userPixmap(":/images/phot.png");
        if (!userPixmap.isNull()) {
            userIconLabel->setPixmap(userPixmap);
            userIconLabel->setScaledContents(true);
        }
    }
    
    // Charger le logo depuis les ressources
    QLabel *logoLabel = findChild<QLabel*>("label_2");
    if (logoLabel) {
        // Le logo est déjà chargé via Lg.png dans mainwindow, mais on peut aussi charger depuis ressources si nécessaire
        // Pour l'instant, on utilise le logo depuis les ressources si disponible
        QPixmap logoPixmap(":/images/Lg.png");
        if (!logoPixmap.isNull()) {
            logoLabel->setPixmap(logoPixmap);
            logoLabel->setScaledContents(true);
        }
    }
    
    // Connecter les signaux pour la validation en temps réel
    QLineEdit *lineEdit = findChild<QLineEdit*>("lineEdit");
    QLineEdit *lineEdit_2 = findChild<QLineEdit*>("lineEdit_2");
    QPushButton *btnConnexion = findChild<QPushButton*>("add_employee_btn_3");
    
    if (lineEdit) {
        connect(lineEdit, &QLineEdit::textChanged, this, &AUTHETIFICATION::onIdentifiantChanged);
        // Supprimer le placeholder texte par défaut
        if (lineEdit->text() == "identifiant") {
            lineEdit->clear();
            lineEdit->setPlaceholderText("Identifiant");
        }
        lineEdit->setEchoMode(QLineEdit::Normal);
    }
    
    if (lineEdit_2) {
        connect(lineEdit_2, &QLineEdit::textChanged, this, &AUTHETIFICATION::onMotDePasseChanged);
        // Supprimer le placeholder texte par défaut
        if (lineEdit_2->text() == "Mot de passe ") {
            lineEdit_2->clear();
            lineEdit_2->setPlaceholderText("Mot de passe");
        }
        lineEdit_2->setEchoMode(QLineEdit::Password);
    }
    
    if (btnConnexion) {
        connect(btnConnexion, &QPushButton::clicked, this, &AUTHETIFICATION::onConnexion);
    }
    
    // Bouton Face ID
    QPushButton *btnFaceID = findChild<QPushButton*>("add_employee_btn_4");
    if (btnFaceID) {
        connect(btnFaceID, &QPushButton::clicked, this, &AUTHETIFICATION::onFaceID);
    }
    
    // Vérifier si un Face ID est déjà enregistré et lancer la reconnaissance automatiquement
    QTimer::singleShot(500, this, [this]() {
        if (hasFaceIDRegistered()) {
            recognizeFaceID();
        }
    });
}

AUTHETIFICATION::~AUTHETIFICATION()
{
    delete ui;
}

void AUTHETIFICATION::onConnexion()
{
    QLineEdit *lineEdit = findChild<QLineEdit*>("lineEdit");
    QLineEdit *lineEdit_2 = findChild<QLineEdit*>("lineEdit_2");
    
    if (!lineEdit || !lineEdit_2) {
        QMessageBox::warning(this, "Erreur", "Champs de saisie introuvables");
        return;
    }
    
    AuthInput input;
    input.identifiant = lineEdit->text();
    input.motDePasse = lineEdit_2->text();
    
    AuthValidationResult result = InputController::validateAuth(input);
    
    if (!result.ok) {
        // Afficher les erreurs
        QStringList erreurs;
        if (result.fieldErrors.contains("identifiant")) {
            erreurs << "Identifiant: " + result.fieldErrors["identifiant"];
            afficherErreur("identifiant", result.fieldErrors["identifiant"]);
        } else {
            effacerErreur("identifiant");
        }
        
        if (result.fieldErrors.contains("motDePasse")) {
            erreurs << "Mot de passe: " + result.fieldErrors["motDePasse"];
            afficherErreur("motDePasse", result.fieldErrors["motDePasse"]);
        } else {
            effacerErreur("motDePasse");
        }
        
        QMessageBox::warning(this, "Erreur de validation", 
                           "Veuillez corriger les erreurs suivantes:\n\n" + erreurs.join("\n"));
        return;
    }
    
    // Si la validation réussit, effacer les erreurs
    effacerErreur("identifiant");
    effacerErreur("motDePasse");
    
    // Vérifier les identifiants : admin / admin123
    QString identifiant = result.normalizedIdentifiant;
    QString motDePasse = input.motDePasse;
    
    const QString ADMIN_USERNAME = "admin";
    const QString ADMIN_PASSWORD = "admin123";
    
    if (identifiant != ADMIN_USERNAME || motDePasse != ADMIN_PASSWORD) {
        QMessageBox::warning(this, "Erreur d'authentification", 
                           "Identifiant ou mot de passe incorrect.\n\n"
                           "Veuillez réessayer.");
        // Effacer le champ mot de passe pour sécurité
        if (lineEdit_2) {
            lineEdit_2->clear();
        }
        return;
    }
    
    // Authentification réussie
    qDebug() << "✅ [AUTH] Connexion réussie pour l'utilisateur: admin";
    accept(); // Ferme le dialogue avec succès (retourne QDialog::Accepted)
}

void AUTHETIFICATION::onIdentifiantChanged()
{
    QLineEdit *lineEdit = findChild<QLineEdit*>("lineEdit");
    if (!lineEdit) return;
    
    QString identifiant = lineEdit->text();
    QString error;
    
    if (InputController::validateIdentifiant(identifiant, error)) {
        effacerErreur("identifiant");
    } else if (!identifiant.isEmpty()) {
        // Afficher l'erreur seulement si le champ n'est pas vide
        afficherErreur("identifiant", error);
    } else {
        effacerErreur("identifiant");
    }
}

void AUTHETIFICATION::onMotDePasseChanged()
{
    QLineEdit *lineEdit_2 = findChild<QLineEdit*>("lineEdit_2");
    if (!lineEdit_2) return;
    
    QString motDePasse = lineEdit_2->text();
    QString error;
    
    if (InputController::validateMotDePasse(motDePasse, error)) {
        effacerErreur("motDePasse");
    } else if (!motDePasse.isEmpty()) {
        // Afficher l'erreur seulement si le champ n'est pas vide
        afficherErreur("motDePasse", error);
    } else {
        effacerErreur("motDePasse");
    }
}

void AUTHETIFICATION::validerChamps()
{
    onIdentifiantChanged();
    onMotDePasseChanged();
}

void AUTHETIFICATION::afficherErreur(const QString &champ, const QString &message)
{
    // Créer ou mettre à jour un label d'erreur pour le champ
    QLabel *errorLabel = findChild<QLabel*>(champ + "_error");
    
    if (!errorLabel) {
        // Créer un nouveau label d'erreur
        errorLabel = new QLabel(this);
        errorLabel->setObjectName(champ + "_error");
        errorLabel->setStyleSheet("color: red; font-size: 10px;");
        
        // Positionner le label sous le champ correspondant
        QLineEdit *lineEdit = nullptr;
        if (champ == "identifiant") {
            lineEdit = findChild<QLineEdit*>("lineEdit");
        } else if (champ == "motDePasse") {
            lineEdit = findChild<QLineEdit*>("lineEdit_2");
        }
        
        if (lineEdit) {
            QRect geom = lineEdit->geometry();
            errorLabel->setGeometry(geom.x(), geom.y() + geom.height() + 5, geom.width(), 15);
        }
    }
    
    errorLabel->setText(message);
    errorLabel->show();
}

void AUTHETIFICATION::effacerErreur(const QString &champ)
{
    QLabel *errorLabel = findChild<QLabel*>(champ + "_error");
    if (errorLabel) {
        errorLabel->hide();
        errorLabel->clear();
    }
}

// ========== FACE ID IMPLEMENTATION ==========

void AUTHETIFICATION::onFaceID()
{
    // Si un Face ID existe déjà, faire la reconnaissance
    if (hasFaceIDRegistered()) {
        recognizeFaceID();
    } else {
        // Sinon, enregistrer un nouveau Face ID
        startFaceIDRegistration();
    }
}

void AUTHETIFICATION::startFaceIDRegistration()
{
    // Vérifier si une caméra est disponible
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune caméra détectée.\nVeuillez connecter une caméra et réessayer.");
        return;
    }
    
    // Créer un dialogue modal pour la capture
    QDialog *captureDialog = new QDialog(this);
    captureDialog->setWindowTitle("Enregistrement Face ID");
    captureDialog->setModal(true);
    captureDialog->resize(640, 480);
    
    QVBoxLayout *layout = new QVBoxLayout(captureDialog);
    
    // Widget vidéo pour afficher le flux de la caméra
    QVideoWidget *videoWidget = new QVideoWidget(captureDialog);
    layout->addWidget(videoWidget);
    
    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnCapture = new QPushButton("Capturer", captureDialog);
    QPushButton *btnCancel = new QPushButton("Annuler", captureDialog);
    buttonLayout->addWidget(btnCapture);
    buttonLayout->addWidget(btnCancel);
    layout->addLayout(buttonLayout);
    
    // Configurer la caméra
    QCamera *camera = new QCamera(cameras.first(), captureDialog);
    QImageCapture *imageCapture = new QImageCapture(captureDialog);
    QMediaCaptureSession *captureSession = new QMediaCaptureSession(captureDialog);
    
    captureSession->setCamera(camera);
    captureSession->setImageCapture(imageCapture);
    captureSession->setVideoOutput(videoWidget);
    
    // Variables pour stocker l'image capturée (utiliser QPointer pour éviter les problèmes de scope)
    QImage *capturedImagePtr = new QImage();
    bool *imageCapturedPtr = new bool(false);
    
    // Connecter le signal de capture
    connect(imageCapture, &QImageCapture::imageCaptured, captureDialog, [=](int /*id*/, const QImage &img) {
        *capturedImagePtr = img;
        *imageCapturedPtr = true;
        qDebug() << "✅ Image capturée, taille:" << img.size();
    });
    
    // Connecter le signal d'erreur
    connect(imageCapture, &QImageCapture::errorOccurred, captureDialog, [=](int /*id*/, QImageCapture::Error /*error*/, const QString &errorString) {
        qDebug() << "❌ Erreur capture:" << errorString;
        QMessageBox::warning(captureDialog, "Erreur", "Erreur lors de la capture: " + errorString);
    });
    
    // Connecter le bouton de capture
    connect(btnCapture, &QPushButton::clicked, captureDialog, [=]() {
        *imageCapturedPtr = false;
        int id = imageCapture->capture();
        if (id == -1) {
            QMessageBox::warning(captureDialog, "Erreur", "Impossible de démarrer la capture.");
            return;
        }
        // Attendre un peu pour que l'image soit capturée
        QTimer::singleShot(1000, captureDialog, [=]() {
            if (*imageCapturedPtr && !capturedImagePtr->isNull()) {
                captureDialog->accept();
            } else {
                QMessageBox::warning(captureDialog, "Erreur", "Échec de la capture. Veuillez réessayer.");
            }
        });
    });
    
    // Connecter le bouton d'annulation
    connect(btnCancel, &QPushButton::clicked, captureDialog, &QDialog::reject);
    
    // Démarrer la caméra
    camera->start();
    
    // Attendre la fermeture du dialogue
    int result = captureDialog->exec();
    
    if (result == QDialog::Accepted && *imageCapturedPtr && !capturedImagePtr->isNull()) {
        // Normaliser l'image
        QImage normalized = normalizeFaceImage(*capturedImagePtr);
        
        // Générer la signature
        QString signature = generateFaceSignature(normalized);
        
        // Sauvegarder dans la base de données
        if (saveFaceID(signature)) {
            QMessageBox::information(this, "Succès", "Face ID enregistré avec succès !");
        } else {
            QMessageBox::warning(this, "Erreur", "Échec de l'enregistrement du Face ID dans la base de données.");
        }
    }
    
    // Nettoyer
    camera->stop();
    delete capturedImagePtr;
    delete imageCapturedPtr;
    delete captureDialog;
}

QImage AUTHETIFICATION::normalizeFaceImage(const QImage &image)
{
    // Redimensionner à 200x200
    QImage resized = image.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    // Convertir en niveaux de gris
    QImage grayscale = resized.convertToFormat(QImage::Format_Grayscale8);
    
    // Appliquer un flou moyen (filtre 3x3)
    QImage blurred = grayscale;
    for (int y = 1; y < blurred.height() - 1; y++) {
        for (int x = 1; x < blurred.width() - 1; x++) {
            int sum = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    sum += qGray(grayscale.pixel(x + dx, y + dy));
                }
            }
            blurred.setPixel(x, y, qRgb(sum / 9, sum / 9, sum / 9));
        }
    }
    
    return blurred;
}

QString AUTHETIFICATION::generateFaceSignature(const QImage &image)
{
    // Diviser l'image en blocs de 20x20 pixels
    const int blockSize = 20;
    const int blocksX = image.width() / blockSize;
    const int blocksY = image.height() / blockSize;
    
    QByteArray signatureData;
    
    // Calculer la valeur moyenne de chaque bloc
    for (int by = 0; by < blocksY; by++) {
        for (int bx = 0; bx < blocksX; bx++) {
            int sum = 0;
            int count = 0;
            
            for (int y = by * blockSize; y < (by + 1) * blockSize && y < image.height(); y++) {
                for (int x = bx * blockSize; x < (bx + 1) * blockSize && x < image.width(); x++) {
                    sum += qGray(image.pixel(x, y));
                    count++;
                }
            }
            
            if (count > 0) {
                quint8 avg = sum / count;
                signatureData.append(avg);
            }
        }
    }
    
    // Générer un hash SHA256 de la signature
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(signatureData);
    QString hashString = hash.result().toHex();
    
    // Créer une signature binaire simple basée sur les valeurs moyennes
    QString binarySignature;
    for (int i = 0; i < signatureData.size(); i++) {
        quint8 val = signatureData[i];
        binarySignature += (val > 128) ? '1' : '0';
    }
    
    // Retourner hash + signature binaire
    return hashString + "_" + binarySignature;
}

bool AUTHETIFICATION::saveFaceID(const QString &signature)
{
    QSqlQuery query;
    
    // Vérifier la longueur de la signature
    if (signature.length() > 500) {
        qDebug() << "❌ Signature trop longue:" << signature.length() << "caractères (max 500)";
        QMessageBox::warning(this, "Erreur", 
                           "La signature générée est trop longue (" + QString::number(signature.length()) + " caractères).\n"
                           "Veuillez réessayer.");
        return false;
    }
    
    // Vérifier d'abord si la table existe
    query.prepare("SELECT COUNT(*) FROM USER_TABLES WHERE TABLE_NAME = 'FACE_ID'");
    bool tableExists = false;
    if (query.exec() && query.next()) {
        tableExists = (query.value(0).toInt() > 0);
    }
    
    if (!tableExists) {
        QMessageBox::warning(this, "Table manquante", 
                           "La table FACE_ID n'existe pas dans la base de données.\n\n"
                           "Veuillez exécuter le script SQL create_faceid_table.sql pour créer la table.\n\n"
                           "Le script se trouve dans le répertoire du projet.");
        return false;
    }
    
    // Supprimer l'ancien enregistrement s'il existe
    query.prepare("DELETE FROM \"SYSTEM\".\"FACE_ID\"");
    query.exec(); // Ignorer les erreurs
    
    // Insérer le nouveau Face ID
    query.prepare("INSERT INTO \"SYSTEM\".\"FACE_ID\" (\"FACE_HASH\", \"DATE_CREATION\") "
                  "VALUES (:FACE_HASH, :DATE_CREATION)");
    query.bindValue(":FACE_HASH", signature);
    query.bindValue(":DATE_CREATION", QDateTime::currentDateTime());
    
    if (!query.exec()) {
        QString errorText = query.lastError().text();
        QString nativeError = query.lastError().nativeErrorCode();
        
        qDebug() << "❌ Erreur SQL (saveFaceID):" << errorText;
        qDebug() << "   Code erreur Oracle:" << nativeError;
        qDebug() << "   Longueur signature:" << signature.length();
        
        // Gérer les différents types d'erreurs
        QString errorMessage;
        if (nativeError == "00942" || errorText.contains("table", Qt::CaseInsensitive) || errorText.contains("does not exist", Qt::CaseInsensitive)) {
            errorMessage = "La table FACE_ID n'existe pas dans la base de données.\n\n"
                          "Veuillez exécuter le script SQL create_faceid_table.sql pour créer la table.";
        } else if (nativeError == "04098" || errorText.contains("trigger", Qt::CaseInsensitive) || errorText.contains("invalid", Qt::CaseInsensitive)) {
            errorMessage = "Le trigger TRG_FACE_ID est invalide.\n\n"
                          "Veuillez exécuter le script SQL create_faceid_table.sql pour recréer le trigger.\n\n"
                          "Détails: " + errorText;
        } else if (nativeError == "01400" || errorText.contains("NOT NULL", Qt::CaseInsensitive)) {
            errorMessage = "Erreur: une colonne NOT NULL n'a pas de valeur.\n\n"
                          "Détails: " + errorText;
        } else if (nativeError == "01438" || nativeError == "12899" || errorText.contains("value larger", Qt::CaseInsensitive) || errorText.contains("too large", Qt::CaseInsensitive)) {
            // Extraire la longueur maximale depuis le message d'erreur si possible
            QString maxLength = "64";
            if (errorText.contains("maximum:")) {
                int maxPos = errorText.indexOf("maximum:") + 8;
                int endPos = errorText.indexOf(")", maxPos);
                if (endPos > maxPos) {
                    maxLength = errorText.mid(maxPos, endPos - maxPos).trimmed();
                }
            }
            
            errorMessage = "La signature est trop longue pour la colonne FACE_HASH.\n\n"
                          "Longueur actuelle: " + QString::number(signature.length()) + " caractères\n"
                          "Longueur maximale: " + maxLength + " caractères\n\n"
                          "Veuillez exécuter le script SQL fix_faceid_column_size.sql pour modifier la taille de la colonne.\n\n"
                          "Détails: " + errorText;
        } else {
            errorMessage = "Erreur lors de l'enregistrement dans la base de données.\n\n"
                          "Code erreur: " + nativeError + "\n"
                          "Message: " + errorText;
        }
        
        QMessageBox::warning(this, "Erreur d'enregistrement", errorMessage);
        return false;
    }
    
    qDebug() << "✅ Face ID sauvegardé avec succès";
    return true;
}

bool AUTHETIFICATION::hasFaceIDRegistered()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM \"SYSTEM\".\"FACE_ID\"");
    
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "🔍 Face ID enregistré:" << (count > 0 ? "Oui" : "Non");
        return count > 0;
    }
    
    qDebug() << "⚠ Erreur lors de la vérification du Face ID:" << query.lastError().text();
    return false;
}

QString AUTHETIFICATION::getSavedFaceHash()
{
    QSqlQuery query;
    query.prepare("SELECT \"FACE_HASH\" FROM \"SYSTEM\".\"FACE_ID\" WHERE ROWNUM = 1 ORDER BY \"DATE_CREATION\" DESC");
    
    if (query.exec() && query.next()) {
        QString hash = query.value(0).toString();
        qDebug() << "✅ Face ID récupéré depuis la base de données";
        return hash;
    }
    
    qDebug() << "⚠ Aucun Face ID trouvé dans la base de données";
    return QString();
}

bool AUTHETIFICATION::verifyFaceID(const QString &capturedHash)
{
    QString savedHash = getSavedFaceHash();
    
    if (savedHash.isEmpty()) {
        qDebug() << "❌ Aucun Face ID sauvegardé pour comparaison";
        return false;
    }
    
    // Calculer la distance de Hamming entre les deux signatures
    int distance = hammingDistance(capturedHash, savedHash);
    
    // Extraire les parties binaires des signatures
    QString capturedBinary = capturedHash.contains("_") ? capturedHash.split("_").last() : capturedHash;
    QString savedBinary = savedHash.contains("_") ? savedHash.split("_").last() : savedHash;
    
    int maxLength = qMax(capturedBinary.length(), savedBinary.length());
    double similarity = maxLength > 0 ? (1.0 - (double)distance / maxLength) * 100.0 : 0.0;
    
    qDebug() << "🔍 Comparaison Face ID:";
    qDebug() << "   Distance de Hamming:" << distance;
    qDebug() << "   Longueur max:" << maxLength;
    qDebug() << "   Similarité:" << QString::number(similarity, 'f', 2) << "%";
    
    // Seuil de tolérance: 75% de similarité minimum
    bool match = similarity >= 75.0;
    
    if (match) {
        // Mettre à jour la date de dernière utilisation
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE \"SYSTEM\".\"FACE_ID\" SET \"DATE_DERNIERE_UTILISATION\" = :DATE WHERE ROWNUM = 1");
        updateQuery.bindValue(":DATE", QDateTime::currentDateTime());
        updateQuery.exec();
    }
    
    return match;
}

int AUTHETIFICATION::hammingDistance(const QString &sig1, const QString &sig2)
{
    // Extraire les parties binaires des signatures
    QString binary1 = sig1.contains("_") ? sig1.split("_").last() : sig1;
    QString binary2 = sig2.contains("_") ? sig2.split("_").last() : sig2;
    
    int distance = 0;
    int minLength = qMin(binary1.length(), binary2.length());
    
    // Compter les différences
    for (int i = 0; i < minLength; i++) {
        if (binary1[i] != binary2[i]) {
            distance++;
        }
    }
    
    // Ajouter la différence de longueur comme pénalité
    distance += qAbs(binary1.length() - binary2.length());
    
    return distance;
}

void AUTHETIFICATION::recognizeFaceID()
{
    // Vérifier si une caméra est disponible
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune caméra détectée.\nVeuillez connecter une caméra et réessayer.");
        return;
    }
    
    // Créer un dialogue modal pour la reconnaissance
    QDialog *recognizeDialog = new QDialog(this);
    recognizeDialog->setWindowTitle("Reconnaissance Face ID");
    recognizeDialog->setModal(true);
    recognizeDialog->resize(640, 480);
    
    QVBoxLayout *layout = new QVBoxLayout(recognizeDialog);
    
    // Widget vidéo pour afficher le flux de la caméra
    QVideoWidget *videoWidget = new QVideoWidget(recognizeDialog);
    layout->addWidget(videoWidget);
    
    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnRecognize = new QPushButton("Reconnaître", recognizeDialog);
    QPushButton *btnCancel = new QPushButton("Annuler", recognizeDialog);
    buttonLayout->addWidget(btnRecognize);
    buttonLayout->addWidget(btnCancel);
    layout->addLayout(buttonLayout);
    
    // Configurer la caméra
    QCamera *camera = new QCamera(cameras.first(), recognizeDialog);
    QImageCapture *imageCapture = new QImageCapture(recognizeDialog);
    QMediaCaptureSession *captureSession = new QMediaCaptureSession(recognizeDialog);
    
    captureSession->setCamera(camera);
    captureSession->setImageCapture(imageCapture);
    captureSession->setVideoOutput(videoWidget);
    
    // Variables pour stocker l'image capturée
    QImage *capturedImagePtr = new QImage();
    bool *imageCapturedPtr = new bool(false);
    
    // Connecter le signal de capture
    connect(imageCapture, &QImageCapture::imageCaptured, recognizeDialog, [=](int /*id*/, const QImage &img) {
        *capturedImagePtr = img;
        *imageCapturedPtr = true;
        qDebug() << "✅ Image capturée pour reconnaissance, taille:" << img.size();
    });
    
    // Connecter le signal d'erreur
    connect(imageCapture, &QImageCapture::errorOccurred, recognizeDialog, [=](int /*id*/, QImageCapture::Error /*error*/, const QString &errorString) {
        qDebug() << "❌ Erreur capture:" << errorString;
        QMessageBox::warning(recognizeDialog, "Erreur", "Erreur lors de la capture: " + errorString);
    });
    
    // Connecter le bouton de reconnaissance
    connect(btnRecognize, &QPushButton::clicked, recognizeDialog, [=]() {
        *imageCapturedPtr = false;
        int id = imageCapture->capture();
        if (id == -1) {
            QMessageBox::warning(recognizeDialog, "Erreur", "Impossible de démarrer la capture.");
            return;
        }
        // Attendre un peu pour que l'image soit capturée
        QTimer::singleShot(1000, recognizeDialog, [=]() {
            if (*imageCapturedPtr && !capturedImagePtr->isNull()) {
                recognizeDialog->accept();
            } else {
                QMessageBox::warning(recognizeDialog, "Erreur", "Échec de la capture. Veuillez réessayer.");
            }
        });
    });
    
    // Connecter le bouton d'annulation
    connect(btnCancel, &QPushButton::clicked, recognizeDialog, &QDialog::reject);
    
    // Démarrer la caméra
    camera->start();
    
    // Attendre la fermeture du dialogue
    int result = recognizeDialog->exec();
    
    if (result == QDialog::Accepted && *imageCapturedPtr && !capturedImagePtr->isNull()) {
        // Normaliser l'image
        QImage normalized = normalizeFaceImage(*capturedImagePtr);
        
        // Générer la signature
        QString signature = generateFaceSignature(normalized);
        
        // Vérifier avec le Face ID sauvegardé
        if (verifyFaceID(signature)) {
            QMessageBox::information(this, "Succès", "Face ID reconnu avec succès !\nConnexion en cours...");
            // Accepter l'authentification
            accept();
        } else {
            QMessageBox::warning(this, "Échec", 
                                "Face ID non reconnu.\n\n"
                                "Veuillez réessayer ou utiliser la connexion normale.");
        }
    }
    
    // Nettoyer
    camera->stop();
    delete capturedImagePtr;
    delete imageCapturedPtr;
    delete recognizeDialog;
}

