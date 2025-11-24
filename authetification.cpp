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
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QCryptographicHash>
#include <QCamera>
#include <QVideoWidget>
#include <QImageCapture>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

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
    
    // Connecter le bouton Face ID
    QPushButton *btnFaceID = findChild<QPushButton*>("add_employee_btn_4");
    if (btnFaceID) {
        connect(btnFaceID, &QPushButton::clicked, this, &AUTHETIFICATION::onFaceID);
    }
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
    
    // Ici, vous pouvez ajouter la logique de connexion réelle
    // (vérification des identifiants dans la base de données, etc.)
    // Pour l'instant, on accepte juste le dialogue si la validation est OK
    // Le message de succès sera affiché dans main.cpp après accept()
    
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

// Vérifier si un Face ID est déjà enregistré
bool AUTHETIFICATION::hasFaceIDRegistered()
{
    // Vérifier d'abord si la table existe
    if (!tableFaceIDExists()) {
        return false;
    }
    
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM \"SYSTEM\".\"FACE_ID\"");
    if (!query.exec()) {
        qDebug() << "Erreur lors de la vérification Face ID:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

// Vérifier si la table FACE_ID existe
bool AUTHETIFICATION::tableFaceIDExists()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM USER_TABLES WHERE TABLE_NAME = 'FACE_ID'");
    if (!query.exec()) {
        // Essayer une autre méthode pour Oracle
        query.prepare("SELECT COUNT(*) FROM ALL_TABLES WHERE OWNER = 'SYSTEM' AND TABLE_NAME = 'FACE_ID'");
        if (!query.exec()) {
            return false;
        }
    }
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

// Sauvegarder un nouveau Face ID
bool AUTHETIFICATION::saveFaceID(const QString &faceHash)
{
    // Vérifier si la table existe
    if (!tableFaceIDExists()) {
        qDebug() << "La table FACE_ID n'existe pas. Veuillez exécuter le script create_faceid_table.sql";
        return false;
    }
    
    QSqlQuery query;
    // Supprimer l'ancien Face ID s'il existe (un seul Face ID par système)
    query.prepare("DELETE FROM \"SYSTEM\".\"FACE_ID\"");
    if (!query.exec()) {
        // Ignorer l'erreur si la table est vide (c'est normal)
        QString errorText = query.lastError().text();
        if (!errorText.contains("table", Qt::CaseInsensitive) && 
            !errorText.contains("does not exist", Qt::CaseInsensitive)) {
            qDebug() << "Avertissement lors de la suppression de l'ancien Face ID:" << errorText;
        }
    }
    
    // Insérer le nouveau Face ID
    query.prepare("INSERT INTO \"SYSTEM\".\"FACE_ID\" (FACE_HASH, DATE_CREATION) VALUES (:hash, SYSDATE)");
    query.bindValue(":hash", faceHash);
    
    if (!query.exec()) {
        QSqlError error = query.lastError();
        qDebug() << "Erreur lors de la sauvegarde Face ID:";
        qDebug() << "  - Texte:" << error.text();
        qDebug() << "  - Code natif:" << error.nativeErrorCode();
        qDebug() << "  - Driver:" << error.driverText();
        qDebug() << "  - Database:" << error.databaseText();
        return false;
    }
    return true;
}

// Calculer la distance de Hamming entre deux hashs (nombre de bits différents)
int AUTHETIFICATION::hammingDistance(const QString &hash1, const QString &hash2)
{
    if (hash1.length() != hash2.length()) {
        return -1; // Longueurs différentes
    }
    
    int distance = 0;
    for (int i = 0; i < hash1.length(); i++) {
        if (hash1[i] != hash2[i]) {
            distance++;
        }
    }
    return distance;
}

// Vérifier un Face ID existant avec tolérance améliorée
bool AUTHETIFICATION::verifyFaceID(const QString &faceHash)
{
    QSqlQuery query;
    query.prepare("SELECT FACE_HASH FROM \"SYSTEM\".\"FACE_ID\"");
    
    if (!query.exec()) {
        qDebug() << "Erreur lors de la vérification Face ID:" << query.lastError().text();
        return false;
    }
    
    if (query.next()) {
        QString storedHash = query.value(0).toString();
        
        // Comparaison exacte d'abord
        if (storedHash == faceHash) {
            // Mettre à jour la date de dernière utilisation
            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE \"SYSTEM\".\"FACE_ID\" SET DATE_DERNIERE_UTILISATION = SYSDATE WHERE FACE_HASH = :hash");
            updateQuery.bindValue(":hash", storedHash);
            updateQuery.exec();
            qDebug() << "Face ID reconnu (correspondance exacte)";
            return true;
        }
        
        // Si pas de correspondance exacte, calculer la distance de Hamming
        int distance = hammingDistance(storedHash, faceHash);
        qDebug() << "Distance de Hamming:" << distance << "sur" << storedHash.length() << "caractères";
        
        // Tolérance beaucoup plus élevée : 30% de différence acceptée
        // La nouvelle signature basée sur les moyennes de blocs est plus stable
        int tolerance = (storedHash.length() * 3) / 10; // 30%
        if (distance <= tolerance) {
            qDebug() << "Face ID reconnu (correspondance avec tolérance:" << tolerance << ")";
            // Mettre à jour la date de dernière utilisation
            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE \"SYSTEM\".\"FACE_ID\" SET DATE_DERNIERE_UTILISATION = SYSDATE WHERE FACE_HASH = :hash");
            updateQuery.bindValue(":hash", storedHash);
            updateQuery.exec();
            return true;
        } else {
            qDebug() << "Face ID non reconnu - Distance:" << distance << "> tolerance:" << tolerance;
            qDebug() << "Signature stockée (premiers 20):" << storedHash.left(20);
            qDebug() << "Signature capturée (premiers 20):" << faceHash.left(20);
        }
    }
    return false;
}

