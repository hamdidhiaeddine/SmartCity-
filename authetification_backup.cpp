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
    QDialog *faceIDDialog = new QDialog(this);
    faceIDDialog->setWindowTitle("Face ID - Enregistrement");
    faceIDDialog->setModal(true);
    faceIDDialog->resize(800, 600);
    
    QVBoxLayout *layout = new QVBoxLayout(faceIDDialog);
    
    // Label d'instruction
    QString instructionText = "Enregistrement Face ID\n\nPositionnez votre visage devant la caméra";
    if (hasFaceIDRegistered()) {
        instructionText += "\n\n⚠️ Un Face ID existe déjà. Il sera remplacé par le nouveau.";
    }
    QLabel *instructionLabel = new QLabel(instructionText, faceIDDialog);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px; color: #333;");
    layout->addWidget(instructionLabel);
    
    // Zone d'affichage de la caméra
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
    
    QPushButton *btnSave = new QPushButton("💾 Enregistrer Face ID", faceIDDialog);
    btnSave->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 12px 20px; border-radius: 5px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:disabled { background-color: #ccc; }"
    );
    btnSave->setEnabled(false);
    
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
    buttonLayout->addWidget(btnSave);
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
    qDebug() << "🔍 [FACE ID] Nombre de caméras détectées:" << cameras.size();
    
    if (cameras.isEmpty()) {
        qDebug() << "❌ [FACE ID] Aucune caméra détectée";
        QMessageBox::warning(faceIDDialog, "Erreur", 
            "Aucune caméra détectée sur cet appareil.\n"
            "Veuillez connecter une caméra et réessayer.");
        delete faceIDDialog;
        return;
    }
    
    // Afficher les informations sur les caméras détectées
    for (int i = 0; i < cameras.size(); ++i) {
        qDebug() << "📷 [FACE ID] Caméra" << i << ":" << cameras[i].description() 
                 << "| Position:" << (cameras[i].position() == QCameraDevice::FrontFace ? "Frontale" : "Arrière");
    }
    
    // Utiliser la première caméra disponible
    camera = new QCamera(cameras.first(), faceIDDialog);
    qDebug() << "✅ [FACE ID] Caméra sélectionnée:" << cameras.first().description();
    imageCapture = new QImageCapture(faceIDDialog);
    captureSession = new QMediaCaptureSession(faceIDDialog);
    
    captureSession->setCamera(camera);
    captureSession->setImageCapture(imageCapture);
    captureSession->setVideoOutput(viewfinder);
    
    // Démarrer la caméra
    qDebug() << "🎥 [FACE ID] Démarrage de la caméra...";
    camera->start();
    
    // Vérifier l'état de la caméra
    connect(camera, &QCamera::activeChanged, [](bool active) {
        qDebug() << (active ? "✅ [FACE ID] Caméra activée" : "❌ [FACE ID] Caméra désactivée");
    });
    
    connect(camera, &QCamera::errorOccurred, [](QCamera::Error error, const QString &errorString) {
        qDebug() << "❌ [FACE ID] Erreur caméra:" << error << "|" << errorString;
    });
    
    // Connecter le bouton de capture
    connect(btnCapture, &QPushButton::clicked, [&]() {
        if (camera && camera->isActive() && imageCapture) {
            imageCapture->capture();
        }
    });
    
    // Gérer l'image capturée
    connect(imageCapture, &QImageCapture::imageCaptured, [&](int id, const QImage &image) {
        Q_UNUSED(id);
        qDebug() << "📸 [FACE ID] Image capturée - Taille:" << image.width() << "x" << image.height();
        capturedImage = image;
        imageCaptured = true;
        
        // Afficher l'image capturée
        QPixmap pixmap = QPixmap::fromImage(image);
        capturedImageLabel->setPixmap(pixmap.scaled(640, 480, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        
        // Cacher le viewfinder et afficher l'image capturée
        viewfinder->hide();
        capturedImageLabel->show();
        
        // Activer le bouton de sauvegarde
        btnSave->setEnabled(true);
        btnCapture->setEnabled(false);
        btnRetake->show();
        
        instructionLabel->setText("Image capturée ! Cliquez sur 'Enregistrer Face ID' pour sauvegarder.");
    });
    
    // Gérer les erreurs de capture
    connect(imageCapture, &QImageCapture::errorOccurred, [&](int id, QImageCapture::Error error, const QString &errorString) {
        Q_UNUSED(id);
        Q_UNUSED(error);
        QMessageBox::warning(faceIDDialog, "Erreur de capture", 
            QString("Erreur lors de la capture: %1").arg(errorString));
    });
    
    // Connecter le bouton de sauvegarde
    connect(btnSave, &QPushButton::clicked, [&, faceIDDialog]() {
        if (!imageCaptured || capturedImage.isNull()) {
            QMessageBox::warning(faceIDDialog, "Erreur", "Aucune image capturée.");
            return;
        }
        
        // Normaliser l'image et générer une signature
        QImage normalizedImage = normalizeFaceImage(capturedImage);
        QString faceHash = generateFaceSignature(normalizedImage);
        
        qDebug() << "🔐 [FACE ID] Signature générée - Longueur:" << faceHash.length() << "caractères";
        
        // Sauvegarder dans la base de données
        QSqlQuery query;
        
        // Supprimer l'ancien Face ID s'il existe
        query.prepare("DELETE FROM \"SYSTEM\".\"FACE_ID\"");
        query.exec(); // Ignorer les erreurs si la table n'existe pas
        
        // Essayer d'insérer avec le trigger (sans spécifier l'ID)
        query.prepare("INSERT INTO \"SYSTEM\".\"FACE_ID\" (FACE_HASH, DATE_CREATION) VALUES (:hash, SYSDATE)");
        query.bindValue(":hash", faceHash);
        
        if (!query.exec()) {
            QSqlError error = query.lastError();
            QString errorText = error.text();
            
            // Si le trigger est invalide, générer l'ID manuellement
            if (errorText.contains("trigger", Qt::CaseInsensitive) && 
                (errorText.contains("invalid", Qt::CaseInsensitive) || 
                 error.nativeErrorCode() == "4098")) {
                
                qDebug() << "⚠️ [FACE ID] Trigger invalide, génération manuelle de l'ID";
                
                // Obtenir le prochain ID manuellement
                query.prepare("SELECT NVL(MAX(ID), 0) + 1 FROM \"SYSTEM\".\"FACE_ID\"");
                if (query.exec() && query.next()) {
                    int nextId = query.value(0).toInt();
                    query.prepare("INSERT INTO \"SYSTEM\".\"FACE_ID\" (ID, FACE_HASH, DATE_CREATION) VALUES (:id, :hash, SYSDATE)");
                    query.bindValue(":id", nextId);
                    query.bindValue(":hash", faceHash);
                    
                    if (!query.exec()) {
                        QSqlError error2 = query.lastError();
                        QMessageBox::critical(faceIDDialog, "Erreur d'enregistrement", 
                            QString("❌ Erreur lors de l'enregistrement du Face ID.\n\n"
                                   "Message : %1\n"
                                   "Code : %2\n\n"
                                   "Vérifiez que la table FACE_ID existe et que vous avez les permissions nécessaires.")
                                .arg(error2.text())
                                .arg(error2.nativeErrorCode()));
                        return;
                    }
                } else {
                    QMessageBox::critical(faceIDDialog, "Erreur d'enregistrement", 
                        QString("❌ Impossible de générer l'ID pour le Face ID.\n\n"
                               "Message : %1\n"
                               "Code : %2\n\n"
                               "Vérifiez que la table FACE_ID existe dans la base de données.")
                            .arg(error.text())
                            .arg(error.nativeErrorCode()));
                    return;
                }
            } else {
                // Autre type d'erreur
                QMessageBox::critical(faceIDDialog, "Erreur d'enregistrement", 
                    QString("❌ Erreur lors de l'enregistrement du Face ID.\n\n"
                           "Message : %1\n"
                           "Code : %2\n\n"
                           "Vérifiez que la table FACE_ID existe dans la base de données.\n"
                           "Si le trigger est invalide, exécutez le script fix_faceid_trigger.sql")
                        .arg(error.text())
                        .arg(error.nativeErrorCode()));
                return;
            }
        }
        
        qDebug() << "✅ [FACE ID] Face ID enregistré avec succès dans la base de données";
        
        // Arrêter la caméra
        if (camera) {
            camera->stop();
        }
        
        QMessageBox::information(faceIDDialog, "Face ID Enregistré", 
            "✅ Face ID enregistré avec succès !\n\n"
            "Votre visage a été sauvegardé.\n"
            "Vous pouvez maintenant utiliser Face ID pour vous connecter.\n\n"
            "Fermez cette fenêtre et rouvrez l'application pour utiliser Face ID.");
        
        // Fermer uniquement le dialogue Face ID, pas l'authentification
        faceIDDialog->accept();
    });
    
    // Connecter le bouton de reprise
    connect(btnRetake, &QPushButton::clicked, [&]() {
        imageCaptured = false;
        capturedImage = QImage();
        btnSave->setEnabled(false);
        btnCapture->setEnabled(true);
        btnRetake->hide();
        capturedImageLabel->hide();
        viewfinder->show();
        instructionLabel->setText("Positionnez votre visage devant la caméra");
        
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

bool AUTHETIFICATION::hasFaceIDRegistered()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM \"SYSTEM\".\"FACE_ID\"");
    
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "🔍 [FACE ID] Nombre de Face ID enregistrés:" << count;
        return count > 0;
    }
    
    qDebug() << "⚠️ [FACE ID] Erreur lors de la vérification:" << query.lastError().text();
    return false;
}

QString AUTHETIFICATION::getSavedFaceHash()
{
    QSqlQuery query;
    query.prepare("SELECT FACE_HASH FROM \"SYSTEM\".\"FACE_ID\" WHERE ROWNUM = 1 ORDER BY ID DESC");
    
    if (query.exec() && query.next()) {
        QString hash = query.value(0).toString();
        qDebug() << "✅ [FACE ID] Hash récupéré depuis la base de données";
        return hash;
    }
    
    qDebug() << "❌ [FACE ID] Impossible de récupérer le hash:" << query.lastError().text();
    return QString();
}

bool AUTHETIFICATION::verifyFaceID(const QString &capturedHash)
{
    QString savedHash = getSavedFaceHash();
    
    if (savedHash.isEmpty()) {
        qDebug() << "❌ [FACE ID] Aucun hash sauvegardé trouvé";
        return false;
    }
    
    // Calculer la distance de Hamming entre les signatures
    int distance = hammingDistance(capturedHash, savedHash);
    int maxDistance = savedHash.length() * 0.25; // Tolérance de 25%
    bool match = (distance <= maxDistance);
    
    qDebug() << "🔐 [FACE ID] Comparaison des signatures:";
    qDebug() << "  - Signature capturée (premiers 30 caractères):" << capturedHash.left(30) << "...";
    qDebug() << "  - Signature sauvegardée (premiers 30 caractères):" << savedHash.left(30) << "...";
    qDebug() << "  - Distance de Hamming:" << distance << "/" << savedHash.length();
    qDebug() << "  - Distance maximale tolérée:" << maxDistance;
    qDebug() << "  - Correspondance:" << (match ? "✅ OUI" : "❌ NON");
    
    return match;
}

void AUTHETIFICATION::recognizeFaceID()
{
    // Créer un dialogue pour la reconnaissance Face ID
    QDialog *faceIDDialog = new QDialog(this);
    faceIDDialog->setWindowTitle("Face ID - Reconnaissance");
    faceIDDialog->setModal(true);
    faceIDDialog->resize(800, 600);
    
    QVBoxLayout *layout = new QVBoxLayout(faceIDDialog);
    
    // Label d'instruction
    QLabel *instructionLabel = new QLabel("Reconnaissance Face ID en cours...\nPositionnez votre visage devant la caméra", faceIDDialog);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px; color: #333;");
    layout->addWidget(instructionLabel);
    
    // Zone d'affichage de la caméra
    QVideoWidget *viewfinder = new QVideoWidget(faceIDDialog);
    viewfinder->setMinimumSize(640, 480);
    viewfinder->setStyleSheet("border: 2px solid #2196F3; background-color: #000;");
    layout->addWidget(viewfinder);
    
    // Boutons
    QPushButton *btnRecognize = new QPushButton("🔍 Reconnaître", faceIDDialog);
    btnRecognize->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; padding: 12px 20px; border-radius: 5px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0b7dda; }"
        "QPushButton:disabled { background-color: #ccc; }"
    );
    
    QPushButton *btnCancel = new QPushButton("❌ Annuler", faceIDDialog);
    btnCancel->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; padding: 12px 20px; border-radius: 5px; font-size: 14px; }"
        "QPushButton:hover { background-color: #da190b; }"
    );
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnRecognize);
    buttonLayout->addWidget(btnCancel);
    layout->addLayout(buttonLayout);
    
    // Variables pour la caméra et l'image capturée
    QCamera *camera = nullptr;
    QImageCapture *imageCapture = nullptr;
    QMediaCaptureSession *captureSession = nullptr;
    QImage capturedImage;
    bool recognitionSuccessful = false;
    
    // Initialiser la caméra
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    
    if (cameras.isEmpty()) {
        QMessageBox::warning(faceIDDialog, "Erreur", 
            "Aucune caméra détectée sur cet appareil.\n"
            "Veuillez utiliser la connexion normale.");
        delete faceIDDialog;
        return;
    }
    
    // Utiliser la première caméra disponible
    camera = new QCamera(cameras.first(), faceIDDialog);
    imageCapture = new QImageCapture(faceIDDialog);
    captureSession = new QMediaCaptureSession(faceIDDialog);
    
    captureSession->setCamera(camera);
    captureSession->setImageCapture(imageCapture);
    captureSession->setVideoOutput(viewfinder);
    
    // Démarrer la caméra
    qDebug() << "🎥 [FACE ID] Démarrage de la caméra pour la reconnaissance...";
    camera->start();
    
    // Connecter le bouton de reconnaissance
    connect(btnRecognize, &QPushButton::clicked, [&]() {
        if (camera && camera->isActive() && imageCapture) {
            btnRecognize->setEnabled(false);
            instructionLabel->setText("Capture en cours...");
            imageCapture->capture();
        }
    });
    
    // Gérer l'image capturée
    connect(imageCapture, &QImageCapture::imageCaptured, [&](int id, const QImage &image) {
        Q_UNUSED(id);
        qDebug() << "📸 [FACE ID] Image capturée pour la reconnaissance - Taille:" << image.width() << "x" << image.height();
        capturedImage = image;
        
        // Normaliser l'image et générer une signature
        QImage normalizedImage = normalizeFaceImage(capturedImage);
        QString capturedHash = generateFaceSignature(normalizedImage);
        
        qDebug() << "🔐 [FACE ID] Signature générée pour la reconnaissance";
        
        // Vérifier le Face ID
        if (verifyFaceID(capturedHash)) {
            recognitionSuccessful = true;
            instructionLabel->setText("✅ Reconnaissance réussie !");
            instructionLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px; color: #4CAF50;");
            
            // Mettre à jour la date de dernière utilisation
            QSqlQuery query;
            query.prepare("UPDATE \"SYSTEM\".\"FACE_ID\" SET DATE_DERNIERE_UTILISATION = SYSDATE WHERE ROWNUM = 1");
            query.exec();
            
            QMessageBox::information(faceIDDialog, "Face ID Reconnu", 
                "✅ Face ID reconnu avec succès !\n\n"
                "Connexion en cours...");
            
            // Arrêter la caméra
            if (camera) {
                camera->stop();
            }
            
            // Fermer les dialogues et accepter l'authentification
            faceIDDialog->accept();
            this->accept();
        } else {
            recognitionSuccessful = false;
            instructionLabel->setText("❌ Reconnaissance échouée.\nVeuillez réessayer ou utiliser la connexion normale.");
            instructionLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px; color: #f44336;");
            btnRecognize->setEnabled(true);
            
            QMessageBox::warning(faceIDDialog, "Reconnaissance échouée", 
                "❌ Face ID non reconnu.\n\n"
                "Le visage capturé ne correspond pas au Face ID enregistré.\n"
                "Veuillez réessayer ou utiliser la connexion normale.");
        }
    });
    
    // Gérer les erreurs de capture
    connect(imageCapture, &QImageCapture::errorOccurred, [&](int id, QImageCapture::Error error, const QString &errorString) {
        Q_UNUSED(id);
        Q_UNUSED(error);
        QMessageBox::warning(faceIDDialog, "Erreur de capture", 
            QString("Erreur lors de la capture: %1").arg(errorString));
        btnRecognize->setEnabled(true);
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

QImage AUTHETIFICATION::normalizeFaceImage(const QImage &image)
{
    // Redimensionner l'image à une taille fixe pour la normalisation
    QImage normalized = image.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // Convertir en niveaux de gris
    if (normalized.format() != QImage::Format_Grayscale8) {
        normalized = normalized.convertToFormat(QImage::Format_Grayscale8);
    }
    
    // Appliquer un léger flou pour réduire le bruit
    QImage blurred = normalized;
    for (int y = 1; y < blurred.height() - 1; ++y) {
        for (int x = 1; x < blurred.width() - 1; ++x) {
            int sum = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    sum += normalized.pixelColor(x + dx, y + dy).value();
                }
            }
            int avg = sum / 9;
            blurred.setPixelColor(x, y, QColor(avg, avg, avg));
        }
    }
    
    return blurred;
}

