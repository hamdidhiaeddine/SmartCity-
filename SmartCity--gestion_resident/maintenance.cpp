#include "maintenance.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QMetaType>
#include <utility>

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

Maintenance::Maintenance()
    : m_id(-1)
    , m_idJardin(-1)
    , m_typeMaintenance()
    , m_description()
    , m_dateEffectuee()
    , m_dateRecommandee()
    , m_statut()
    , m_frequence()
{
}

Maintenance::Maintenance(int id,
                         int idJardin,
                         const QString &typeMaintenance,
                         const QString &description,
                         const QDate &dateEffectuee,
                         const QDate &dateRecommandee,
                         const QString &statut,
                         const QString &frequence)
    : m_id(id)
    , m_idJardin(idJardin)
    , m_typeMaintenance(typeMaintenance)
    , m_description(description)
    , m_dateEffectuee(dateEffectuee)
    , m_dateRecommandee(dateRecommandee)
    , m_statut(statut)
    , m_frequence(frequence)
{
}

int Maintenance::id() const { return m_id; }
int Maintenance::idJardin() const { return m_idJardin; }
QString Maintenance::typeMaintenance() const { return m_typeMaintenance; }
QString Maintenance::description() const { return m_description; }
QDate Maintenance::dateEffectuee() const { return m_dateEffectuee; }
QDate Maintenance::dateRecommandee() const { return m_dateRecommandee; }
QString Maintenance::statut() const { return m_statut; }
QString Maintenance::frequence() const { return m_frequence; }

void Maintenance::setId(int value) { m_id = value; }
void Maintenance::setIdJardin(int value) { m_idJardin = value; }
void Maintenance::setTypeMaintenance(const QString &value) { m_typeMaintenance = value; }
void Maintenance::setDescription(const QString &value) { m_description = value; }
void Maintenance::setDateEffectuee(const QDate &value) { m_dateEffectuee = value; }
void Maintenance::setDateRecommandee(const QDate &value) { m_dateRecommandee = value; }
void Maintenance::setStatut(const QString &value) { m_statut = value; }
void Maintenance::setFrequence(const QString &value) { m_frequence = value; }

QSqlDatabase Maintenance::database()
{
    return resolveDb();
}

bool Maintenance::ajouter(QString *errorText) const
{
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO MAINTENANCE (ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, "
        "DATE_EFFECTUEE, DATE_RECOMMANDEE, STATUT, FREQUENCE) "
        "VALUES (:id, :idJardin, :typeMaint, :desc, :dateEff, :dateRec, :statut, :freq)"));
    query.bindValue(QStringLiteral(":id"), m_id);
    query.bindValue(QStringLiteral(":idJardin"), m_idJardin);
    query.bindValue(QStringLiteral(":typeMaint"), m_typeMaintenance);
    query.bindValue(QStringLiteral(":desc"), m_description);
    query.bindValue(QStringLiteral(":dateEff"), m_dateEffectuee.isValid() ? m_dateEffectuee : QVariant(QMetaType(QMetaType::QDate)));
    query.bindValue(QStringLiteral(":dateRec"), m_dateRecommandee.isValid() ? m_dateRecommandee : QVariant(QMetaType(QMetaType::QDate)));
    query.bindValue(QStringLiteral(":statut"), m_statut);
    query.bindValue(QStringLiteral(":freq"), m_frequence);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return false;
    }
    return true;
}

bool Maintenance::modifier(int oldId, QString *errorText) const
{
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "UPDATE MAINTENANCE SET ID_MAINTENANCE = :newId, ID_JARDIN = :idJardin, "
        "TYPE_MAINTENANCE = :typeMaint, DESCRIPTION = :desc, DATE_EFFECTUEE = :dateEff, "
        "DATE_RECOMMANDEE = :dateRec, STATUT = :statut, FREQUENCE = :freq "
        "WHERE ID_MAINTENANCE = :oldId"));
    query.bindValue(QStringLiteral(":newId"), m_id);
    query.bindValue(QStringLiteral(":idJardin"), m_idJardin);
    query.bindValue(QStringLiteral(":typeMaint"), m_typeMaintenance);
    query.bindValue(QStringLiteral(":desc"), m_description);
    query.bindValue(QStringLiteral(":dateEff"), m_dateEffectuee.isValid() ? m_dateEffectuee : QVariant(QMetaType(QMetaType::QDate)));
    query.bindValue(QStringLiteral(":dateRec"), m_dateRecommandee.isValid() ? m_dateRecommandee : QVariant(QMetaType(QMetaType::QDate)));
    query.bindValue(QStringLiteral(":statut"), m_statut);
    query.bindValue(QStringLiteral(":freq"), m_frequence);
    query.bindValue(QStringLiteral(":oldId"), oldId);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return false;
    }
    return true;
}

