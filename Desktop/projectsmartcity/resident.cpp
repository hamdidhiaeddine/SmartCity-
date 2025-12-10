#include "resident.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QVariant>
#include <QDebug>

namespace {
const char *kTableName = "GEST_RESIDENT";

QSqlDatabase resolveDb()
{
    if (QSqlDatabase::contains("qt_oracle")) {
        return QSqlDatabase::database("qt_oracle");
    }
    return QSqlDatabase::database();
}
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
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qCritical() << "Resident::ajouter - Connexion base de données invalide";
        return false;
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            qCritical() << "Resident::ajouter - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return false;
        }
    }
    
    db.transaction();
    
    // Convertir l'ID en nombre pour Oracle
    bool ok;
    int idNum = resident.id().toInt(&ok);
    if (!ok || idNum <= 0) {
        db.rollback();
        qCritical() << "❌ Resident::ajouter - ID invalide:" << resident.id();
        return false;
    }
    
    // Vérifier si l'ID existe déjà
    QSqlQuery checkQuery(db);
    checkQuery.prepare(QStringLiteral("SELECT 1 FROM %1 WHERE ID = :id").arg(kTableName));
    checkQuery.bindValue(":id", idNum);
    if (checkQuery.exec() && checkQuery.next()) {
        db.rollback();
        qDebug() << "❌ Erreur: L'ID" << idNum << "existe déjà !";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO %1 (ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE) "
        "VALUES (:id, :nom, :prenom, :datenaiss, :adresse, :telephone, :email, :statut, :situation)")
                      .arg(kTableName));
    query.bindValue(":id", idNum);
    query.bindValue(":nom", resident.nom().trimmed());
    query.bindValue(":prenom", resident.prenom().trimmed());
    query.bindValue(":datenaiss", resident.dateNaissance());
    query.bindValue(":adresse", resident.adresse().trimmed());
    query.bindValue(":telephone", resident.telephone().trimmed());
    query.bindValue(":email", resident.email().trimmed());
    query.bindValue(":statut", resident.statut().trimmed());
    query.bindValue(":situation", resident.situation().trimmed());

    if (!query.exec()) {
        QString error = query.lastError().text();
        QString nativeCode = query.lastError().nativeErrorCode();
        db.rollback();
        qWarning() << "❌ Resident::ajouter - erreur SQL:" << error;
        qWarning() << "   Code erreur Oracle:" << nativeCode;
        qWarning() << "   ID:" << idNum;
        qWarning() << "   Nom:" << resident.nom();
        qWarning() << "   Prénom:" << resident.prenom();
        
        // Messages d'erreur spécifiques
        if (error.contains("unique constraint", Qt::CaseInsensitive) || nativeCode == "1") {
            qWarning() << "   → L'ID existe déjà ou contrainte unique violée";
        } else if (error.contains("not null", Qt::CaseInsensitive) || nativeCode == "1400") {
            qWarning() << "   → Une colonne NOT NULL est vide";
        } else if (error.contains("check constraint", Qt::CaseInsensitive) || nativeCode == "2290") {
            qWarning() << "   → Contrainte CHECK violée";
        } else if (error.contains("foreign key", Qt::CaseInsensitive) || nativeCode == "2291" || nativeCode == "2292") {
            qWarning() << "   → Contrainte de clé étrangère violée";
        }
        
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        qCritical() << "Resident::ajouter - Erreur commit:" << error;
        return false;
    }
    
    qDebug() << "✅ Résident ajouté avec succès. ID:" << idNum;
    return true;
}

bool Resident::modifier(const Resident &resident)
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qCritical() << "Resident::modifier - Connexion base de données invalide";
        return false;
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            qCritical() << "Resident::modifier - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return false;
        }
    }
    
    db.transaction();
    
    // Convertir l'ID en nombre pour Oracle
    bool ok;
    int idNum = resident.id().toInt(&ok);
    if (!ok || idNum <= 0) {
        db.rollback();
        qCritical() << "❌ Resident::modifier - ID invalide:" << resident.id();
        return false;
    }
    
    // Vérifier que l'ID existe
    QSqlQuery checkQuery(db);
    checkQuery.prepare(QStringLiteral("SELECT 1 FROM %1 WHERE ID = :id").arg(kTableName));
    checkQuery.bindValue(":id", idNum);
    if (!checkQuery.exec() || !checkQuery.next()) {
        db.rollback();
        qDebug() << "❌ Erreur: L'ID" << idNum << "n'existe pas !";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "UPDATE %1 SET NOM = :nom, PRENOM = :prenom, DATENAISS = :datenaiss, ADRESSE = :adresse, "
        "TELEPHONE = :telephone, EMAIL = :email, STATUT = :statut, SITUATIONFAMILIALE = :situation "
        "WHERE ID = :id")
                      .arg(kTableName));
    query.bindValue(":nom", resident.nom().trimmed());
    query.bindValue(":prenom", resident.prenom().trimmed());
    query.bindValue(":datenaiss", resident.dateNaissance());
    query.bindValue(":adresse", resident.adresse().trimmed());
    query.bindValue(":telephone", resident.telephone().trimmed());
    query.bindValue(":email", resident.email().trimmed());
    query.bindValue(":statut", resident.statut().trimmed());
    query.bindValue(":situation", resident.situation().trimmed());
    query.bindValue(":id", idNum);

    if (!query.exec()) {
        QString error = query.lastError().text();
        QString nativeCode = query.lastError().nativeErrorCode();
        db.rollback();
        qWarning() << "❌ Resident::modifier - erreur SQL:" << error;
        qWarning() << "   Code erreur Oracle:" << nativeCode;
        qWarning() << "   ID:" << idNum;
        qWarning() << "   Nom:" << resident.nom();
        qWarning() << "   Prénom:" << resident.prenom();
        
        // Messages d'erreur spécifiques
        if (error.contains("not null", Qt::CaseInsensitive) || nativeCode == "1400") {
            qWarning() << "   → Une colonne NOT NULL est vide";
        } else if (error.contains("check constraint", Qt::CaseInsensitive) || nativeCode == "2290") {
            qWarning() << "   → Contrainte CHECK violée";
        }
        
        return false;
    }
    
    int rowsAffected = query.numRowsAffected();
    if (rowsAffected == 0) {
        db.rollback();
        qDebug() << "⚠️ Aucune ligne modifiée pour ID:" << idNum;
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        qCritical() << "Resident::modifier - Erreur commit:" << error;
        return false;
    }
    
    qDebug() << "✅ Résident modifié avec succès. ID:" << idNum << "(" << rowsAffected << "ligne(s) affectée(s))";
    return true;
}

