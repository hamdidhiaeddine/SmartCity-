#include "Employee.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMap>
#include <QDebug>

/*static void tryDisableFk()
{
    QSqlQuery q;
    q.exec("ALTER TABLE EMPLOYES DISABLE CONSTRAINT FK_EM");
}

static void tryEnableFk()
{
    QSqlQuery q;
    q.exec("ALTER TABLE EMPLOYES ENABLE CONSTRAINT FK_EM");
}
*/
bool Employee::insert(int &outId, QString &errorText) const
{
    QSqlQuery q;

    // Always use an explicit ID to avoid lastInsertId() (unsupported by Oracle ODBC)
    int useId = idEmploye;
    if (useId <= 0) {
        QSqlQuery qid;
        if (qid.exec("SELECT NVL(MAX(\"ID_EMPLOYE\"),0)+1 FROM \"SYSTEM\".\"EMPLOYES\"") && qid.next()) {
            useId = qid.value(0).toInt();
        } else {
            errorText = qid.lastError().text();
            return false;
        }
    }

    // Ensure unique id
    QSqlQuery qcheck;
    qcheck.prepare("SELECT 1 FROM \"SYSTEM\".\"EMPLOYES\" WHERE \"ID_EMPLOYE\"=?");
    qcheck.addBindValue(useId);
    if (qcheck.exec() && qcheck.next()) {
        errorText = QString::fromLatin1("ID_EMPLOYE %1 existe déjà").arg(useId);
        return false;
    }

    // Insert without ID_RES to avoid binding NULL issues on some ODBC drivers
    q.prepare("INSERT INTO \"SYSTEM\".\"EMPLOYES\"(\"ID_EMPLOYE\", \"NOM\", \"PRENOM\", \"POSTE\", \"SALAIRE\", \"ADRESSE\", \"TELEPHONE\", \"EMAIL\") VALUES(?,?,?,?,?,?,?,?)");
    q.addBindValue(useId);                                  // NUMBER
    q.addBindValue(nom.left(20).trimmed());                 // VARCHAR2(20)
    q.addBindValue(prenom.left(20).trimmed());              // VARCHAR2(20)
    q.addBindValue(poste.left(20).trimmed());               // VARCHAR2(20)
    q.addBindValue(QVariant(salaire));                      // NUMBER
    q.addBindValue(adresse.left(20).trimmed());             // VARCHAR2(20)
    q.addBindValue(QVariant(static_cast<qlonglong>(telephone))); // NUMBER
    q.addBindValue(email.left(20).trimmed());                    // VARCHAR2(20)
    // no ID_RES in insert; defaults to NULL
    if (!q.exec()) {
        const QSqlError err = q.lastError();
        errorText = QString::fromLatin1("INSERT failed: %1 | native: %2 | driver: %3 | db: %4 | query: %5")
                    .arg(err.text())
                    .arg(err.nativeErrorCode())
                    .arg(err.driverText())
                    .arg(err.databaseText())
                    .arg(q.lastQuery());
        // Fallback: try literal SQL to bypass problematic ODBC bindings (for debugging)
        auto esc = [](const QString &s){ QString t=s; t.replace("'", "''"); return t; };
        QString literalSql = QString::fromLatin1(
            "INSERT INTO \"SYSTEM\".\"EMPLOYES\"(\"ID_EMPLOYE\", \"NOM\", \"PRENOM\", \"POSTE\", \"SALAIRE\", \"ADRESSE\", \"TELEPHONE\", \"EMAIL\") "
            "VALUES(%1, '%2', '%3', '%4', %5, '%6', %7, '%8')")
            .arg(useId)
            .arg(esc(nom.left(20).trimmed()))
            .arg(esc(prenom.left(20).trimmed()))
            .arg(esc(poste.left(20).trimmed()))
            .arg(QString::number(salaire, 'f', 2))
            .arg(esc(adresse.left(20).trimmed()))
            .arg(QString::number(telephone))
            .arg(esc(email.left(20).trimmed()));
        QSqlQuery q2;
        if (!q2.exec(literalSql)) {
            const QSqlError err2 = q2.lastError();
            errorText += QString::fromLatin1("\nLiteral INSERT failed: %1 | native: %2 | driver: %3 | db: %4 | query: %5")
                         .arg(err2.text())
                         .arg(err2.nativeErrorCode())
                         .arg(err2.driverText())
                         .arg(err2.databaseText())
                         .arg(literalSql);
            return false;
        }
    }
    outId = useId;
    return true;
}

