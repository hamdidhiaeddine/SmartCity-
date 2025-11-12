#include "resident.h"
#include "InputController.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QRegularExpression>
#include <QDate>

// ========== VALIDATION METHODS ==========

bool Resident::validateNom(const QString &nom, QString &error)
{
    QString trimmed = nom.trimmed();
    if (trimmed.isEmpty()) {
        error = "Le nom est requis";
        return false;
    }
    if (trimmed.length() > 20) {
        error = "Le nom ne peut pas dépasser 20 caractères";
        return false;
    }
    // Normaliser les espaces multiples
    trimmed.replace(QRegularExpression("\\s+"), " ");
    return true;
}

bool Resident::validatePrenom(const QString &prenom, QString &error)
{
    QString trimmed = prenom.trimmed();
    if (trimmed.isEmpty()) {
        error = "Le prénom est requis";
        return false;
    }
    if (trimmed.length() > 20) {
        error = "Le prénom ne peut pas dépasser 20 caractères";
        return false;
    }
    // Normaliser les espaces multiples
    trimmed.replace(QRegularExpression("\\s+"), " ");
    return true;
}

bool Resident::validateDateNaissance(const QDate &date, QString &error)
{
    if (!date.isValid()) {
        error = "La date de naissance est invalide";
        return false;
    }
    QDate today = QDate::currentDate();
    if (date > today) {
        error = "La date de naissance ne peut pas être dans le futur";
        return false;
    }
    // Vérifier que la personne n'est pas trop âgée (par exemple, plus de 150 ans)
    int age = today.year() - date.year();
    if (today < date.addYears(age)) {
        age--;
    }
    if (age > 150) {
        error = "La date de naissance semble invalide (âge > 150 ans)";
        return false;
    }
    return true;
}

bool Resident::validateAdresse(const QString &adresse, QString &error)
{
    QString trimmed = adresse.trimmed();
    if (trimmed.isEmpty()) {
        error = "L'adresse est requise";
        return false;
    }
    if (trimmed.length() > 20) {
        error = "L'adresse ne peut pas dépasser 20 caractères";
        return false;
    }
    return true;
}

bool Resident::validateTelephone(long long telephone, QString &error)
{
    if (telephone <= 0) {
        error = "Le téléphone doit être un nombre positif";
        return false;
    }
    QString telStr = QString::number(telephone);
    QString digitsOnly = telStr;
    digitsOnly.remove(QRegularExpression("[^0-9]"));
    if (digitsOnly.length() < 8 || digitsOnly.length() > 15) {
        error = "Le téléphone doit contenir entre 8 et 15 chiffres";
        return false;
    }
    return true;
}

bool Resident::validateEmail(const QString &email, QString &error)
{
    return InputController::validateEmail(email, error);
}

bool Resident::validateStatut(const QString &statut, QString &error)
{
    QString trimmed = statut.trimmed();
    if (trimmed.isEmpty()) {
        error = "Le statut est requis";
        return false;
    }
    if (trimmed.length() > 20) {
        error = "Le statut ne peut pas dépasser 20 caractères";
        return false;
    }
    return true;
}

bool Resident::validateSituationFamiliale(const QString &situationFamiliale, QString &error)
{
    QString trimmed = situationFamiliale.trimmed();
    // SITUATION_FAMILILE est optionnel (peut être vide)
    if (trimmed.length() > 20) {
        error = "La situation familiale ne peut pas dépasser 20 caractères";
        return false;
    }
    return true;
}

bool Resident::validateAll(const Resident &r, QString &error)
{
    QString err;
    if (!validateNom(r.nom, err)) {
        error = "Nom: " + err;
        return false;
    }
    if (!validatePrenom(r.prenom, err)) {
        error = "Prénom: " + err;
        return false;
    }
    if (!validateDateNaissance(r.dtNaissance, err)) {
        error = "Date de naissance: " + err;
        return false;
    }
    if (!validateAdresse(r.adresse, err)) {
        error = "Adresse: " + err;
        return false;
    }
    if (!validateTelephone(r.telephone, err)) {
        error = "Téléphone: " + err;
        return false;
    }
    if (!validateEmail(r.email, err)) {
        error = "Email: " + err;
        return false;
    }
    if (!validateStatut(r.statut, err)) {
        error = "Statut: " + err;
        return false;
    }
    if (!validateSituationFamiliale(r.situationFamiliale, err)) {
        error = "Situation familiale: " + err;
        return false;
    }
    return true;
}

// ========== CRUD METHODS ==========