// Normaliser l'image faciale pour la comparaison
QImage AUTHETIFICATION::normalizeFaceImage(const QImage &image)
{
    // 1. Redimensionner à une taille fixe (réduit l'impact de la distance)
    QImage normalized = image.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // 2. Convertir en niveaux de gris (réduit l'impact de l'éclairage)
    QImage grayscale = normalized.convertToFormat(QImage::Format_Grayscale8);
    
    // 3. Redimensionner à une taille plus petite pour réduire les variations
    QImage final = grayscale.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    return final;
}

// Générer une signature simplifiée de l'image (basée sur les valeurs moyennes de régions)
QString AUTHETIFICATION::generateFaceHash(const QImage &image)
{
    QImage normalized = normalizeFaceImage(image);
    
    // Créer une signature basée sur les valeurs moyennes de blocs 10x10
    // Cela crée une signature plus tolérante aux variations
    QString signature;
    int blockSize = 10;
    int width = normalized.width();
    int height = normalized.height();
    
    for (int by = 0; by < height; by += blockSize) {
        for (int bx = 0; bx < width; bx += blockSize) {
            int sum = 0;
            int count = 0;
            
            for (int y = by; y < qMin(by + blockSize, height); y++) {
                for (int x = bx; x < qMin(bx + blockSize, width); x++) {
                    sum += qGray(normalized.pixel(x, y));
                    count++;
                }
            }
            
            if (count > 0) {
                int avg = sum / count;
                // Normaliser à une valeur entre 0-15 (hex)
                int normalizedValue = (avg * 15) / 255;
                signature += QString::number(normalizedValue, 16);
            }
        }
    }
    
    return signature;
}

