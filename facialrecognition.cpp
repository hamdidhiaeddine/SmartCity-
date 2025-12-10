#include "facialrecognition.h"
#include "connection.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QBuffer>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QVariant>

FacialRecognition::FacialRecognition()
{
}

QString FacialRecognition::getFaceImagePath(int employeId)
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath + "/FaceImages");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.filePath(QString("employee_%1.jpg").arg(employeId));
}

QImage FacialRecognition::captureFromWebcam()
{
    // Pour l'instant, retourne une image vide
    // L'utilisateur devra sélectionner une image
    return QImage();
}

bool FacialRecognition::saveFaceImage(int employeId, const QImage &image, QString &errorText)
{
    if (image.isNull()) {
        errorText = "Image invalide";
        return false;
    }
    
    // Sauvegarde locale de l'image
    QString imagePath = getFaceImagePath(employeId);
    if (!image.save(imagePath, "JPG", 90)) {
        errorText = "Erreur lors de la sauvegarde de l'image";
        return false;
    }
    
    // Convertir l'image en QByteArray pour la base de données
    QByteArray imageData = imageToByteArray(image);
    
    // Sauvegarder dans la base de données
    QSqlQuery query;
    query.prepare("UPDATE EMPLOYES SET PHOTO = :photo WHERE ID_EMPLOYE = :id");
    query.bindValue(":photo", imageData);
    query.bindValue(":id", employeId);
    
    if (!query.exec()) {
        errorText = "Erreur base de données: " + query.lastError().text();
        return false;
    }
    
    return true;
}

QImage FacialRecognition::loadFaceImage(int employeId, QString &errorText)
{
    // Essayer de charger depuis la base de données
    QSqlQuery query;
    query.prepare("SELECT PHOTO FROM EMPLOYES WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", employeId);
    
    if (!query.exec()) {
        errorText = "Erreur base de données: " + query.lastError().text();
        return QImage();
    }
    
    if (query.next()) {
        QByteArray imageData = query.value(0).toByteArray();
        if (!imageData.isEmpty()) {
            return byteArrayToImage(imageData);
        }
    }
    
    // Si pas dans la base, essayer le fichier local
    QString imagePath = getFaceImagePath(employeId);
    if (QFile::exists(imagePath)) {
        QImage image(imagePath);
        if (!image.isNull()) {
            return image;
        }
    }
    
    errorText = "Aucune photo trouvée pour cet employé";
    return QImage();
}

bool FacialRecognition::hasFaceImage(int employeId)
{
    QSqlQuery query;
    query.prepare("SELECT PHOTO FROM EMPLOYES WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", employeId);
    
    if (query.exec() && query.next()) {
        QByteArray imageData = query.value(0).toByteArray();
        if (!imageData.isEmpty()) {
            return true;
        }
    }
    
    // Vérifier aussi le fichier local
    QString imagePath = getFaceImagePath(employeId);
    return QFile::exists(imagePath);
}

QByteArray FacialRecognition::imageToByteArray(const QImage &image)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG", 90);
    return byteArray;
}

QImage FacialRecognition::byteArrayToImage(const QByteArray &data)
{
    QImage image;
    image.loadFromData(data, "JPG");
    return image;
}

double FacialRecognition::compareFaces(const QImage &image1, const QImage &image2)
{
    // Comparaison simple basée sur la similarité d'histogramme
    // Pour une vraie reconnaissance faciale, il faudrait utiliser OpenCV ou une bibliothèque spécialisée
    
    if (image1.isNull() || image2.isNull()) {
        return 0.0;
    }
    
    // Redimensionner les images à la même taille
    QImage img1 = image1.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage img2 = image2.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    
    // Calculer la différence pixel par pixel
    int totalPixels = img1.width() * img1.height();
    int similarPixels = 0;
    
    for (int y = 0; y < img1.height(); ++y) {
        for (int x = 0; x < img1.width(); ++x) {
            QRgb pixel1 = img1.pixel(x, y);
            QRgb pixel2 = img2.pixel(x, y);
            
            int rDiff = qAbs(qRed(pixel1) - qRed(pixel2));
            int gDiff = qAbs(qGreen(pixel1) - qGreen(pixel2));
            int bDiff = qAbs(qBlue(pixel1) - qBlue(pixel2));
            
            // Si la différence est faible, on considère les pixels similaires
            if (rDiff < 30 && gDiff < 30 && bDiff < 30) {
                similarPixels++;
            }
        }
    }
    
    return (double)similarPixels / totalPixels;
}

int FacialRecognition::authenticateByFace(const QImage &capturedImage, QString &errorText)
{
    if (capturedImage.isNull()) {
        errorText = "Image capturée invalide";
        return -1;
    }
    
    // Récupérer tous les employés avec photo
    QSqlQuery query("SELECT ID_EMPLOYE, PHOTO FROM EMPLOYES WHERE PHOTO IS NOT NULL");
    
    int bestMatchId = -1;
    double bestMatchScore = 0.0;
    const double THRESHOLD = 0.7; // 70% de similarité minimum
    
    while (query.next()) {
        int employeId = query.value(0).toInt();
        QByteArray imageData = query.value(1).toByteArray();
        
        if (!imageData.isEmpty()) {
            QImage storedImage = byteArrayToImage(imageData);
            double score = compareFaces(capturedImage, storedImage);
            
            qDebug() << "Employé" << employeId << "- Score:" << score;
            
            if (score > bestMatchScore && score >= THRESHOLD) {
                bestMatchScore = score;
                bestMatchId = employeId;
            }
        }
    }
    
    if (bestMatchId == -1) {
        errorText = "Aucun visage correspondant trouvé";
        return -1;
    }
    
    qDebug() << "Meilleure correspondance: Employé" << bestMatchId << "avec score" << bestMatchScore;
    return bestMatchId;
}
