#ifndef AUTHETIFICATION_H
#define AUTHETIFICATION_H

#include <QDialog>
#include <QImage>
#include "InputController.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class AUTHETIFICATION : public QDialog
{
    Q_OBJECT

public:
    explicit AUTHETIFICATION(QWidget *parent = nullptr);
    ~AUTHETIFICATION();

private slots:
    void onConnexion();
    void onIdentifiantChanged();
    void onMotDePasseChanged();
    void onFaceID();

private:
    Ui::Dialog *ui;
    void validerChamps();
    void afficherErreur(const QString &champ, const QString &message);
    void effacerErreur(const QString &champ);
    
    // Méthodes Face ID
    QImage normalizeFaceImage(const QImage &image);
    QString generateFaceSignature(const QImage &image);
    bool saveFaceID(const QString &signature);
    bool hasFaceIDRegistered();
    QString getSavedFaceHash();
    bool verifyFaceID(const QString &capturedHash);
    void recognizeFaceID();
    void startFaceIDRegistration();
    int hammingDistance(const QString &sig1, const QString &sig2);
};

#endif // AUTHETIFICATION_H
