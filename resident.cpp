#include "resident.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

namespace {
const char *kTableName = "GEST_RESIDENT";
}

Resident::Resident(const QString &id,
                   const QString &nom,
                   const QString &prenom,
                   const QDate &dateNaissance,
                   const QString &adresse,
                   const QString &telephone,
                   const QString &email,
                   const QString &statut,
                   const QString &situation)
    : m_id(id),
      m_nom(nom),
      m_prenom(prenom),
      m_dateNaissance(dateNaissance),
      m_adresse(adresse),
      m_telephone(telephone),
      m_email(email),
      m_statut(statut),
      m_situation(situation)
{
}

QString Resident::id() const { return m_id; }
QString Resident::nom() const { return m_nom; }
QString Resident::prenom() const { return m_prenom; }
QString Resident::nomComplet() const { return QString("%1 %2").arg(m_nom, m_prenom).simplified(); }
QDate Resident::dateNaissance() const { return m_dateNaissance; }
QString Resident::adresse() const { return m_adresse; }
QString Resident::telephone() const { return m_telephone; }
QString Resident::email() const { return m_email; }
QString Resident::statut() const { return m_statut; }
QString Resident::situation() const { return m_situation; }

bool Resident::ajouter(const Resident &resident)
{
    QSqlQuery query;
    query.prepare(QStringLiteral(
        "INSERT INTO %1 (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE) "
        "VALUES (:id, :nom, :prenom, :datenaiss, :adresse, :telephone, :email, :statut, :situation)")
                      .arg(kTableName));
    query.bindValue(":id", resident.id());
    query.bindValue(":nom", resident.nom());
    query.bindValue(":prenom", resident.prenom());
    query.bindValue(":datenaiss", resident.dateNaissance());
    query.bindValue(":adresse", resident.adresse());
    query.bindValue(":telephone", resident.telephone());
    query.bindValue(":email", resident.email());
    query.bindValue(":statut", resident.statut());
    query.bindValue(":situation", resident.situation());

    if (!query.exec()) {
        qWarning() << "Resident::ajouter - erreur SQL:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Resident::modifier(const Resident &resident)
{
    QSqlQuery query;
    query.prepare(QStringLiteral(
        "UPDATE %1 SET NOM = :nom, PRENOM = :prenom, DATENAISS = :datenaiss, ADRESSE = :adresse, "
        "TELEPHONE = :telephone, EMAIL = :email, STATUT = :statut, SITUATIONFAMILIALE = :situation "
        "WHERE ID = :id")
                      .arg(kTableName));
    query.bindValue(":nom", resident.nom());
    query.bindValue(":prenom", resident.prenom());
    query.bindValue(":datenaiss", resident.dateNaissance());
    query.bindValue(":adresse", resident.adresse());
    query.bindValue(":telephone", resident.telephone());
    query.bindValue(":email", resident.email());
    query.bindValue(":statut", resident.statut());
    query.bindValue(":situation", resident.situation());
    query.bindValue(":id", resident.id());

    if (!query.exec()) {
        qWarning() << "Resident::modifier - erreur SQL:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool Resident::supprimer(const QString &id)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("DELETE FROM %1 WHERE ID = :id").arg(kTableName));
    query.bindValue(":id", id);
    if (!query.exec()) {
        qWarning() << "Resident::supprimer - erreur SQL:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool Resident::existe(const QString &id)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT 1 FROM %1 WHERE ID = :id").arg(kTableName));
    query.bindValue(":id", id);
    if (!query.exec()) {
        qWarning() << "Resident::existe - erreur SQL:" << query.lastError().text();
        return false;
    }
    return query.next();
}

QList<Resident> Resident::recupererTout()
{
    QList<Resident> residents;
    QSqlQuery query(QStringLiteral(
        "SELECT ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE "
        "FROM %1 ORDER BY ID")
                        .arg(kTableName));

    while (query.next()) {
        const QString id = query.value(0).toString();
        const QString nom = query.value(1).toString();
        const QString prenom = query.value(2).toString();

        QDate date = query.value(3).toDate();
        if (!date.isValid()) {
            date = QDate::fromString(query.value(3).toString(), Qt::ISODate);
        }

        const QString adresse = query.value(4).toString();
        const QString telephone = query.value(5).toString();
        const QString email = query.value(6).toString();
        const QString statut = query.value(7).toString();
        const QString situation = query.value(8).toString();

        residents.append(Resident(id, nom, prenom, date, adresse, telephone, email, statut, situation));
    }

    if (query.lastError().isValid()) {
        qWarning() << "Resident::recupererTout - erreur SQL:" << query.lastError().text();
    }

    return residents;
}

