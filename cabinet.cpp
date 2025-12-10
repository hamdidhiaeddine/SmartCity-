#include "cabinet.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QFile>
#include <QTextStream>
#include <QMetaType>
#include <QStringConverter>
#include <QDebug>
#include <QObject>
#include <QStringList>
#include <QVariant>
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

Cabinet::Cabinet()
    : m_id(0), m_idResident(-1)
{
}

Cabinet::Cabinet(int id,
                 const QString &nom,
                 const QString &adresse,
                 const QString &specialite,
                 const QString &telephone,
                 const QString &email,
                 int idResident)
    : m_id(id)
    , m_nom(nom)
    , m_adresse(adresse)
    , m_specialite(specialite)
    , m_telephone(telephone)
    , m_email(email)
    , m_idResident(idResident)
{
}

int Cabinet::id() const { return m_id; }
QString Cabinet::nom() const { return m_nom; }
QString Cabinet::adresse() const { return m_adresse; }
QString Cabinet::specialite() const { return m_specialite; }
QString Cabinet::telephone() const { return m_telephone; }
QString Cabinet::email() const { return m_email; }
int Cabinet::idResident() const { return m_idResident; }

void Cabinet::setId(int value) { m_id = value; }
void Cabinet::setNom(const QString &value) { m_nom = value; }
void Cabinet::setAdresse(const QString &value) { m_adresse = value; }
void Cabinet::setSpecialite(const QString &value) { m_specialite = value; }
void Cabinet::setTelephone(const QString &value) { m_telephone = value; }
void Cabinet::setEmail(const QString &value) { m_email = value; }
void Cabinet::setIdResident(int value) { m_idResident = value; }

QSqlDatabase Cabinet::database()
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qWarning() << "Connexion BDD invalide pour Cabinet";
    }
    return db;
}

bool Cabinet::ajouter(QString *errorText) const
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO GEST_CABINET (ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL, ID_RESIDENT) "
                  "VALUES (:id, :nom, :adresse, :specialite, :telephone, :email, :idResident)");
    query.bindValue(":id", m_id);
    query.bindValue(":nom", m_nom.trimmed());
    query.bindValue(":adresse", m_adresse.trimmed());
    query.bindValue(":specialite", m_specialite.trimmed());
    query.bindValue(":telephone", m_telephone.trimmed());
    query.bindValue(":email", m_email.trimmed());
    if (m_idResident > 0) {
        query.bindValue(":idResident", m_idResident);
    } else {
        query.bindValue(":idResident", QVariant(QMetaType(QMetaType::Int)));
    }

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "INSERT GEST_CABINET" << query.lastError();
        return false;
    }

    return true;
}

bool Cabinet::modifier(int oldId, QString *errorText) const
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
    query.prepare("UPDATE GEST_CABINET SET ID = :newId, NOM = :nom, ADRESSE = :adresse, SPECIALITE = :specialite, "
                  "TELEPHONE = :telephone, EMAIL = :email, ID_RESIDENT = :idResident WHERE ID = :oldId");
    query.bindValue(":newId", m_id);
    query.bindValue(":nom", m_nom.trimmed());
    query.bindValue(":adresse", m_adresse.trimmed());
    query.bindValue(":specialite", m_specialite.trimmed());
    query.bindValue(":telephone", m_telephone.trimmed());
    query.bindValue(":email", m_email.trimmed());
    if (m_idResident > 0) {
        query.bindValue(":idResident", m_idResident);
    } else {
        query.bindValue(":idResident", QVariant(QMetaType(QMetaType::Int)));
    }
    query.bindValue(":oldId", oldId);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "UPDATE GEST_CABINET" << query.lastError();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Cabinet::supprimer(int id, QString *errorText)
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM GEST_CABINET WHERE ID = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "DELETE GEST_CABINET" << query.lastError();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Cabinet::idExists(int id)
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM GEST_CABINET WHERE ID = :id");
    query.bindValue(":id", id);
    return query.exec() && query.next();
}

QList<Cabinet> Cabinet::fetchAll(QString *errorText)
{
    QList<Cabinet> result;
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return result;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL, NVL(ID_RESIDENT, -1) FROM GEST_CABINET ORDER BY ID")) {
        if (errorText) *errorText = query.lastError().text();
        qWarning() << "SELECT GEST_CABINET" << query.lastError();
        return result;
    }

    while (query.next()) {
        Cabinet cabinet;
        cabinet.m_id = query.value(0).toInt();
        cabinet.m_nom = query.value(1).toString();
        cabinet.m_adresse = query.value(2).toString();
        cabinet.m_specialite = query.value(3).toString();
        cabinet.m_telephone = query.value(4).toString();
        cabinet.m_email = query.value(5).toString();
        cabinet.m_idResident = query.value(6).toInt();
        result.append(cabinet);
    }

    return result;
}

QSqlQueryModel *Cabinet::afficherCabinet() const
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL, ID_RESIDENT FROM GEST_CABINET ORDER BY ID",
                    database());
    return model;
}

QString Cabinet::resolveCritereColumn(const QString &critere)
{
    const QString lower = critere.toLower();
    if (lower.contains("nom")) return "NOM";
    if (lower.contains("adress")) return "ADRESSE";
    if (lower.contains("email")) return "EMAIL";
    if (lower.contains("sp")) return "SPECIALITE";
    if (lower.contains("tel")) return "TELEPHONE";
    if (lower.contains("resident")) return "ID_RESIDENT";
    return "NOM";
}

QSqlQueryModel *Cabinet::rechercherCabinet(const QString &critere, const QString &valeur) const
{
    const QString column = resolveCritereColumn(critere);
    QSqlQueryModel *model = new QSqlQueryModel();

    QSqlQuery query(database());
    query.prepare(QStringLiteral("SELECT ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL, ID_RESIDENT "
                                 "FROM GEST_CABINET WHERE %1 LIKE :value ORDER BY ID").arg(column));
    const QString pattern = QStringLiteral("%%1%").arg(valeur.trimmed());
    query.bindValue(":value", pattern);

    if (!query.exec()) {
        qWarning() << "SEARCH GEST_CABINET" << query.lastError();
        delete model;
        return nullptr;
    }

    model->setQuery(std::move(query));
    return model;
}

QSqlQueryModel *Cabinet::getStatSpecialite() const
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT NVL(SPECIALITE, 'Inconnue') AS SPECIALITE, COUNT(*) AS TOTAL "
                    "FROM GEST_CABINET GROUP BY NVL(SPECIALITE, 'Inconnue') ORDER BY TOTAL DESC",
                    database());
    return model;
}

bool Cabinet::exporterExcel(const QString &filePath, QSqlQueryModel *model) const
{
    if (!model) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "EXPORT CABINET" << file.errorString();
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    const int colCount = model->columnCount();
    QStringList header;
    for (int c = 0; c < colCount; ++c) {
        header << model->headerData(c, Qt::Horizontal).toString();
    }
    stream << header.join(';') << '\n';

    const int rowCount = model->rowCount();
    for (int r = 0; r < rowCount; ++r) {
        QStringList rowValues;
        for (int c = 0; c < colCount; ++c) {
            QString value = model->data(model->index(r, c)).toString();
            if (value.contains(';')) {
                value.replace(';', ',');
            }
            rowValues << value;
        }
        stream << rowValues.join(';') << '\n';
    }

    file.close();
    return true;
}