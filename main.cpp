#include "mainwindow.h"
<<<<<<< HEAD
#include "authetification.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"
=======

#include <QApplication>
>>>>>>> ca35564c4967a2d463a1f10ffb114ccf76ef4dbf

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
<<<<<<< HEAD
    
    // Étape 1: Vérifier la connexion à la base de données
    Connection c;
    bool test = c.createconnect();
    if (!test) {
        QMessageBox::critical(nullptr, QObject::tr("Erreur de base de données"),
                              QObject::tr("La connexion à la base de données a échoué.\n"
                                          "L'application ne peut pas démarrer."), 
                              QMessageBox::Ok);
        return -1; // Quitter l'application avec un code d'erreur
    }
    
    // Étape 2: Afficher le dialogue d'authentification
    AUTHETIFICATION authDialog;
    
    // Si l'utilisateur annule ou ferme le dialogue d'authentification, quitter
    if (authDialog.exec() != QDialog::Accepted) {
        return 0; // Quitter proprement si l'authentification est annulée
    }
    
    // Étape 3: Si l'authentification réussit, afficher la fenêtre principale
    MainWindow w;
    w.show();
    
    // Afficher un message de succès de connexion (optionnel)
    QMessageBox::information(nullptr, QObject::tr("Connexion réussie"),
                             QObject::tr("Connexion à la base de données réussie.\n"
                                         "Bienvenue dans l'application."), 
                             QMessageBox::Ok);

=======
    MainWindow w;
    w.show();
>>>>>>> ca35564c4967a2d463a1f10ffb114ccf76ef4dbf
    return a.exec();
}
