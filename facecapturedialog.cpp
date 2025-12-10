#include "facecapturedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QHBoxLayout>
#include <QMediaDevices>

FaceCaptureDialog::FaceCaptureDialog(QWidget *parent)
    : QDialog(parent)
    , camera(nullptr)
    , imageCapture(nullptr)
    , captureSession(nullptr)
    , cameraMode(false)
{
    setupUI();
    setWindowTitle("Capture de Visage");
    resize(600, 700);
}

FaceCaptureDialog::~FaceCaptureDialog()
{
    stopCamera();
}

void FaceCaptureDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Label pour afficher l'image
    imageLabel = new QLabel(this);
    imageLabel->setMinimumSize(500, 400);
    imageLabel->setMaximumSize(500, 400);
    imageLabel->setScaledContents(true);
    imageLabel->setStyleSheet("border: 2px solid #ccc; background-color: #f0f0f0;");
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText("Aucune image sélectionnée");
    
    // Widget vidéo pour la caméra (caché par défaut)
    videoWidget = new QVideoWidget(this);
    videoWidget->setMinimumSize(500, 400);
    videoWidget->setMaximumSize(500, 400);
    videoWidget->hide();
    
    // Layout pour les boutons de sélection
    QHBoxLayout *sourceLayout = new QHBoxLayout();
    
    // Bouton pour sélectionner une image
    selectButton = new QPushButton("📁 Sélectionner une Photo", this);
    selectButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
    );
    connect(selectButton, &QPushButton::clicked, this, &FaceCaptureDialog::onSelectImage);
    
    // Bouton pour capturer depuis la caméra
    cameraButton = new QPushButton("📷 Ouvrir Caméra", this);
    cameraButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0b7dda;"
        "}"
    );
    connect(cameraButton, &QPushButton::clicked, this, &FaceCaptureDialog::onCaptureFromCamera);
    
    sourceLayout->addWidget(selectButton);
    sourceLayout->addWidget(cameraButton);
    
    // Bouton pour capturer la photo (visible seulement en mode caméra)
    captureButton = new QPushButton("📸 Capturer Photo", this);
    captureButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9800;"
        "   color: white;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F57C00;"
        "}"
    );
    captureButton->hide();
    connect(captureButton, &QPushButton::clicked, this, [this]() {
        if (imageCapture) {
            imageCapture->capture();
        }
    });
    
    // Layout pour les boutons de validation
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    acceptButton = new QPushButton("Valider", this);
    acceptButton->setEnabled(false);
    acceptButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0b7dda;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "   color: #666666;"
        "}"
    );
    connect(acceptButton, &QPushButton::clicked, this, &FaceCaptureDialog::onAccept);
    
    cancelButton = new QPushButton("Annuler", this);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #da190b;"
        "}"
    );
    connect(cancelButton, &QPushButton::clicked, this, &FaceCaptureDialog::onCancel);
    
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(cancelButton);
    
    // Ajouter tous les widgets au layout principal
    mainLayout->addWidget(imageLabel, 0, Qt::AlignCenter);
    mainLayout->addWidget(videoWidget, 0, Qt::AlignCenter);
    mainLayout->addLayout(sourceLayout);
    mainLayout->addWidget(captureButton, 0, Qt::AlignCenter);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

void FaceCaptureDialog::onSelectImage()
{
    stopCamera(); // Arrêter la caméra si elle est active
    
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Sélectionner une photo de visage",
        QString(),
        "Images (*.png *.jpg *.jpeg *.bmp)"
    );
    
    if (!fileName.isEmpty()) {
        capturedImage = QImage(fileName);
        
        if (capturedImage.isNull()) {
            QMessageBox::warning(this, "Erreur", "Impossible de charger l'image sélectionnée.");
            return;
        }
        
        // Afficher l'image dans le label
        imageLabel->show();
        videoWidget->hide();
        QPixmap pixmap = QPixmap::fromImage(capturedImage);
        imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        
        // Activer le bouton Valider
        acceptButton->setEnabled(true);
    }
}

void FaceCaptureDialog::onCaptureFromCamera()
{
    if (!cameraMode) {
        startCamera();
    } else {
        stopCamera();
    }
}

void FaceCaptureDialog::startCamera()
{
    // Vérifier si une caméra est disponible
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        QMessageBox::warning(this, "Erreur", 
            "Aucune caméra détectée.\nVeuillez connecter une webcam ou sélectionner une photo.");
        return;
    }
    
    // Créer et démarrer la caméra
    camera = new QCamera(cameras.first(), this);
    imageCapture = new QImageCapture(this);
    captureSession = new QMediaCaptureSession(this);
    
    // Configurer la session de capture
    captureSession->setCamera(camera);
    captureSession->setImageCapture(imageCapture);
    captureSession->setVideoOutput(videoWidget);
    
    // Connecter le signal de capture
    connect(imageCapture, &QImageCapture::imageCaptured, 
            this, &FaceCaptureDialog::onImageCaptured);
    
    // Démarrer la caméra
    camera->start();
    
    // Afficher le widget vidéo
    imageLabel->hide();
    videoWidget->show();
    captureButton->show();
    
    cameraMode = true;
    cameraButton->setText("🔴 Fermer Caméra");
    cameraButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #da190b;"
        "}"
    );
}

void FaceCaptureDialog::stopCamera()
{
    if (camera) {
        camera->stop();
        camera->deleteLater();
        camera = nullptr;
    }
    
    if (imageCapture) {
        imageCapture->deleteLater();
        imageCapture = nullptr;
    }
    
    if (captureSession) {
        captureSession->deleteLater();
        captureSession = nullptr;
    }
    
    videoWidget->hide();
    captureButton->hide();
    imageLabel->show();
    
    cameraMode = false;
    cameraButton->setText("📷 Ouvrir Caméra");
    cameraButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0b7dda;"
        "}"
    );
}

void FaceCaptureDialog::onImageCaptured(int id, const QImage &preview)
{
    Q_UNUSED(id);
    
    capturedImage = preview;
    
    if (!capturedImage.isNull()) {
        // Arrêter la caméra
        stopCamera();
        
        // Afficher l'image capturée
        imageLabel->show();
        QPixmap pixmap = QPixmap::fromImage(capturedImage);
        imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        
        // Activer le bouton Valider
        acceptButton->setEnabled(true);
        
        QMessageBox::information(this, "Succès", "Photo capturée avec succès!");
    } else {
        QMessageBox::warning(this, "Erreur", "Échec de la capture de l'image.");
    }
}

void FaceCaptureDialog::onAccept()
{
    if (capturedImage.isNull()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner ou capturer une photo.");
        return;
    }
    
    stopCamera();
    accept();
}

void FaceCaptureDialog::onCancel()
{
    stopCamera();
    reject();
}
