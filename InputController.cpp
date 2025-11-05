#include "InputController.h"
#include <QRegularExpression>

static inline bool isEmptyOrNull(const QString &s) { return s.trimmed().isEmpty(); }

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

ValidationResult InputController::validateCreate(const EmployeeInput &in)
{
    ValidationResult r;
    Employee e;

    e.nom = normalizeNameLike(in.nom);
    if (e.nom.isEmpty() || e.nom.size() > 20) r.fieldErrors.insert("nom", "Nom requis (1-20)");

    e.prenom = normalizeNameLike(in.prenom);
    if (e.prenom.isEmpty() || e.prenom.size() > 20) r.fieldErrors.insert("prenom", "Prénom requis (1-20)");

    e.poste = trimmedSingleSpaced(in.poste);
    if (e.poste.isEmpty() || e.poste.size() > 20) r.fieldErrors.insert("poste", "Poste requis (1-20)");

    QString salaireErr;
    if (!parseSalaire(in.salaire, e.salaire, salaireErr)) r.fieldErrors.insert("salaire", salaireErr);

    e.idRes = trimmedSingleSpaced(in.idRes);
    if (!e.idRes.isEmpty() && e.idRes.size() > 20) r.fieldErrors.insert("idRes", "ID_RES max 20");

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


