#include "InputController.h"
#include <QRegularExpression>

QString InputController::trimmedSingleSpaced(const QString &value)
{
    QString v = value;
    v = v.trimmed();
    v.replace(QRegularExpression("\\s+"), " ");
    return v;
}

QString InputController::normalizeNameLike(const QString &value)
{
    QString v = trimmedSingleSpaced(value);
    if (v.isEmpty()) return v;
    // Capitalize first letter of each word
    QStringList parts = v.split(' ', Qt::SkipEmptyParts);
    for (QString &p : parts) {
        if (!p.isEmpty()) {
            p[0] = p[0].toUpper();
        }
    }
    return parts.join(' ');
}

bool InputController::parseSalaire(const QString &text, double &out, QString &err)
{
    bool ok = false;
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        err = "Salaire requis";
        return false;
    }
    out = trimmed.toDouble(&ok);
    if (!ok || !qIsFinite(out)) {
        err = "Salaire invalide";
        return false;
    }
    if (out < 0.0) {
        err = "Salaire doit être >= 0";
        return false;
    }
    // Limit to 2 decimals logically (not strictly necessary for storage)
    double rounded = qRound64(out * 100.0) / 100.0;
    out = rounded;
    return true;
}

bool InputController::parseTelephone(const QString &text, long long &out, QString &err)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        err = "Téléphone requis";
        return false;
    }
    
    // Vérifier que c'est un nombre valide
    bool ok = false;
    out = trimmed.toLongLong(&ok);
    if (!ok) {
        err = "Téléphone invalide (doit être un nombre)";
        return false;
    }
    
    // Vérifier la longueur (généralement 8-15 chiffres)
    QString digitsOnly = trimmed;
    digitsOnly.remove(QRegularExpression("[^0-9]"));
    if (digitsOnly.length() < 8 || digitsOnly.length() > 15) {
        err = "Téléphone doit contenir entre 8 et 15 chiffres";
        return false;
    }
    
    if (out < 0) {
        err = "Téléphone doit être positif";
        return false;
    }
    
    return true;
}

bool InputController::validateEmail(const QString &email, QString &error)
{
    const QString trimmed = email.trimmed();
    
    if (trimmed.isEmpty()) {
        error = "Email requis";
        return false;
    }
    
    if (trimmed.length() > 30) {
        error = "Email ne peut pas dépasser 30 caractères";
        return false;
    }
    
    // Validation basique du format email
    QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!regex.match(trimmed).hasMatch()) {
        error = "Format d'email invalide";
        return false;
    }
    
    return true;
}

bool InputController::validateTelephone(const QString &telephone, QString &error)
{
    long long dummy;
    return parseTelephone(telephone, dummy, error);
}

ValidationResult InputController::validateCreate(const EmployeeInput &in)
{
    ValidationResult r;
    Employee e;

    e.nom = normalizeNameLike(in.nom);
    if (e.nom.isEmpty() || e.nom.size() > 20) r.fieldErrors.insert("nom", "Nom requis (1-20 caractères)");

    e.prenom = normalizeNameLike(in.prenom);
    if (e.prenom.isEmpty() || e.prenom.size() > 20) r.fieldErrors.insert("prenom", "Prénom requis (1-20 caractères)");

    e.poste = trimmedSingleSpaced(in.poste);
    if (e.poste.isEmpty() || e.poste.size() > 20) r.fieldErrors.insert("poste", "Poste requis (1-20 caractères)");

    QString salaireErr;
    if (!parseSalaire(in.salaire, e.salaire, salaireErr)) r.fieldErrors.insert("salaire", salaireErr);

    e.adresse = trimmedSingleSpaced(in.adresse);
    if (e.adresse.isEmpty() || e.adresse.size() > 20) r.fieldErrors.insert("adresse", "Adresse requise (1-20 caractères)");

    QString telephoneErr;
    if (!parseTelephone(in.telephone, e.telephone, telephoneErr)) {
        r.fieldErrors.insert("telephone", telephoneErr);
    }

    QString emailErr;
    if (!validateEmail(in.email, emailErr)) {
        r.fieldErrors.insert("email", emailErr);
    } else {
        e.email = in.email.trimmed();
    }

    // idRes est optionnel et n'est pas stocké dans Employee (uniquement dans EmployeeFilter)
    // Validation de idRes si fourni (pour filtres uniquement)
    if (!in.idRes.isEmpty()) {
        QString idResNormalized = trimmedSingleSpaced(in.idRes);
        if (idResNormalized.size() > 20) {
            r.fieldErrors.insert("idRes", "ID_RES max 20 caractères");
        }
    }

    r.ok = r.fieldErrors.isEmpty();
    r.normalized = e;
    return r;
}

ValidationResult InputController::validateUpdate(const EmployeeInput &in)
{
    // For now, same rules as create; could allow partials if needed
    return validateCreate(in);
}

bool InputController::validateFilters(EmployeeFilter &filter, QMap<QString, QString> &errors)
{
    if (filter.search.size() > 50) {
        errors.insert("search", "Recherche trop longue");
    }
    if (filter.poste.size() > 20) {
        errors.insert("poste", "Poste max 20");
    }
    if (filter.idRes.size() > 20) {
        errors.insert("idRes", "ID_RES max 20");
    }
    if (filter.minSalaire >= 0 && filter.maxSalaire >= 0 && filter.minSalaire > filter.maxSalaire) {
        errors.insert("salaire", "minSalaire > maxSalaire");
    }
    return errors.isEmpty();
}

// Validation pour l'authentification
bool InputController::validateIdentifiant(const QString &identifiant, QString &error)
{
    const QString trimmed = identifiant.trimmed();
    
    if (trimmed.isEmpty()) {
        error = "L'identifiant est requis";
        return false;
    }
    
    if (trimmed.length() < 3) {
        error = "L'identifiant doit contenir au moins 3 caractères";
        return false;
    }
    
    if (trimmed.length() > 50) {
        error = "L'identifiant ne peut pas dépasser 50 caractères";
        return false;
    }
    
    // Vérifier que l'identifiant ne contient que des caractères alphanumériques et certains caractères spéciaux
    QRegularExpression regex("^[a-zA-Z0-9_@.-]+$");
    if (!regex.match(trimmed).hasMatch()) {
        error = "L'identifiant ne peut contenir que des lettres, chiffres et les caractères _ @ . -";
        return false;
    }
    
    return true;
}

bool InputController::validateMotDePasse(const QString &motDePasse, QString &error)
{
    if (motDePasse.isEmpty()) {
        error = "Le mot de passe est requis";
        return false;
    }
    
    if (motDePasse.length() < 6) {
        error = "Le mot de passe doit contenir au moins 6 caractères";
        return false;
    }
    
    if (motDePasse.length() > 100) {
        error = "Le mot de passe ne peut pas dépasser 100 caractères";
        return false;
    }
    
    return true;
}

AuthValidationResult InputController::validateAuth(const AuthInput &in)
{
    AuthValidationResult result;
    
    QString identifiantErr;
    QString normalizedIdentifiant = in.identifiant.trimmed();
    if (!validateIdentifiant(in.identifiant, identifiantErr)) {
        result.fieldErrors.insert("identifiant", identifiantErr);
    }
    
    QString motDePasseErr;
    if (!validateMotDePasse(in.motDePasse, motDePasseErr)) {
        result.fieldErrors.insert("motDePasse", motDePasseErr);
    }
    
    result.ok = result.fieldErrors.isEmpty();
    result.normalizedIdentifiant = normalizedIdentifiant;
    
    return result;
}