bool Resident::supprimer(const QString &id)
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qCritical() << "Resident::supprimer - Connexion base de données invalide";
        return false;
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            qCritical() << "Resident::supprimer - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return false;
        }
    }
    
    db.transaction();
    
    // Convertir l'ID en nombre pour Oracle
    bool ok;
    int idNum = id.toInt(&ok);
    if (!ok || idNum <= 0) {
        db.rollback();
        qCritical() << "❌ Resident::supprimer - ID invalide:" << id;
        return false;
    }
    
    // Vérifier que l'ID existe
    QSqlQuery checkQuery(db);
    checkQuery.prepare(QStringLiteral("SELECT 1 FROM %1 WHERE ID = :id").arg(kTableName));
    checkQuery.bindValue(":id", idNum);
    if (!checkQuery.exec() || !checkQuery.next()) {
        db.rollback();
        qDebug() << "❌ Erreur: L'ID" << idNum << "n'existe pas !";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM %1 WHERE ID = :id").arg(kTableName));
    query.bindValue(":id", idNum);
    
    if (!query.exec()) {
        QString error = query.lastError().text();
        QString nativeCode = query.lastError().nativeErrorCode();
        db.rollback();
        qWarning() << "❌ Resident::supprimer - erreur SQL:" << error;
        qWarning() << "   Code erreur Oracle:" << nativeCode;
        qWarning() << "   ID:" << idNum;
        
        // Messages d'erreur spécifiques
        if (error.contains("foreign key", Qt::CaseInsensitive) || nativeCode == "2292") {
            qWarning() << "   → Impossible de supprimer: des enregistrements référencent ce résident";
            qWarning() << "   → Vérifiez les tables: GEST_VEHICULE, GEST_EMPLOYE, GEST_CABINET, etc.";
        } else if (error.contains("child record", Qt::CaseInsensitive)) {
            qWarning() << "   → Des enregistrements enfants existent pour ce résident";
        }
        
        return false;
    }
    
    int rowsAffected = query.numRowsAffected();
    if (rowsAffected == 0) {
        db.rollback();
        qWarning() << "⚠️ Resident::supprimer - Aucune ligne supprimée. ID:" << idNum;
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        qCritical() << "Resident::supprimer - Erreur commit:" << error;
        return false;
    }
    
    qDebug() << "✅ Résident supprimé avec succès. ID:" << idNum << "(" << rowsAffected << "ligne(s) affectée(s))";
    return true;
}

bool Resident::existe(const QString &id)
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid() || !db.isOpen()) {
        if (!db.open()) {
            qWarning() << "Resident::existe - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return false;
        }
    }
    
    // Convertir l'ID en nombre pour Oracle
    bool ok;
    int idNum = id.toInt(&ok);
    if (!ok || idNum <= 0) {
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT 1 FROM %1 WHERE ID = :id").arg(kTableName));
    query.bindValue(":id", idNum);
    if (!query.exec()) {
        qWarning() << "Resident::existe - erreur SQL:" << query.lastError().text();
        return false;
    }
    return query.next();
}

QList<Resident> Resident::recupererTout()
{
    QList<Resident> residents;
    QSqlDatabase db = resolveDb();
    
    if (!db.isValid() || !db.isOpen()) {
        if (!db.open()) {
            qWarning() << "Resident::recupererTout - Impossible d'ouvrir la connexion:" << db.lastError().text();
            return residents;
        }
    }
    
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT ID, NOM, PRENOM, DATENAISS, ADRESSE, TELEPHONE, EMAIL, STATUT, SITUATIONFAMILIALE "
        "FROM %1 ORDER BY ID")
                        .arg(kTableName));

    if (!query.exec()) {
        qWarning() << "Resident::recupererTout - erreur SQL:" << query.lastError().text();
        return residents;
    }

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

    return residents;
}