QString AUTHETIFICATION::generateFaceSignature(const QImage &image)
{
    // Diviser l'image en blocs et calculer la moyenne de chaque bloc
    const int blockSize = 20; // 10x10 blocs pour une image 200x200
    const int blocksPerRow = image.width() / blockSize;
    const int blocksPerCol = image.height() / blockSize;
    
    QString signature;
    QByteArray signatureBytes;
    
    for (int by = 0; by < blocksPerCol; ++by) {
        for (int bx = 0; bx < blocksPerRow; ++bx) {
            int sum = 0;
            int count = 0;
            
            for (int y = by * blockSize; y < (by + 1) * blockSize && y < image.height(); ++y) {
                for (int x = bx * blockSize; x < (bx + 1) * blockSize && x < image.width(); ++x) {
                    sum += image.pixelColor(x, y).value();
                    count++;
                }
            }
            
            int avg = count > 0 ? sum / count : 0;
            signatureBytes.append(static_cast<char>(avg));
        }
    }
    
    // Générer un hash SHA256 de la signature pour la stocker
    QByteArray hash = QCryptographicHash::hash(signatureBytes, QCryptographicHash::Sha256);
    
    // Créer une signature binaire basée sur les moyennes (plus tolérante)
    QString binarySignature;
    for (int i = 0; i < signatureBytes.size(); ++i) {
        // Convertir chaque valeur en binaire (8 bits)
        unsigned char value = static_cast<unsigned char>(signatureBytes[i]);
        for (int bit = 7; bit >= 0; --bit) {
            binarySignature.append((value >> bit) & 1 ? '1' : '0');
        }
    }
    
    // Combiner le hash et la signature binaire pour une meilleure robustesse
    return hash.toHex() + "_" + binarySignature;
}

