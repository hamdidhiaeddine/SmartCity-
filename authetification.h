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

private:
    Ui::Dialog *ui;
    void validerChamps();
    void afficherErreur(const QString &champ, const QString &message);
    void effacerErreur(const QString &champ);
};

#endif // AUTHETIFICATION_H
