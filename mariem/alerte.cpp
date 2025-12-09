#include "alerte.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>

/* ----- CONSTRUCTEURS ----- */
Alerte::Alerte(QObject *parent)
    : QObject(parent), id(0), idMaison(0), niveau(0), dateAlerte(QDate::currentDate())
{
}

Alerte::Alerte(int idMaison, const QString &zone, int niveau, const QString &statut)
    : id(0), idMaison(idMaison), zone(zone), niveau(niveau), statut(statut), dateAlerte(QDate::currentDate())
{
}

Alerte::Alerte(int idMaison, const QString &zone, int niveau, const QString &statut, const QDate &dateAlerte)
    : id(0), idMaison(idMaison), zone(zone), niveau(niveau), statut(statut), dateAlerte(dateAlerte)
{
}

/* ----- GETTERS ----- */
int Alerte::getId() const { return id; }
int Alerte::getIdMaison() const { return idMaison; }
QString Alerte::getZone() const { return zone; }
int Alerte::getNiveau() const { return niveau; }
QString Alerte::getStatut() const { return statut; }
QDate Alerte::getDateAlerte() const { return dateAlerte; }

/* ----- SETTERS ----- */
void Alerte::setId(int id) { this->id = id; }
void Alerte::setIdMaison(int idMaison) { this->idMaison = idMaison; }
void Alerte::setZone(const QString &zone) { this->zone = zone; }
void Alerte::setNiveau(int niveau) { this->niveau = niveau; }
void Alerte::setStatut(const QString &statut) { this->statut = statut; }
void Alerte::setDateAlerte(const QDate &dateAlerte) { this->dateAlerte = dateAlerte; }

/* ============================
 *           CRUD
 * ============================ */

bool Alerte::ajouter()
{
    QSqlQuery query(QSqlDatabase::database("qt_oracle"));

    // Pour Oracle avec IDENTITY, insérer les données
    query.prepare("INSERT INTO GEST_ALERTES (ID_MAISON, ZONE, NIVEAU, STATUT, DATE_ALERTE) "
                  "VALUES (:id_maison, :zone, :niveau, :statut, :date_alerte)");

    query.bindValue(":id_maison", idMaison);
    query.bindValue(":zone", zone);
    query.bindValue(":niveau", niveau);
    query.bindValue(":statut", statut);
    query.bindValue(":date_alerte", dateAlerte);

    if (!query.exec()) {
        qDebug() << "Erreur ajout alerte:" << query.lastError().text();
        return false;
    }
    
    // Récupérer l'ID généré (Oracle IDENTITY crée automatiquement l'ID)
    // Utiliser MAX(ID) pour récupérer le dernier ID inséré
    QSqlQuery queryId(QSqlDatabase::database("qt_oracle"));
    queryId.prepare("SELECT MAX(ID) FROM GEST_ALERTES WHERE ID_MAISON = :id_maison AND ZONE = :zone");
    queryId.bindValue(":id_maison", idMaison);
    queryId.bindValue(":zone", zone);
    
    if (queryId.exec() && queryId.next()) {
        id = queryId.value(0).toInt();
    }
    
    return true;
}

bool Alerte::modifier(int id)
{
    QSqlQuery query(QSqlDatabase::database("qt_oracle"));

    query.prepare("UPDATE GEST_ALERTES "
                  "SET ID_MAISON = :id_maison, ZONE = :zone, NIVEAU = :niveau, "
                  "STATUT = :statut, DATE_ALERTE = :date_alerte "
                  "WHERE ID = :id");

    query.bindValue(":id", id);
    query.bindValue(":id_maison", idMaison);
    query.bindValue(":zone", zone);
    query.bindValue(":niveau", niveau);
    query.bindValue(":statut", statut);
    query.bindValue(":date_alerte", dateAlerte);

    if (!query.exec()) {
        qDebug() << "Erreur modification alerte:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Alerte::supprimer(int id)
{
    QSqlQuery query(QSqlDatabase::database("qt_oracle"));

    query.prepare("DELETE FROM GEST_ALERTES WHERE ID = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression alerte:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Alerte::rechercher(int id)
{
    QSqlQuery query(QSqlDatabase::database("qt_oracle"));

    query.prepare("SELECT * FROM GEST_ALERTES WHERE ID = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next())
        return false;

    this->id = query.value("ID").toInt();
    this->idMaison = query.value("ID_MAISON").toInt();
    this->zone = query.value("ZONE").toString();
    this->niveau = query.value("NIVEAU").toInt();
    this->statut = query.value("STATUT").toString();
    this->dateAlerte = query.value("DATE_ALERTE").toDate();

    return true;
}

QSqlQueryModel* Alerte::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();

    model->setQuery(
        "SELECT ID, ID_MAISON, ZONE, NIVEAU, STATUT, DATE_ALERTE FROM GEST_ALERTES ORDER BY DATE_ALERTE DESC",
        QSqlDatabase::database("qt_oracle")
        );

    return model;
}

bool Alerte::marquerCommeTraitee(int id)
{
    QSqlQuery query(QSqlDatabase::database("qt_oracle"));

    query.prepare("UPDATE GEST_ALERTES SET STATUT = 'Traitée' WHERE ID = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur marquer comme traitée:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Alerte::existeAlertePourMaison(int idMaison)
{
    QSqlQuery query(QSqlDatabase::database("qt_oracle"));

    query.prepare("SELECT COUNT(*) FROM GEST_ALERTES WHERE ID_MAISON = :id_maison AND STATUT != 'Traitée'");
    query.bindValue(":id_maison", idMaison);

    if (!query.exec() || !query.next())
        return false;

    return query.value(0).toInt() > 0;
}

