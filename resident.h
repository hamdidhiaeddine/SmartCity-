#ifndef RESIDENT_H
#define RESIDENT_H

#include <QString>
#include <QDate>
#include <QVector>

struct Resident
{
    int idResident = 0;
    QString nom;
    QString prenom;
    QDate dtNaissance;
    QString adresse;
    long long telephone = 0;
    QString email;
    QString statut;
    QString situationFamiliale;

    // CRUD instance helpers
    bool insert(int &outId, QString &errorText) const;        // create
    bool updateById(int oldId, int newId = -1, QString *errorText = nullptr) const;         // update (oldId = ID à modifier, newId = nouvel ID si -1, garde l'ancien)
    static bool removeById(int id, QString &errorText);        // delete
    static QVector<Resident> fetchAll(QString &errorText);     // read (list)
    static bool idExists(int id); // Vérifie si un ID existe déjà
    
    // Validation methods
    static bool validateNom(const QString &nom, QString &error);
    static bool validatePrenom(const QString &prenom, QString &error);
    static bool validateDateNaissance(const QDate &date, QString &error);
    static bool validateAdresse(const QString &adresse, QString &error);
    static bool validateTelephone(long long telephone, QString &error);
    static bool validateEmail(const QString &email, QString &error);
    static bool validateStatut(const QString &statut, QString &error);
    static bool validateSituationFamiliale(const QString &situationFamiliale, QString &error);
    static bool validateAll(const Resident &r, QString &error);
};

#endif // RESIDENT_H