bool Resident::insert(int &outId, QString &errorText) const
{
    // Validation avant insertion
    QString validationError;
    if (!validateAll(*this, validationError)) {
        errorText = validationError;
        qDebug() << "❌ [DEBUG INSERT] Validation échouée:" << validationError;
        return false;
    }

    // Always use an explicit ID to avoid lastInsertId() (unsupported by Oracle ODBC)
    int useId = idResident;
    if (useId <= 0) {
        QSqlQuery qid;
        if (qid.exec("SELECT NVL(MAX(\"ID_RESIDENT\"),0)+1 FROM \"SYSTEM\".\"RESIDENTS\"") && qid.next()) {
            useId = qid.value(0).toInt();
        } else {
            errorText = qid.lastError().text();
            qDebug() << "❌ [DEBUG INSERT] Erreur lors de la génération de l'ID:" << errorText;
            return false;
        }
    }

    // Ensure unique id
    QSqlQuery qcheck;
    qcheck.prepare("SELECT 1 FROM \"SYSTEM\".\"RESIDENTS\" WHERE \"ID_RESIDENT\"=?");
    qcheck.addBindValue(useId);
    if (qcheck.exec() && qcheck.next()) {
        errorText = QString::fromLatin1("ID_RESIDENT %1 existe déjà").arg(useId);
        qDebug() << "❌ [DEBUG INSERT] ID existe déjà:" << useId;
        return false;
    }

    // Préparer les valeurs pour l'insertion
    QString nomVal = nom.left(20).trimmed();
    QString prenomVal = prenom.left(20).trimmed();
    QString adresseVal = adresse.left(20).trimmed();
    QString emailVal = email.left(30).trimmed();
    QString statutVal = statut.left(20).trimmed();
    QString situationVal = situationFamiliale.left(20).trimmed();
    
    qDebug() << "🔍 [DEBUG INSERT] Tentative d'insertion - ID:" << useId 
             << "| Nom:" << nomVal 
             << "| Prenom:" << prenomVal 
             << "| Date:" << dtNaissance.toString("yyyy-MM-dd")
             << "| Adresse:" << adresseVal
             << "| Tel:" << telephone
             << "| Email:" << emailVal
             << "| Statut:" << statutVal
             << "| Situation:" << situationVal;
    
    // Utiliser uniquement la requête SQL littérale (plus fiable avec Oracle ODBC)
    auto esc = [](const QString &s){ QString t=s; t.replace("'", "''"); return t; };
    QString dateStr = dtNaissance.toString("yyyy-MM-dd");
    QString literalSql = QString::fromLatin1(
        "INSERT INTO \"SYSTEM\".\"RESIDENTS\"(\"ID_RESIDENT\", \"NOM\", \"PRENOM\", \"DT_NAISSANCE\", \"ADRESSE\", \"TELEPHONE\", \"EMAIL\", \"STATUT\", \"SITUATION_FAMILILE\") "
        "VALUES(%1, '%2', '%3', DATE '%4', '%5', %6, '%7', '%8', '%9')")
        .arg(useId)
        .arg(esc(nomVal))
        .arg(esc(prenomVal))
        .arg(dateStr)
        .arg(esc(adresseVal))
        .arg(QString::number(telephone))
        .arg(esc(emailVal))
        .arg(esc(statutVal))
        .arg(esc(situationVal));
    
    // Exécuter UNE SEULE FOIS la requête INSERT
    QSqlQuery query;
    qDebug() << "🔍 [DEBUG INSERT] Requête SQL:" << literalSql;
    if (query.exec(literalSql)) {
        outId = useId;
        qDebug() << "✅ [DEBUG INSERT] Insertion réussie";
        return true;
    }
    
    // Si la requête échoue, retourner l'erreur
    const QSqlError err = query.lastError();
    qDebug() << "❌ [DEBUG INSERT] Erreur SQL:" << err.text() << "| native:" << err.nativeErrorCode();
    errorText = QString::fromLatin1("INSERT failed: %1 | native: %2 | driver: %3 | db: %4")
                .arg(err.text().isEmpty() ? "Unknown error" : err.text())
                .arg(err.nativeErrorCode().isEmpty() ? "N/A" : err.nativeErrorCode())
                .arg(err.driverText().isEmpty() ? "N/A" : err.driverText())
                .arg(err.databaseText().isEmpty() ? "N/A" : err.databaseText());
    return false;
}

