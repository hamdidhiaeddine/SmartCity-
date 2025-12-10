#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"
#include "authentification.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // ✅ Étape 1: Connexion à la base de données
    Connection c;
    bool test = c.createconnect();
    if (!test) {
        QMessageBox::critical(nullptr, QObject::tr("Erreur de connexion"),
                              QObject::tr("Impossible de se connecter à la base de données.\n"
                                          "Vérifiez vos paramètres de connexion."), 
                              QMessageBox::Cancel);
        return -1;
    }
    
    QMessageBox::information(nullptr, 
                             QObject::tr("Base de données"),
                             QObject::tr("Connexion à la base de données réussie !\n\n"
                                         "Authentifiez-vous pour continuer."), 
                             QMessageBox::Ok);
    
    // ✅ Étape 2: Authentification
    Authentification authDialog;
    if (authDialog.exec() != QDialog::Accepted) {
        // L'utilisateur a annulé ou fermé le dialog d'authentification
        return 0;
    }
    
    // ✅ Étape 3: Lancer l'application principale
    MainWindow w;
    w.show();
    
    // Rafraîchir les résidents après la connexion DB
    w.rafraichirResidents();
    
    return a.exec();
}
