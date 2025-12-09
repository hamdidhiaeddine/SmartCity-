#include "maison.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

/* =====================
 * CONSTRUCTEURS
 * ===================== */

Maison::Maison(QObject *parent)
    : QObject(parent), id(0), securite(0), nbrPieces(0)
{
}

Maison::Maison(int id, const QString &adresse, int securite, const QString &statut)
{
    this->id = id;
    this->adresse = adresse;
    this->securite = securite;
    this->statut = statut;
    this->type = "";
    this->nbrPieces = 0;
}

Maison::Maison(int id, const QString &adresse, int securite, const QString &statut,
               const QString &type, int nbrPieces)
{
    this->id = id;
    this->adresse = adresse;
    this->securite = securite;
    this->statut = statut;
    this->type = type;
    this->nbrPieces = nbrPieces;
}

/* =====================
 * GETTERS / SETTERS
 * ===================== */

int Maison::getId() const { return id; }
QString Maison::getAdresse() const { return adresse; }
int Maison::getSecurite() const { return securite; }
QString Maison::getStatut() const { return statut; }
QString Maison::getType() const { return type; }
int Maison::getNbrPieces() const { return nbrPieces; }

void Maison::setId(int id) { this->id = id; }
void Maison::setAdresse(const QString &adresse) { this->adresse = adresse; }
void Maison::setSecurite(int securite) { this->securite = securite; }
void Maison::setStatut(const QString &statut) { this->statut = statut; }
void Maison::setType(const QString &type) { this->type = type; }
void Maison::setNbrPieces(int nbrPieces) { this->nbrPieces = nbrPieces; }

/* =====================
 * CRUD
 * ===================== */

bool Maison::ajouter()
{
    // Vérification avant insertion
    if (securite < 1 || securite > 5) {
        qDebug() << "ERREUR: Niveau de sécurité invalide:" << securite;
        qDebug() << "Maison ID:" << id << "Adresse:" << adresse;
        return false;
    }

    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("INSERT INTO GEST_MAISON (ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES)"
              "VALUES (:id, :ad, :sec, :st, :type, :nbr)");

    q.bindValue(":id", id);
    q.bindValue(":ad", adresse);
    q.bindValue(":sec", securite);
    q.bindValue(":st", statut);
    q.bindValue(":type", type);
    q.bindValue(":nbr", nbrPieces);

    if(!q.exec()) {
        qDebug() << "Erreur ajout maison:" << q.lastError();
        qDebug() << "Valeurs: ID=" << id << "SEC=" << securite;
        return false;
    }
    return true;
}

bool Maison::modifier(int id)
{
    // Vérification avant modification
    if (securite < 1 || securite > 5) {
        qDebug() << "ERREUR: Niveau de sécurité invalide pour modification:" << securite;
        qDebug() << "Maison ID:" << id;
        return false;
    }

    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("UPDATE GEST_MAISON SET ASRESSE=:ad, SECURITE=:sec, STATUS=:st,"
              "TYPE=:type, NBRDESPIECES=:nbr WHERE ID=:id");

    q.bindValue(":id", id);
    q.bindValue(":ad", adresse);
    q.bindValue(":sec", securite);
    q.bindValue(":st", statut);
    q.bindValue(":type", type);
    q.bindValue(":nbr", nbrPieces);

    if (!q.exec()) {
        qDebug() << "Erreur modification maison:" << q.lastError();
        qDebug() << "Valeurs: ID=" << id << "SEC=" << securite;
        return false;
    }
    return true;
}

bool Maison::supprimer(int id)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("DELETE FROM GEST_MAISON WHERE ID=:id");
    q.bindValue(":id", id);
    return q.exec();
}

bool Maison::rechercher(int id)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("SELECT * FROM GEST_MAISON WHERE ID=:id");
    q.bindValue(":id", id);

    if(!q.exec() || !q.next()) return false;

    this->id = q.value("ID").toInt();
    adresse  = q.value("ASRESSE").toString();
    statut   = q.value("STATUS").toString();
    securite = q.value("SECURITE").toInt();
    type     = q.value("TYPE").toString();
    nbrPieces= q.value("NBRDESPIECES").toInt();

    return true;
}

QSqlQueryModel* Maison::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(
        "SELECT ID, ASRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES FROM GEST_MAISON ORDER BY ID ASC",
        QSqlDatabase::database("qt_oracle")
    );
    return model;
}

/* =============================
 * STATISTIQUES : STATUT
 * ============================= */

QMap<QString,int> Maison::getStatistiquesStatus()
{
    QMap<QString,int> stats;
    stats["actif"] = 0;
    stats["inactif"] = 0;
    stats["maintenance"] = 0;
    stats["vendu"] = 0;

    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("SELECT STATUS FROM GEST_MAISON");
    if (!q.exec()) return {{"actif",0},{"inactif",0},{"maintenance",0},{"vendu",0},{"total",0}};

    int total = 0;
    while(q.next()) {
        QString s = q.value(0).toString().toLower();

        if(s.contains("actif") && !s.contains("inactif")) stats["actif"]++;
        else if(s.contains("inactif")) stats["inactif"]++;
        else if(s.contains("maintenance")) stats["maintenance"]++;
        else if(s.contains("vendu")) stats["vendu"]++;

        total++;
    }

    stats["total"] = total;
    return stats;
}

/* =============================
 * STATISTIQUES : NIVEAU
 * ============================= */

QMap<QString,int> Maison::getStatistiquesNiveauSecurite()
{
    QMap<QString,int> stats;
    stats["danger"] = 0;
    stats["moyen"]  = 0;
    stats["bon"]    = 0;

    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("SELECT SECURITE FROM GEST_MAISON");
    if (!q.exec()) return {{"danger",0},{"moyen",0},{"bon",0},{"total",0}};

    int total = 0;
    while(q.next()) {
        int s = q.value(0).toInt();

        if(s < 2) stats["danger"]++;
        else if(s <= 3) stats["moyen"]++;
        else stats["bon"]++;

        total++;
    }

    stats["total"] = total;
    return stats;
}