bool Employee::updateById(int id, QString &errorText) const
{
    QSqlQuery q;
    // Update without touching ID_RES (stays NULL by default)
    q.prepare("UPDATE \"SYSTEM\".\"EMPLOYES\" SET \"NOM\"=?, \"PRENOM\"=?, \"POSTE\"=?, \"SALAIRE\"=?, \"ADRESSE\"=?, \"TELEPHONE\"=?, \"EMAIL\"=? WHERE \"ID_EMPLOYE\"=?");
    q.addBindValue(nom.left(20).trimmed());
    q.addBindValue(prenom.left(20).trimmed());
    q.addBindValue(poste.left(20).trimmed());
    q.addBindValue(QVariant(salaire));
    q.addBindValue(adresse.left(20).trimmed());
    q.addBindValue(QVariant(static_cast<qlonglong>(telephone)));
    q.addBindValue(email.left(20).trimmed());
    q.addBindValue(id);
    if (!q.exec()) {
        const QSqlError err = q.lastError();
        errorText = QString::fromLatin1("UPDATE failed: %1 | native: %2 | driver: %3 | db: %4 | query: %5")
                    .arg(err.text())
                    .arg(err.nativeErrorCode())
                    .arg(err.driverText())
                    .arg(err.databaseText())
                    .arg(q.lastQuery());
        // Fallback literal UPDATE for ODBC edge cases
        auto esc = [](const QString &s){ QString t=s; t.replace("'", "''"); return t; };
        QString literalSql = QString::fromLatin1(
            "UPDATE \"SYSTEM\".\"EMPLOYES\" SET \"NOM\"='%1', \"PRENOM\"='%2', \"POSTE\"='%3', \"SALAIRE\"=%4, \"ADRESSE\"='%5', \"TELEPHONE\"=%6, \"EMAIL\"='%7' WHERE \"ID_EMPLOYE\"=%8")
            .arg(esc(nom.left(20).trimmed()))
            .arg(esc(prenom.left(20).trimmed()))
            .arg(esc(poste.left(20).trimmed()))
            .arg(QString::number(salaire, 'f', 2))
            .arg(esc(adresse.left(20).trimmed()))
            .arg(QString::number(telephone))
            .arg(esc(email.left(20).trimmed()))
            .arg(id);
        QSqlQuery q2;
        if (!q2.exec(literalSql)) {
            const QSqlError err2 = q2.lastError();
            errorText += QString::fromLatin1("\nLiteral UPDATE failed: %1 | native: %2 | driver: %3 | db: %4 | query: %5")
                         .arg(err2.text())
                         .arg(err2.nativeErrorCode())
                         .arg(err2.driverText())
                         .arg(err2.databaseText())
                         .arg(literalSql);
            return false;
        }
        // Literal update succeeded
        errorText.clear();
        return true;
    }
    return q.numRowsAffected() > 0;
}

bool Employee::removeById(int id, QString &errorText)
{
    QSqlQuery q;
    q.prepare("DELETE FROM \"SYSTEM\".\"EMPLOYES\" WHERE \"ID_EMPLOYE\"=?");
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

QVector<Employee> Employee::fetchAll(QString &errorText)
{
    QVector<Employee> items;
    // Qualify with schema to avoid ambiguity across users
    QSqlQuery q;
    q.setForwardOnly(true);
    q.prepare("SELECT \"ID_EMPLOYE\", \"NOM\", \"PRENOM\", \"EMAIL\", \"POSTE\", \"SALAIRE\", \"ADRESSE\", \"TELEPHONE\" FROM \"SYSTEM\".\"EMPLOYES\" ORDER BY \"ID_EMPLOYE\" ASC");
    if (!q.exec()) { 
        errorText = q.lastError().text(); 
        qDebug() << "SELECT error:" << errorText;
        return items; 
    }
    int count = 0;
    while (q.next()) {
        Employee e;
        e.idEmploye = q.value(0).toInt();
        e.nom = q.value(1).toString();
        e.prenom = q.value(2).toString();
        e.email = q.value(3).toString();
        e.poste = q.value(4).toString();
        e.salaire = q.value(5).toDouble();
        e.adresse = q.value(6).toString();
        e.telephone = q.value(7).toLongLong();
        items.push_back(e);
        count++;
    }
    qDebug() << "fetchAll: rows loaded =" << count;
    return items;
}

QMap<QString, int> Employee::getStatistiquesParPoste(QString &errorText)
{
    QMap<QString, int> stats;
    QSqlQuery q;
    q.setForwardOnly(true);
    
    // Requête SQL pour compter les employés par poste
    q.prepare("SELECT \"POSTE\", COUNT(*) FROM \"SYSTEM\".\"EMPLOYES\" GROUP BY \"POSTE\" ORDER BY \"POSTE\"");
    
    if (!q.exec()) {
        errorText = q.lastError().text();
        qDebug() << "Erreur statistiques par poste:" << errorText;
        return stats;
    }
    
    while (q.next()) {
        QString poste = q.value(0).toString();
        int count = q.value(1).toInt();
        stats[poste] = count;
        qDebug() << "Poste:" << poste << "-> Nombre:" << count;
    }
    
    return stats;
}


