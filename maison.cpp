#include "maison.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlDatabase>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVariant>

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
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES)"
              "VALUES (:id, :ad, :sec, :st, :type, :nbr)");

    q.bindValue(":id", id);
    q.bindValue(":ad", adresse);
    q.bindValue(":sec", securite);
    q.bindValue(":st", statut);
    q.bindValue(":type", type);
    q.bindValue(":nbr", nbrPieces);

    if(!q.exec()) {
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool Maison::ajouter(int idOverride, QString *errorText)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    int useId = id;
    if (idOverride > 0) useId = idOverride;
    if (useId <= 0) {
        QSqlQuery qid(QSqlDatabase::database("qt_oracle"));
        if (qid.exec("SELECT NVL(MAX(ID),0)+1 FROM GEST_MAISON") && qid.next()) {
            useId = qid.value(0).toInt();
        } else {
            if (errorText) *errorText = qid.lastError().text();
            return false;
        }
    }

    QSqlQuery qcheck(QSqlDatabase::database("qt_oracle"));
    qcheck.prepare("SELECT 1 FROM GEST_MAISON WHERE ID=:id");
    qcheck.bindValue(":id", useId);
    if (qcheck.exec() && qcheck.next()) {
        if (errorText) *errorText = QString("ID %1 existe déjà").arg(useId);
        return false;
    }

    q.prepare("INSERT INTO GEST_MAISON (ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES) VALUES (:id, :ad, :sec, :st, :type, :nbr)");
    q.bindValue(":id", useId);
    q.bindValue(":ad", adresse.trimmed());
    q.bindValue(":sec", securite);
    q.bindValue(":st", statut);
    q.bindValue(":type", type);
    q.bindValue(":nbr", nbrPieces);
    if (!q.exec()) {
        qDebug() << "INSERT GEST_MAISON error:" << q.lastError().text();
        if (errorText) *errorText = q.lastError().text();
        return false;
    }
    id = useId;
    qDebug() << "INSERT GEST_MAISON ok id=" << id << " adresse=" << adresse;
    return true;
}

bool Maison::modifier(int id)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("UPDATE GEST_MAISON SET ADRESSE=:ad, SECURITE=:sec, STATUS=:st,"
              "TYPE=:type, NBRDESPIECES=:nbr WHERE ID=:id");

    q.bindValue(":id", id);
    q.bindValue(":ad", adresse);
    q.bindValue(":sec", securite);
    q.bindValue(":st", statut);
    q.bindValue(":type", type);
    q.bindValue(":nbr", nbrPieces);

    return q.exec();
}

bool Maison::modifier(int oldId, int newId, QString *errorText)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    int targetId = oldId;
    if (newId > 0 && newId != oldId) {
        QSqlQuery exists(QSqlDatabase::database("qt_oracle"));
        exists.prepare("SELECT 1 FROM GEST_MAISON WHERE ID=:id");
        exists.bindValue(":id", newId);
        if (exists.exec() && exists.next()) {
            if (errorText) *errorText = QString("L'ID %1 existe déjà").arg(newId);
            return false;
        }
        targetId = newId;
    }

    q.prepare("UPDATE GEST_MAISON SET ID=:newid, ADRESSE=:ad, SECURITE=:sec, STATUS=:st, TYPE=:type, NBRDESPIECES=:nbr WHERE ID=:oldid");
    q.bindValue(":newid", targetId);
    q.bindValue(":ad", adresse);
    q.bindValue(":sec", securite);
    q.bindValue(":st", statut);
    q.bindValue(":type", type);
    q.bindValue(":nbr", nbrPieces);
    q.bindValue(":oldid", oldId);
    if (!q.exec()) {
        if (errorText) *errorText = q.lastError().text();
        return false;
    }
    id = targetId;
    return true;
}

bool Maison::supprimer(int id)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("DELETE FROM GEST_MAISON WHERE ID=:id");
    q.bindValue(":id", id);
    return q.exec();
}

bool Maison::supprimer(int id, QString *errorText)
{
    QSqlDatabase db = QSqlDatabase::database("qt_oracle");
    db.transaction();
    QSqlQuery q(db);
    q.prepare("DELETE FROM GEST_MAISON WHERE ID=:id");
    q.bindValue(":id", id);
    if (!q.exec()) {
        if (errorText) *errorText = q.lastError().text();
        db.rollback();
        return false;
    }
    if (!db.commit()) {
        if (errorText) *errorText = db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

bool Maison::rechercher(int id)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("SELECT * FROM GEST_MAISON WHERE ID=:id");
    q.bindValue(":id", id);

    if(!q.exec() || !q.next()) return false;

    this->id = q.value("ID").toInt();
    adresse  = q.value("ADRESSE").toString();
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
        "SELECT ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES FROM GEST_MAISON ORDER BY ID ASC",
        QSqlDatabase::database("qt_oracle")
        );
    return model;
}

void Maison::afficher(QTableWidget *table)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("SELECT ID, ADRESSE, SECURITE, STATUS, TYPE, NBRDESPIECES FROM GEST_MAISON ORDER BY ID ASC");
    if (!q.exec()) {
        table->clear();
        table->setRowCount(0);
        table->setColumnCount(0);
        return;
    }
    QVector<QVector<QString>> rows;
    QVector<int> maisonIds;
    while (q.next()) {
        QVector<QString> r;
        int idMaison = q.value(0).toInt();
        r << q.value(0).toString()
          << q.value(1).toString()
          << q.value(2).toString()
          << q.value(3).toString()
          << q.value(4).toString()
          << q.value(5).toString();
        
        // Récupérer les résidents pour cette maison
        QStringList residents = getResidentsParMaison(idMaison);
        QString residentsText = residents.isEmpty() ? "Aucun" : QString::number(residents.count()) + " résident(s)";
        r << residentsText;
        
        rows.push_back(r);
        maisonIds.push_back(idMaison);
    }
    table->clear();
    table->setColumnCount(7);
    table->setRowCount(rows.size());
    QStringList headers; headers << "ID Maison" << "Adresse" << "Sécurité" << "Statut" << "Type" << "Nbr Pièces" << "Résidents";
    table->setHorizontalHeaderLabels(headers);
    for (int i = 0; i < rows.size(); ++i) {
        const auto &r = rows[i];
        for (int c = 0; c < r.size(); ++c) {
            table->setItem(i, c, new QTableWidgetItem(r[c]));
        }
    }
}

