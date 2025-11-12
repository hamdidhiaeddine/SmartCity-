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
    QString adresse;
    QString telephone; // raw text from UI
    QString email;
    QString idRes;   // optional
};

struct ValidationResult
{
    bool ok = false;
    QMap<QString, QString> fieldErrors; // field -> message
    Employee normalized;
};

struct AuthInput
{
    QString identifiant;
    QString motDePasse;
};

struct AuthValidationResult
{
    bool ok = false;
    QMap<QString, QString> fieldErrors; // field -> message
    QString normalizedIdentifiant;
};

class InputController
{
public:
    static ValidationResult validateCreate(const EmployeeInput &in);
    static ValidationResult validateUpdate(const EmployeeInput &in);
    static bool validateFilters(EmployeeFilter &filter, QMap<QString, QString> &errors);
    static QString normalizeNameLike(const QString &value);
    
    // Validation pour l'authentification
    static AuthValidationResult validateAuth(const AuthInput &in);
    static bool validateIdentifiant(const QString &identifiant, QString &error);
    static bool validateMotDePasse(const QString &motDePasse, QString &error);
    
    // Validation pour les employés (méthodes publiques pour validation en temps réel)
    static bool validateEmail(const QString &email, QString &error);
    static bool validateTelephone(const QString &telephone, QString &error);
    
private:
    static QString trimmedSingleSpaced(const QString &value);
    static bool parseSalaire(const QString &text, double &out, QString &err);
    static bool parseTelephone(const QString &text, long long &out, QString &err);
};

#endif // INPUTCONTROLLER_H


