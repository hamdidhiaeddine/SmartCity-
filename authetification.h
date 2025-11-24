#ifndef AUTHETIFICATION_H
#define AUTHETIFICATION_H

#include <QDialog>
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
    
    // Méthodes pour la gestion Face ID
    bool hasFaceIDRegistered();
    bool saveFaceID(const QString &faceHash);
    bool verifyFaceID(const QString &faceHash);
    QString generateFaceHash(const QImage &image);
    QImage normalizeFaceImage(const QImage &image);
    bool tableFaceIDExists();
    int hammingDistance(const QString &hash1, const QString &hash2);
};

#endif // AUTHETIFICATION_H