void AUTHETIFICATION::onFaceID()
{
    // Vérifier si un Face ID est déjà enregistré
    bool faceIDExists = hasFaceIDRegistered();
    
    // Mode : enregistrement (false) ou reconnaissance (true)
    bool isRecognitionMode = faceIDExists;
    // Créer un dialogue pour la reconnaissance faciale avec caméra
    QDialog *faceIDDialog = new QDialog(this);
    QString dialogTitle = isRecognitionMode 
        ? "Face ID - Reconnaissance"
        : "Face ID - Enregistrement";
    faceIDDialog->setWindowTitle(dialogTitle);
    faceIDDialog->setModal(true);
    faceIDDialog->resize(800, 600);
    
    QVBoxLayout *layout = new QVBoxLayout(faceIDDialog);
    
    // Label d'instruction (change selon le mode)
    QString instructionText = isRecognitionMode 
        ? "Positionnez votre visage devant la caméra pour la reconnaissance"
        : "Enregistrez votre Face ID\nPositionnez votre visage devant la caméra";
    QLabel *instructionLabel = new QLabel(instructionText, faceIDDialog);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px; color: #333;");
    layout->addWidget(instructionLabel);
    
    // Zone d'affichage de la caméra (Qt 6 utilise QVideoWidget)
    QVideoWidget *viewfinder = new QVideoWidget(faceIDDialog);
    viewfinder->setMinimumSize(640, 480);
    viewfinder->setStyleSheet("border: 2px solid #2196F3; background-color: #000;");
    layout->addWidget(viewfinder);
    
    // Zone d'affichage de l'image capturée (cachée initialement)
    QLabel *capturedImageLabel = new QLabel(faceIDDialog);
    capturedImageLabel->setAlignment(Qt::AlignCenter);
    capturedImageLabel->setMinimumSize(640, 480);
    capturedImageLabel->setStyleSheet("border: 2px solid #4CAF50; background-color: #f5f5f5;");
    capturedImageLabel->setText("Image capturée");
    capturedImageLabel->hide();
    layout->addWidget(capturedImageLabel);
    
    // Boutons
    QPushButton *btnCapture = new QPushButton("📷 Capturer", faceIDDialog);
    btnCapture->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; padding: 12px 20px; border-radius: 5px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0b7dda; }"
        "QPushButton:disabled { background-color: #ccc; }"
    );
    
    // Bouton qui change selon le mode (Reconnaître ou Enregistrer)
    QString btnText = isRecognitionMode ? "✅ Reconnaître" : "💾 Enregistrer Face ID";
    QPushButton *btnRecognize = new QPushButton(btnText, faceIDDialog);
    btnRecognize->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 12px 20px; border-radius: 5px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:disabled { background-color: #ccc; }"
    );
    btnRecognize->setEnabled(false);
    
    QPushButton *btnRetake = new QPushButton("🔄 Reprendre", faceIDDialog);
    btnRetake->setStyleSheet(
        "QPushButton { background-color: #FF9800; color: white; padding: 12px 20px; border-radius: 5px; font-size: 14px; }"
        "QPushButton:hover { background-color: #e68900; }"
    );
    btnRetake->hide();
    
    QPushButton *btnCancel = new QPushButton("❌ Annuler", faceIDDialog);
    btnCancel->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; padding: 12px 20px; border-radius: 5px; font-size: 14px; }"
        "QPushButton:hover { background-color: #da190b; }"
    );
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnCapture);
    buttonLayout->addWidget(btnRecognize);
    buttonLayout->addWidget(btnRetake);
    buttonLayout->addWidget(btnCancel);
    layout->addLayout(buttonLayout);
    
    // Variables pour la caméra et l'image capturée
    QCamera *camera = nullptr;
    QImageCapture *imageCapture = nullptr;
    QMediaCaptureSession *captureSession = nullptr;
    QImage capturedImage;
    bool imageCaptured = false;
    
    // Initialiser la caméra
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        QMessageBox::warning(faceIDDialog, "Erreur", 
            "Aucune caméra détectée sur cet appareil.\n"
            "Veuillez connecter une caméra et réessayer.");
        delete faceIDDialog;
        return;
    }
    
    // Utiliser la première caméra disponible (généralement la caméra frontale ou principale)
    camera = new QCamera(cameras.first(), faceIDDialog);
    imageCapture = new QImageCapture(faceIDDialog);
    captureSession = new QMediaCaptureSession(faceIDDialog);
    
    captureSession->setCamera(camera);
    captureSession->setImageCapture(imageCapture);
    captureSession->setVideoOutput(viewfinder);
    
    // Démarrer la caméra
    camera->start();
    
    // Connecter le bouton de capture
    connect(btnCapture, &QPushButton::clicked, [&]() {
        if (camera && camera->isActive() && imageCapture) {
            imageCapture->capture();
        }
    });
    
    // Gérer l'image capturée (Qt 6 API)
    connect(imageCapture, &QImageCapture::imageCaptured, [&](int id, const QImage &image) {
        Q_UNUSED(id);
        capturedImage = image;
        imageCaptured = true;
        
        // Afficher l'image capturée
        QPixmap pixmap = QPixmap::fromImage(image);
        capturedImageLabel->setPixmap(pixmap.scaled(640, 480, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        
        // Cacher le viewfinder et afficher l'image capturée
        viewfinder->hide();
        capturedImageLabel->show();
        
        // Activer le bouton de reconnaissance/enregistrement
        btnRecognize->setEnabled(true);
        btnCapture->setEnabled(false);
        btnRetake->show();
        
        QString nextInstruction = isRecognitionMode 
            ? "Image capturée ! Cliquez sur 'Reconnaître' pour continuer."
            : "Image capturée ! Cliquez sur 'Enregistrer Face ID' pour sauvegarder.";
        instructionLabel->setText(nextInstruction);
    });
    
    // Gérer les erreurs de capture (Qt 6 API)
    connect(imageCapture, &QImageCapture::errorOccurred, [&](int id, QImageCapture::Error error, const QString &errorString) {
        Q_UNUSED(id);
        Q_UNUSED(error);
        QMessageBox::warning(faceIDDialog, "Erreur de capture", 
            QString("Erreur lors de la capture: %1").arg(errorString));
    });
    
    // Connecter le bouton de reconnaissance/enregistrement
    connect(btnRecognize, &QPushButton::clicked, [&, faceIDDialog, isRecognitionMode]() {
        if (!imageCaptured || capturedImage.isNull()) {
            QMessageBox::warning(faceIDDialog, "Erreur", "Aucune image capturée.");
            return;
        }
        
        // Générer le hash de l'image faciale
        QString faceHash = generateFaceHash(capturedImage);
        
        if (isRecognitionMode) {
            // MODE RECONNAISSANCE : Vérifier si le Face ID correspond
            if (verifyFaceID(faceHash)) {
                // Reconnaissance réussie
                QMessageBox::information(faceIDDialog, "Reconnaissance Faciale", 
                    "✅ Reconnaissance faciale réussie !\n\n"
                    "Votre visage a été reconnu avec succès.\n"
                    "Vous allez être connecté...");
                
                // Arrêter la caméra
                if (camera) {
                    camera->stop();
                }
                
                // Fermer les dialogues et accepter l'authentification
                faceIDDialog->accept();
                this->accept();
            } else {
                // Reconnaissance échouée
                QMessageBox::critical(faceIDDialog, "Erreur de Reconnaissance", 
                    "❌ Reconnaissance faciale échouée !\n\n"
                    "Le visage capturé ne correspond pas au Face ID enregistré.\n"
                    "Veuillez réessayer ou utiliser l'authentification par identifiant/mot de passe.");
                
                // Réinitialiser pour permettre une nouvelle tentative
                imageCaptured = false;
                capturedImage = QImage();
                btnRecognize->setEnabled(false);
                btnCapture->setEnabled(true);
                btnRetake->show();
                capturedImageLabel->hide();
                viewfinder->show();
                instructionLabel->setText("Positionnez votre visage devant la caméra pour la reconnaissance");
                
                // Redémarrer la caméra
                if (camera && !camera->isActive()) {
                    camera->start();
                }
            }
        } else {
            // MODE ENREGISTREMENT : Sauvegarder le nouveau Face ID
            if (saveFaceID(faceHash)) {
                QMessageBox::information(faceIDDialog, "Face ID Enregistré", 
                    "✅ Face ID enregistré avec succès !\n\n"
                    "Votre visage a été sauvegardé.\n"
                    "Vous pouvez maintenant utiliser Face ID pour vous connecter.");
                
                // Arrêter la caméra
                if (camera) {
                    camera->stop();
                }
                
                // Fermer les dialogues et accepter l'authentification
                faceIDDialog->accept();
                this->accept();
            } else {
                // Vérifier si c'est un problème de table
                QString errorDetails;
                if (!tableFaceIDExists()) {
                    errorDetails = "La table FACE_ID n'existe pas dans la base de données.\n\n"
                                  "Veuillez exécuter le script SQL suivant dans votre base de données Oracle :\n"
                                  "create_faceid_table.sql\n\n"
                                  "Ce script crée la table nécessaire pour stocker les Face ID.";
                } else {
                    QSqlQuery testQuery;
                    testQuery.prepare("SELECT 1 FROM \"SYSTEM\".\"FACE_ID\"");
                    if (!testQuery.exec()) {
                        QSqlError error = testQuery.lastError();
                        errorDetails = QString("Erreur de base de données :\n%1\n\nCode : %2")
                                      .arg(error.text())
                                      .arg(error.nativeErrorCode());
                    } else {
                        errorDetails = "Erreur lors de l'enregistrement du Face ID.\n"
                                      "Veuillez vérifier votre connexion à la base de données.";
                    }
                }
                
                QMessageBox::critical(faceIDDialog, "Erreur d'enregistrement", 
                    QString("❌ Erreur lors de l'enregistrement du Face ID.\n\n%1").arg(errorDetails));
            }
        }
    });
    
    // Connecter le bouton de reprise
    connect(btnRetake, &QPushButton::clicked, [&, isRecognitionMode]() {
        imageCaptured = false;
        capturedImage = QImage();
        btnRecognize->setEnabled(false);
        btnCapture->setEnabled(true);
        btnRetake->hide();
        capturedImageLabel->hide();
        viewfinder->show();
        QString resetInstruction = isRecognitionMode 
            ? "Positionnez votre visage devant la caméra pour la reconnaissance"
            : "Enregistrez votre Face ID\nPositionnez votre visage devant la caméra";
        instructionLabel->setText(resetInstruction);
        
        // Redémarrer la caméra si nécessaire
        if (camera && !camera->isActive()) {
            camera->start();
        }
    });
    
    // Connecter le bouton d'annulation
    connect(btnCancel, &QPushButton::clicked, [&, faceIDDialog]() {
        if (camera) {
            camera->stop();
        }
        faceIDDialog->reject();
    });
    
    // Nettoyer lors de la fermeture
    connect(faceIDDialog, &QDialog::finished, [&]() {
        if (camera) {
            camera->stop();
        }
    });
    
    // Afficher le dialogue
    faceIDDialog->exec();
    delete faceIDDialog;
}
