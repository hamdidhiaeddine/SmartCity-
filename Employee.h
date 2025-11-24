#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QString>
#include <QMap>

struct Employee
{
    int idEmploye = 0;
    QString nom;
    QString prenom;
    QString poste;
    double salaire = 0.0;
    QString adresse;
    long long telephone = 0;
    QString email;

    // CRUD instance helpers
    bool insert(int &outId, QString &errorText) const;        // create
    bool updateById(int id, QString &errorText) const;         // update
    static bool removeById(int id, QString &errorText);        // delete
    static QVector<Employee> fetchAll(QString &errorText);     // read (list)
    
    // Statistiques par poste
    static QMap<QString, int> getStatistiquesParPoste(QString &errorText);  // Retourne un map: poste -> nombre d'employés
};

struct EmployeeFilter
{
    QString search;      // matches nom/prenom/poste prefix (case-insensitive)
    QString poste;
    QString idRes;
    double minSalaire = -1.0; // < 0 => ignore
    double maxSalaire = -1.0; // < 0 => ignore
};

struct SortSpec
{
    QString column; // allowed: nom, prenom, poste, salaire, id_employe
    bool ascending = true;
};

#endif // EMPLOYEE_H