/* =============================
 * STATISTIQUES : STATUT
 * ============================= */

QMap<QString,int> Maison::getStatistiquesStatus()
{
    QMap<QString,int> stats;
    stats["vide"] = 0;
    stats["occupe"] = 0;

    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("SELECT STATUS FROM GEST_MAISON");
    if (!q.exec()) return {{"vide",0},{"occupe",0},{"total",0}};

    int total = 0;
    while(q.next()) {
        QString s = q.value(0).toString().toLower();

        if(s.contains("vide")) stats["vide"]++;
        else stats["occupe"]++;

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

bool Maison::validateAdresse(const QString &adresse, QString &error)
{
    QString trimmed = adresse.trimmed();
    if (trimmed.isEmpty()) { error = "Adresse requise"; return false; }
    if (trimmed.length() > 200) { error = "Adresse trop longue (max 200)"; return false; }
    return true;
}

bool Maison::validateSecurite(const QString &securiteText, QString &error)
{
    bool ok=false; int s = securiteText.trimmed().toInt(&ok);
    if (!ok) { error = "Sécurité doit être numérique"; return false; }
    if (s < 0 || s > 10) { error = "Sécurité doit être entre 0 et 10"; return false; }
    return true;
}

bool Maison::validateStatut(const QString &statut, QString &error)
{
    QString t = statut.trimmed().toLower();
    if (t.isEmpty()) { error = "Statut requis"; return false; }
    if (t != "vide" && t != "occupe" && t != "occupé") { error = "Statut invalide (vide/occupé)"; return false; }
    return true;
}

bool Maison::validateType(const QString &type, QString &error)
{
    QString t = type.trimmed();
    if (t.isEmpty()) { error = "Type requis"; return false; }
    if (t.length() > 50) { error = "Type trop long (max 50)"; return false; }
    return true;
}

bool Maison::validateNbrPieces(int nbrPieces, QString &error)
{
    if (nbrPieces <= 0) { error = "Nombre de pièces doit être > 0"; return false; }
    if (nbrPieces > 50) { error = "Nombre de pièces trop grand"; return false; }
    return true;
}

bool Maison::idExists(int id)
{
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    q.prepare("SELECT 1 FROM GEST_MAISON WHERE ID=:id");
    q.bindValue(":id", id);
    return (q.exec() && q.next());
}

QStringList Maison::getResidentsParMaison(int idMaison)
{
    QStringList residents;
    QSqlQuery q(QSqlDatabase::database("qt_oracle"));
    
    // Requête pour récupérer les résidents par ID_MAISON (clé étrangère)
    q.prepare("SELECT NOM, PRENOM, TELEPHONE "
              "FROM GEST_RESIDENT "
              "WHERE ID_MAISON = :id_maison "
              "ORDER BY NOM, PRENOM");
    q.bindValue(":id_maison", idMaison);
    
    if (q.exec()) {
        while (q.next()) {
            QString nom = q.value(0).toString();
            QString prenom = q.value(1).toString();
            QString telephone = q.value(2).toString();
            residents << QString("%1 %2 (%3)").arg(nom, prenom, telephone);
        }
    }
    
    return residents;
}

bool Maison::assignerResidentAMaison(const QString &idResident, int idMaison)
{
    // Vérifier que la maison existe
    QSqlQuery qMaison(QSqlDatabase::database("qt_oracle"));
    qMaison.prepare("SELECT ID, ADRESSE FROM GEST_MAISON WHERE ID = :id");
    qMaison.bindValue(":id", idMaison);
    
    if (!qMaison.exec() || !qMaison.next()) {
        qDebug() << "Maison introuvable";
        return false;
    }
    
    QString adresseMaison = qMaison.value(1).toString();
    
    // Mettre à jour ID_MAISON et ADRESSE du résident
    QSqlQuery qUpdate(QSqlDatabase::database("qt_oracle"));
    qUpdate.prepare("UPDATE GEST_RESIDENT SET ID_MAISON = :id_maison, ADRESSE = :adresse WHERE ID = :id");
    qUpdate.bindValue(":id_maison", idMaison);
    qUpdate.bindValue(":adresse", adresseMaison);
    qUpdate.bindValue(":id", idResident);
    
    if (!qUpdate.exec()) {
        qDebug() << "Erreur lors de l'assignation du résident:" << qUpdate.lastError().text();
        return false;
    }
    
    qDebug() << "Résident" << idResident << "assigné à la maison" << idMaison;
    return true;
}
// Convenience constructor used by UI: adresse, statut, securiteText, nbrPieces, type
Maison::Maison(const QString &adresseIn, const QString &statutIn, const QString &securiteText, int nbrPiecesIn, const QString &typeIn)
{
    id = 0;
    adresse = adresseIn;
    statut = statutIn;
    bool ok = false;
    int sec = securiteText.trimmed().toInt(&ok);
    securite = ok ? sec : 0;
    type = typeIn;
    nbrPieces = nbrPiecesIn;
}
