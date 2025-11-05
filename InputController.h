#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QString>
#include <QMap>
#include <QVariant>
#include "Employee.h"

struct EmployeeInput
{
    QString nom;
    QString prenom;
    QString poste;
    QString salaire; // raw text from UI
    QString idRes;   // optional
};

struct ValidationResult
{
    bool ok = false;
    QMap<QString, QString> fieldErrors; // field -> message
    Employee normalized;
};

class InputController
{
public:
    static ValidationResult validateCreate(const EmployeeInput &in);
    static ValidationResult validateUpdate(const EmployeeInput &in);
    static bool validateFilters(EmployeeFilter &filter, QMap<QString, QString> &errors);
    static QString normalizeNameLike(const QString &value);
private:
    static QString trimmedSingleSpaced(const QString &value);
    static bool parseSalaire(const QString &text, double &out, QString &err);
};

#endif // INPUTCONTROLLER_H


