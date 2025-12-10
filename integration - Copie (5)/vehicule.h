#ifndef VEHICULE_H
#define VEHICULE_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDate>

class Vehicule
{
private:
    int id_vehi;
    QString immat, marque, modele, type, etat, service;
    QDate date_maint;

public:
    Vehicule();
    Vehicule(QString immat, QString marque, QString modele, QString type, QString etat, QString service, QDate date_maint);

    // CRUD
    bool ajouter();
    QSqlQuery afficher();
    bool modifier(QString oldImmat);
    bool supprimer(QString immat);
};

#endif // VEHICULE_H