bool Resident::updateById(int oldId, int newId, QString *errorText) const
{
    // Validation avant mise à jour
    QString validationError;
    if (!validateAll(*this, validationError)) {
        if (errorText) *errorText = validationError;
        qDebug() << "❌ [DEBUG UPDATE] Validation échouée:" << validationError;
        return false;
    }
    
    // Si newId > 0 et différent de oldId, vérifier qu'il n'existe pas déjà
    // (Cette vérification est déjà faite dans mainwindow.cpp, mais on la garde comme sécurité)
    if (newId > 0 && newId != oldId) {
        if (idExists(newId)) {
            QString errorMsg = QString("L'ID %1 existe déjà dans la base de données. Veuillez utiliser un autre ID.").arg(newId);
            if (errorText) *errorText = errorMsg;
            return false;
        }
    }
    
    QSqlQuery q;
    // Si newId > 0 et différent de oldId, on met à jour aussi l'ID
    if (newId > 0 && newId != oldId) {
        // Mettre à jour l'ID aussi
        q.prepare("UPDATE \"SYSTEM\".\"RESIDENTS\" SET \"ID_RESIDENT\"=?, \"NOM\"=?, \"PRENOM\"=?, \"DT_NAISSANCE\"=?, \"ADRESSE\"=?, \"TELEPHONE\"=?, \"EMAIL\"=?, \"STATUT\"=?, \"SITUATION_FAMILILE\"=? WHERE \"ID_RESIDENT\"=?");
        q.addBindValue(newId);
        q.addBindValue(nom.left(20).trimmed());
        q.addBindValue(prenom.left(20).trimmed());
        q.addBindValue(dtNaissance);
        q.addBindValue(adresse.left(20).trimmed());
        q.addBindValue(QVariant(static_cast<qlonglong>(telephone)));
        q.addBindValue(email.left(30).trimmed());
        q.addBindValue(statut.left(20).trimmed());
        q.addBindValue(situationFamiliale.left(20).trimmed());
        q.addBindValue(oldId);
    } else {
        // Garder l'ancien ID
        q.prepare("UPDATE \"SYSTEM\".\"RESIDENTS\" SET \"NOM\"=?, \"PRENOM\"=?, \"DT_NAISSANCE\"=?, \"ADRESSE\"=?, \"TELEPHONE\"=?, \"EMAIL\"=?, \"STATUT\"=?, \"SITUATION_FAMILILE\"=? WHERE \"ID_RESIDENT\"=?");
        q.addBindValue(nom.left(20).trimmed());
        q.addBindValue(prenom.left(20).trimmed());
        q.addBindValue(dtNaissance);
        q.addBindValue(adresse.left(20).trimmed());
        q.addBindValue(QVariant(static_cast<qlonglong>(telephone)));
        q.addBindValue(email.left(30).trimmed());
        q.addBindValue(statut.left(20).trimmed());
        q.addBindValue(situationFamiliale.left(20).trimmed());
        q.addBindValue(oldId);
    }
    
    qDebug() << "🔍 [DEBUG UPDATE] Tentative de mise à jour - OldID:" << oldId << "NewID:" << (newId > 0 && newId != oldId ? newId : oldId) << "Date:" << dtNaissance.toString("yyyy-MM-dd");

    if (!q.exec()) {
        const QSqlError err = q.lastError();
        qDebug() << "❌ [DEBUG UPDATE] Erreur SQL:" << err.text() << "| native:" << err.nativeErrorCode() << "| driver:" << err.driverText() << "| db:" << err.databaseText();
        qDebug() << "❌ [DEBUG UPDATE] Requête:" << q.lastQuery();
        QString errorMsg = QString::fromLatin1("UPDATE failed: %1 | native: %2 | driver: %3 | db: %4")
                    .arg(err.text().isEmpty() ? "Unknown error" : err.text())
                    .arg(err.nativeErrorCode().isEmpty() ? "N/A" : err.nativeErrorCode())
                    .arg(err.driverText().isEmpty() ? "N/A" : err.driverText())
                    .arg(err.databaseText().isEmpty() ? "N/A" : err.databaseText());
        if (errorText) *errorText = errorMsg;
        // Fallback literal UPDATE for ODBC edge cases
        auto esc = [](const QString &s){ QString t=s; t.replace("'", "''"); return t; };
        QString dateStr = dtNaissance.toString("yyyy-MM-dd");
        QString literalSql;
        if (newId > 0 && newId != oldId) {
            literalSql = QString::fromLatin1(
                "UPDATE \"SYSTEM\".\"RESIDENTS\" SET \"ID_RESIDENT\"=%1, \"NOM\"='%2', \"PRENOM\"='%3', \"DT_NAISSANCE\"=DATE '%4', \"ADRESSE\"='%5', \"TELEPHONE\"=%6, \"EMAIL\"='%7', \"STATUT\"='%8', \"SITUATION_FAMILILE\"='%9' WHERE \"ID_RESIDENT\"=%10")
                .arg(newId)
                .arg(esc(nom.left(20).trimmed()))
                .arg(esc(prenom.left(20).trimmed()))
                .arg(dateStr)
                .arg(esc(adresse.left(20).trimmed()))
                .arg(QString::number(telephone))
                .arg(esc(email.left(30).trimmed()))
                .arg(esc(statut.left(20).trimmed()))
                .arg(esc(situationFamiliale.left(20).trimmed()))
                .arg(oldId);
        } else {
            literalSql = QString::fromLatin1(
                "UPDATE \"SYSTEM\".\"RESIDENTS\" SET \"NOM\"='%1', \"PRENOM\"='%2', \"DT_NAISSANCE\"=DATE '%3', \"ADRESSE\"='%4', \"TELEPHONE\"=%5, \"EMAIL\"='%6', \"STATUT\"='%7', \"SITUATION_FAMILILE\"='%8' WHERE \"ID_RESIDENT\"=%9")
                .arg(esc(nom.left(20).trimmed()))
                .arg(esc(prenom.left(20).trimmed()))
                .arg(dateStr)
                .arg(esc(adresse.left(20).trimmed()))
                .arg(QString::number(telephone))
                .arg(esc(email.left(30).trimmed()))
                .arg(esc(statut.left(20).trimmed()))
                .arg(esc(situationFamiliale.left(20).trimmed()))
                .arg(oldId);
        }
        QSqlQuery q2;
        if (!q2.exec(literalSql)) {
            const QSqlError err2 = q2.lastError();
            errorMsg += QString::fromLatin1("\nLiteral UPDATE failed: %1 | native: %2 | driver: %3 | db: %4 | query: %5")
                         .arg(err2.text())
                         .arg(err2.nativeErrorCode())
                         .arg(err2.driverText())
                         .arg(err2.databaseText())
                         .arg(literalSql);
            if (errorText) *errorText = errorMsg;
            return false;
        }
        // Literal update succeeded
        if (errorText) errorText->clear();
        return true;
    }
    return q.numRowsAffected() > 0;
}

