#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"
#include <QSqlDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Connexion BD
    Connection c;
    bool test = c.createconnect();

    if (test) {

        // Récupérer la connexion NOMMÉE ("qt_oracle")
        QSqlDatabase db = QSqlDatabase::database("qt_oracle");

        qDebug() << "=== Connexion BD OK ===";
        qDebug() << "isValid =" << db.isValid();
        qDebug() << "isOpen  =" << db.isOpen();

        // Créer automatiquement la table GEST_ALERTES si elle n'existe pas
        c.createTableAlertes();

    } else {
        qDebug() << "=== ECHEC connexion BD ===";
    }

    MainWindow w;
    w.show();

    qDebug() << "Drivers SQL disponibles :" << QSqlDatabase::drivers();

    return a.exec();
}
