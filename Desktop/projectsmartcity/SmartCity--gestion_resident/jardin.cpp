#include "jardin.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDate>
#include <QDebug>
#include <QObject>

namespace
{
    QSqlDatabase resolveDb()
    {
        if (QSqlDatabase::contains("qt_oracle")) {
            return QSqlDatabase::database("qt_oracle");
        }
        return QSqlDatabase::database();
    }
}

Jardin::Jardin()
    : m_id(0), m_superficie(0.0)
{
}

Jardin::Jardin(int id,
               const QString &emplacement,
               double superficie,
               const QString &typeSol,
               const QString &typeChoix)
    : m_id(id)
    , m_emplacement(emplacement)
    , m_superficie(superficie)
    , m_typeSol(typeSol)
    , m_typeChoix(typeChoix)
{
}

int Jardin::id() const { return m_id; }
QString Jardin::emplacement() const { return m_emplacement; }
double Jardin::superficie() const { return m_superficie; }
QString Jardin::typeSol() const { return m_typeSol; }
QString Jardin::typeChoix() const { return m_typeChoix; }

void Jardin::setId(int value) { m_id = value; }
void Jardin::setEmplacement(const QString &value) { m_emplacement = value; }
void Jardin::setSuperficie(double value) { m_superficie = value; }
void Jardin::setTypeSol(const QString &value) { m_typeSol = value; }
void Jardin::setTypeChoix(const QString &value) { m_typeChoix = value; }

QSqlDatabase Jardin::database()
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qWarning() << "Connexion BDD invalide pour Jardin";
    }
    return db;
}

bool Jardin::ajouter(QString *errorText) const
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TYPE_CHOIX) "
                  "VALUES (:id, :emplacement, :superficie, :typeSol, :typeChoix)");
    query.bindValue(":id", m_id);
    query.bindValue(":emplacement", m_emplacement.trimmed());
    query.bindValue(":superficie", m_superficie);
    query.bindValue(":typeSol", m_typeSol.trimmed());
    query.bindValue(":typeChoix", m_typeChoix.trimmed());

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "INSERT GEST_JARDIN" << query.lastError();
        return false;
    }

    return true;
}

bool Jardin::modifier(int oldId, QString *errorText) const
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return false;
    }

    if (m_id != oldId && idExists(m_id)) {
        if (errorText) *errorText = QObject::tr("L'ID %1 existe déjà").arg(m_id);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE GEST_JARDIN SET ID_JARDIN = :newId, EMPLACEMENT = :emplacement, "
                  "SUPERFICIE = :superficie, TYPE_SOL = :typeSol, TYPE_CHOIX = :typeChoix WHERE ID_JARDIN = :oldId");
    query.bindValue(":newId", m_id);
    query.bindValue(":emplacement", m_emplacement.trimmed());
    query.bindValue(":superficie", m_superficie);
    query.bindValue(":typeSol", m_typeSol.trimmed());
    query.bindValue(":typeChoix", m_typeChoix.trimmed());
    query.bindValue(":oldId", oldId);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "UPDATE GEST_JARDIN" << query.lastError();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Jardin::supprimer(int id, QString *errorText)
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM GEST_JARDIN WHERE ID_JARDIN = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "DELETE GEST_JARDIN" << query.lastError();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Jardin::idExists(int id)
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM GEST_JARDIN WHERE ID_JARDIN = :id");
    query.bindValue(":id", id);
    return query.exec() && query.next();
}

QList<Jardin> Jardin::fetchAll(QString *errorText)
{
    QList<Jardin> result;
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return result;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TYPE_CHOIX "
                    "FROM GEST_JARDIN ORDER BY ID_JARDIN")) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "SELECT GEST_JARDIN" << query.lastError();
        return result;
    }

    while (query.next()) {
        Jardin jardin;
        jardin.m_id = query.value(0).toInt();
        jardin.m_emplacement = query.value(1).toString();
        jardin.m_superficie = query.value(2).toDouble();
        jardin.m_typeSol = query.value(3).toString();
        jardin.m_typeChoix = query.value(4).toString();
        result.append(jardin);
    }

    return result;
}

QList<Jardin> Jardin::fetchAllSortedByType(QString *errorText)
{
    QList<Jardin> result;
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return result;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TYPE_CHOIX "
                    "FROM GEST_JARDIN ORDER BY TYPE_CHOIX, ID_JARDIN")) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "SELECT GEST_JARDIN (tri par type)" << query.lastError();
        return result;
    }

    while (query.next()) {
        Jardin jardin;
        jardin.m_id = query.value(0).toInt();
        jardin.m_emplacement = query.value(1).toString();
        jardin.m_superficie = query.value(2).toDouble();
        jardin.m_typeSol = query.value(3).toString();
        jardin.m_typeChoix = query.value(4).toString();
        result.append(jardin);
    }

    return result;
}

QList<Jardin> Jardin::fetchAllSortedById(QString *errorText)
{
    QList<Jardin> result;
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return result;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TYPE_CHOIX "
                    "FROM GEST_JARDIN ORDER BY ID_JARDIN ASC")) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "SELECT GEST_JARDIN (tri par ID)" << query.lastError();
        return result;
    }

    while (query.next()) {
        Jardin jardin;
        jardin.m_id = query.value(0).toInt();
        jardin.m_emplacement = query.value(1).toString();
        jardin.m_superficie = query.value(2).toDouble();
        jardin.m_typeSol = query.value(3).toString();
        jardin.m_typeChoix = query.value(4).toString();
        result.append(jardin);
    }

    return result;
}

QList<Jardin> Jardin::fetchAllSortedBySuperficie(QString *errorText)
{
    QList<Jardin> result;
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return result;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TYPE_CHOIX "
                    "FROM GEST_JARDIN ORDER BY SUPERFICIE DESC")) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "SELECT GEST_JARDIN (tri par superficie)" << query.lastError();
        return result;
    }

    while (query.next()) {
        Jardin jardin;
        jardin.m_id = query.value(0).toInt();
        jardin.m_emplacement = query.value(1).toString();
        jardin.m_superficie = query.value(2).toDouble();
        jardin.m_typeSol = query.value(3).toString();
        jardin.m_typeChoix = query.value(4).toString();
        result.append(jardin);
    }

    return result;
}