bool Resident::removeById(int id, QString &errorText)
{
    QSqlQuery q;
    q.prepare("DELETE FROM \"SYSTEM\".\"RESIDENTS\" WHERE \"ID_RESIDENT\"=?");
    q.addBindValue(id);
    if (!q.exec()) {
        const QSqlError err = q.lastError();
        errorText = QString::fromLatin1("DELETE failed: %1 | native: %2 | driver: %3 | db: %4 | query: %5")
                    .arg(err.text())
                    .arg(err.nativeErrorCode())
                    .arg(err.driverText())
                    .arg(err.databaseText())
                    .arg(q.lastQuery());
        return false;
    }
    return q.numRowsAffected() > 0;
}

QVector<Resident> Resident::fetchAll(QString &errorText)
{
    QVector<Resident> items;
    // Qualify with schema to avoid ambiguity across users
    QSqlQuery q;
    q.setForwardOnly(true);
    q.prepare("SELECT \"ID_RESIDENT\", \"NOM\", \"PRENOM\", \"DT_NAISSANCE\", \"ADRESSE\", \"TELEPHONE\", \"EMAIL\", \"STATUT\", \"SITUATION_FAMILILE\" FROM \"SYSTEM\".\"RESIDENTS\" ORDER BY \"ID_RESIDENT\" ASC");
    if (!q.exec()) { 
        errorText = q.lastError().text(); 
        qDebug() << "SELECT error:" << errorText;
        return items; 
    }
    int count = 0;
    while (q.next()) {
        Resident r;
        r.idResident = q.value(0).toInt();
        r.nom = q.value(1).toString();
        r.prenom = q.value(2).toString();
        r.dtNaissance = q.value(3).toDate();
        r.adresse = q.value(4).toString();
        r.telephone = q.value(5).toLongLong();
        r.email = q.value(6).toString();
        r.statut = q.value(7).toString();
        r.situationFamiliale = q.value(8).toString();
        items.push_back(r);
        count++;
    }
    qDebug() << "fetchAll: rows loaded =" << count;
    return items;
}

bool Resident::idExists(int id)
{
    QSqlQuery query;
    query.prepare("SELECT 1 FROM \"SYSTEM\".\"RESIDENTS\" WHERE \"ID_RESIDENT\" = :ID");
    query.bindValue(":ID", id);

    if (query.exec() && query.next()) {
        qDebug() << "⚠️ ID" << id << "existe déjà dans la base de données";
        return true;
    }

    return false;
}

