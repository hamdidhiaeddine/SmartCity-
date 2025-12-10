#ifndef FACIALRECOGNITION_H
#define FACIALRECOGNITION_H

#include <QString>
#include <QImage>
#include <QByteArray>

class FacialRecognition
{
public:
    FacialRecognition();
    
    // Capture une image depuis la webcam
    static QImage captureFromWebcam();
    
    // Enregistre l'image faciale d'un employé
    static bool saveFaceImage(int employeId, const QImage &image, QString &errorText);
    
    // Charge l'image faciale d'un employé
    static QImage loadFaceImage(int employeId, QString &errorText);
    
    // Vérifie si un employé a une image faciale enregistrée
    static bool hasFaceImage(int employeId);
    
    // Convertit QImage en QByteArray pour stockage en base
    static QByteArray imageToByteArray(const QImage &image);
    
    // Convertit QByteArray en QImage depuis la base
    static QImage byteArrayToImage(const QByteArray &data);
    
    // Compare deux images faciales (simple comparaison de similarité)
    static double compareFaces(const QImage &image1, const QImage &image2);
    
    // Authentifie un employé par reconnaissance faciale
    static int authenticateByFace(const QImage &capturedImage, QString &errorText);
    
private:
    static QString getFaceImagePath(int employeId);
};

#endif // FACIALRECOGNITION_H