int AUTHETIFICATION::hammingDistance(const QString &sig1, const QString &sig2)
{
    // Séparer le hash et la signature binaire
    QStringList parts1 = sig1.split('_');
    QStringList parts2 = sig2.split('_');
    
    if (parts1.size() != 2 || parts2.size() != 2) {
        // Si le format n'est pas correct, comparer directement
        int distance = 0;
        int minLen = qMin(sig1.length(), sig2.length());
        for (int i = 0; i < minLen; ++i) {
            if (sig1[i] != sig2[i]) {
                distance++;
            }
        }
        distance += qAbs(sig1.length() - sig2.length());
        return distance;
    }
    
    QString binary1 = parts1[1];
    QString binary2 = parts2[1];
    
    // Calculer la distance de Hamming sur la partie binaire
    int distance = 0;
    int minLen = qMin(binary1.length(), binary2.length());
    
    for (int i = 0; i < minLen; ++i) {
        if (binary1[i] != binary2[i]) {
            distance++;
        }
    }
    
    // Ajouter la différence de longueur
    distance += qAbs(binary1.length() - binary2.length());
    
    return distance;
}

bool AUTHETIFICATION::deleteFaceID()
{
    QSqlQuery query;
    query.prepare("DELETE FROM \"SYSTEM\".\"FACE_ID\"");
    
    if (query.exec()) {
        int rowsAffected = query.numRowsAffected();
        qDebug() << "🗑️ [FACE ID] Face ID supprimé - Lignes affectées:" << rowsAffected;
        
        if (rowsAffected > 0) {
            QMessageBox::information(this, "Face ID Supprimé", 
                "✅ Face ID supprimé avec succès !\n\n"
                "Vous devrez vous connecter avec votre identifiant et mot de passe.");
            return true;
        } else {
            QMessageBox::information(this, "Aucun Face ID", 
                "ℹ️ Aucun Face ID n'était enregistré.");
            return false;
        }
    } else {
        QSqlError error = query.lastError();
        qDebug() << "❌ [FACE ID] Erreur lors de la suppression:" << error.text();
        QMessageBox::warning(this, "Erreur", 
            QString("❌ Erreur lors de la suppression du Face ID.\n\n"
                   "Message : %1\n"
                   "Code : %2")
                .arg(error.text())
                .arg(error.nativeErrorCode()));
        return false;
    }
}
