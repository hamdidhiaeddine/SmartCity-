#ifndef AUTHENTIFICATION_H
#define AUTHENTIFICATION_H

#include <QDialog>

namespace Ui {
class Authentification;
}

class Authentification : public QDialog
{
    Q_OBJECT

public:
    explicit Authentification(QWidget *parent = nullptr);
    ~Authentification();
    
    QString getIdentifiant() const;
    QString getMotDePasse() const;
    bool isAuthentifie() const { return m_authentifie; }

private slots:
    void onConnexionClicked();
    void onInscriptionClicked();
    void onMotDePasseOublieClicked();
    void onConnexionVisage();

private:
    Ui::Authentification *ui;
    bool m_authentifie;
    
    bool validerConnexion(const QString &identifiant, const QString &motDePasse);
    bool validerConnexionVisage();
};

#endif // AUTHENTIFICATION_H