bool Maintenance::supprimer(int id, QString *errorText)
{
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM MAINTENANCE WHERE ID_MAINTENANCE = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return false;
    }
    return true;
}

bool Maintenance::idExists(int id)
{
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM MAINTENANCE WHERE ID_MAINTENANCE = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

QList<Maintenance> Maintenance::fetchAll(QString *errorText)
{
    QList<Maintenance> list;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return list;
    }

    QSqlQuery query(std::move(db));
    if (!query.exec(QStringLiteral(
        "SELECT ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, "
        "DATE_EFFECTUEE, DATE_RECOMMANDEE, STATUT, FREQUENCE FROM MAINTENANCE ORDER BY DATE_RECOMMANDEE"))) {
        if (errorText) *errorText = query.lastError().text();
        return list;
    }

    while (query.next()) {
        Maintenance m(
            query.value(0).toInt(),
            query.value(1).toInt(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toDate(),
            query.value(5).toDate(),
            query.value(6).toString(),
            query.value(7).toString()
        );
        list.append(m);
    }
    return list;
}

QList<Maintenance> Maintenance::fetchByJardin(int idJardin, QString *errorText)
{
    QList<Maintenance> list;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return list;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, "
        "DATE_EFFECTUEE, DATE_RECOMMANDEE, STATUT, FREQUENCE FROM MAINTENANCE "
        "WHERE ID_JARDIN = :idJardin ORDER BY DATE_RECOMMANDEE"));
    query.bindValue(QStringLiteral(":idJardin"), idJardin);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return list;
    }

    while (query.next()) {
        Maintenance m(
            query.value(0).toInt(),
            query.value(1).toInt(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toDate(),
            query.value(5).toDate(),
            query.value(6).toString(),
            query.value(7).toString()
        );
        list.append(m);
    }
    return list;
}

QList<Maintenance> Maintenance::fetchMaintenancePredictive(QString *errorText)
{
    QList<Maintenance> list;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return list;
    }

    QSqlQuery query(std::move(db));
    // Récupère les maintenances prédictives (statut = 'Prévu' ou date future)
    if (!query.exec(QStringLiteral(
        "SELECT ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, "
        "DATE_EFFECTUEE, DATE_RECOMMANDEE, STATUT, FREQUENCE FROM MAINTENANCE "
        "WHERE STATUT = 'Prévu' OR DATE_RECOMMANDEE >= SYSDATE ORDER BY DATE_RECOMMANDEE"))) {
        if (errorText) *errorText = query.lastError().text();
        return list;
    }

    while (query.next()) {
        Maintenance m(
            query.value(0).toInt(),
            query.value(1).toInt(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toDate(),
            query.value(5).toDate(),
            query.value(6).toString(),
            query.value(7).toString()
        );
        list.append(m);
    }
    return list;
}

QList<Maintenance> Maintenance::fetchMaintenancesAVenir(int joursFutur, QString *errorText)
{
    QList<Maintenance> list;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return list;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, "
        "DATE_EFFECTUEE, DATE_RECOMMANDEE, STATUT, FREQUENCE FROM MAINTENANCE "
        "WHERE DATE_RECOMMANDEE BETWEEN SYSDATE AND SYSDATE + :jours ORDER BY DATE_RECOMMANDEE"));
    query.bindValue(QStringLiteral(":jours"), joursFutur);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return list;
    }

    while (query.next()) {
        Maintenance m(
            query.value(0).toInt(),
            query.value(1).toInt(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toDate(),
            query.value(5).toDate(),
            query.value(6).toString(),
            query.value(7).toString()
        );
        list.append(m);
    }
    return list;
}

int Maintenance::getNextId()
{
    QSqlDatabase db = database();
    if (!db.isOpen()) return 1;
    
    QSqlQuery query(db);
    if (query.exec("SELECT NVL(MAX(ID_MAINTENANCE), 0) + 1 FROM MAINTENANCE") && query.next()) {
        return query.value(0).toInt();
    }
    return 1;
}

QStringList Maintenance::typesMaintenanceDisponibles()
{
    return QStringList{"Arrosage", "Taille", "Fertilisation", "Désherbage", 
                       "Traitement", "Plantation", "Nettoyage", "Autre"};
}

QStringList Maintenance::statutsDisponibles()
{
    return QStringList{"Prévu", "En cours", "Terminé", "Annulé", "Reporté"};
}

QStringList Maintenance::frequencesDisponibles()
{
    return QStringList{"Quotidien", "Hebdomadaire", "Mensuel", "Trimestriel", "Annuel", "Ponctuel"};
}
