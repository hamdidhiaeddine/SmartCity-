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
    : m_id(0), m_superficie(0.0), m_temperatureMoyenneSol(0.0)
{
}

Jardin::Jardin(int id,
               const QString &emplacement,
               double superficie,
               const QString &typeSol,
               double temperatureMoyenneSol,
               const QString &typeChoix)
    : m_id(id)
    , m_emplacement(emplacement)
    , m_superficie(superficie)
    , m_typeSol(typeSol)
    , m_temperatureMoyenneSol(temperatureMoyenneSol)
    , m_typeChoix(typeChoix)
{
}

int Jardin::id() const { return m_id; }
QString Jardin::emplacement() const { return m_emplacement; }
double Jardin::superficie() const { return m_superficie; }
QString Jardin::typeSol() const { return m_typeSol; }
double Jardin::temperatureMoyenneSol() const { return m_temperatureMoyenneSol; }
QString Jardin::typeChoix() const { return m_typeChoix; }

void Jardin::setId(int value) { m_id = value; }
void Jardin::setEmplacement(const QString &value) { m_emplacement = value; }
void Jardin::setSuperficie(double value) { m_superficie = value; }
void Jardin::setTypeSol(const QString &value) { m_typeSol = value; }
void Jardin::setTemperatureMoyenneSol(double value) { m_temperatureMoyenneSol = value; }
void Jardin::setTypeChoix(const QString &value) { m_typeChoix = value; }

QSqlDatabase Jardin::database()
{
    QSqlDatabase db = resolveDb();
    if (!db.isValid()) {
        qWarning() << "Connexion BDD invalide pour Jardin";
        qWarning() << "   Connexions disponibles:" << QSqlDatabase::connectionNames();
    } else if (!db.isOpen()) {
        qWarning() << "Connexion BDD fermée pour Jardin, tentative d'ouverture...";
        if (!db.open()) {
            qCritical() << "Impossible d'ouvrir la connexion:" << db.lastError().text();
        }
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
    
    if (!db.isOpen()) {
        if (!db.open()) {
            QString error = db.lastError().text();
            if (errorText) *errorText = QString("Impossible d'ouvrir la connexion: %1").arg(error);
            qCritical() << "Jardin::ajouter - Impossible d'ouvrir la connexion:" << error;
            return false;
        }
    }
    
    db.transaction();

    // Normaliser TYPE_CHOIX selon la contrainte CHK_JARDIN_TYPE
    // La contrainte attend EXACTEMENT: 'Publique', 'Privee', 'Autres' (sensible à la casse)
    // IMPORTANT: Supprimer TOUS les espaces et caractères invisibles
    QString typeChoixNormalise = m_typeChoix.trimmed();
    
    // Supprimer tous les espaces (y compris les espaces insécables)
    typeChoixNormalise = typeChoixNormalise.simplified();
    typeChoixNormalise.replace(" ", "");
    typeChoixNormalise.replace(QChar(0x00A0), ""); // Espace insécable
    typeChoixNormalise.replace(QChar(0x2009), ""); // Espace fine
    typeChoixNormalise.replace(QChar(0x202F), ""); // Espace insécable étroite
    
    // Convertir en minuscules pour la comparaison (mais garder la casse exacte pour la base)
    QString typeChoixLower = typeChoixNormalise.toLower();
    
    // Mapper les valeurs possibles vers les valeurs acceptées par la contrainte
    // Vérifier d'abord les correspondances exactes (sans espaces)
    if (typeChoixNormalise.compare("Publique", Qt::CaseSensitive) == 0) {
        typeChoixNormalise = "Publique";
    }
    else if (typeChoixNormalise.compare("Privee", Qt::CaseSensitive) == 0) {
        typeChoixNormalise = "Privee";
    }
    else if (typeChoixNormalise.compare("Autres", Qt::CaseSensitive) == 0) {
        typeChoixNormalise = "Autres";
    }
    // Vérifier les variantes pour "Publique"
    else if (typeChoixLower.startsWith("public") || 
             typeChoixLower.startsWith("communal") ||
             typeChoixLower.startsWith("communa")) {
        typeChoixNormalise = "Publique";
    }
    // Vérifier les variantes pour "Privee"
    else if (typeChoixLower.startsWith("prive") || 
             typeChoixLower.startsWith("private")) {
        typeChoixNormalise = "Privee";
    }
    // Vérifier les variantes pour "Autres"
    else if (typeChoixLower.startsWith("autre") || 
             typeChoixLower.startsWith("other")) {
        typeChoixNormalise = "Autres";
    }
    // Si la valeur n'est pas reconnue, utiliser "Autres" par défaut
    else {
        qWarning() << "Jardin::ajouter - Type non reconnu:" << typeChoixNormalise << ", utilisation de 'Autres'";
        qWarning() << "   Valeur originale:" << m_typeChoix;
        qWarning() << "   Longueur:" << typeChoixNormalise.length();
        qWarning() << "   Code ASCII:" << typeChoixNormalise.toUtf8().toHex();
        typeChoixNormalise = "Autres";
    }
    
    // Vérification finale: s'assurer que la valeur est exactement l'une des trois acceptées
    if (typeChoixNormalise != "Publique" && typeChoixNormalise != "Privee" && typeChoixNormalise != "Autres") {
        qCritical() << "ERREUR CRITIQUE: Normalisation échouée! Valeur:" << typeChoixNormalise;
        typeChoixNormalise = "Autres"; // Valeur par défaut sûre
    }
    
    qDebug() << "   Type original:" << m_typeChoix << "→ Normalisé:" << typeChoixNormalise;
    qDebug() << "   Longueur normalisée:" << typeChoixNormalise.length();

    QSqlQuery query(db);
    query.prepare("INSERT INTO GEST_JARDIN (ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX) "
                  "VALUES (:id, :emplacement, :superficie, :typeSol, :tempMoy, :typeChoix)");
    query.bindValue(":id", m_id);
    query.bindValue(":emplacement", m_emplacement.trimmed());
    query.bindValue(":superficie", m_superficie);
    query.bindValue(":typeSol", m_typeSol.trimmed());
    query.bindValue(":tempMoy", m_temperatureMoyenneSol);
    query.bindValue(":typeChoix", typeChoixNormalise);

    if (!query.exec()) {
        QString errorMsg = query.lastError().text();
        QString nativeCode = query.lastError().nativeErrorCode();
        
        db.rollback();
        
        // Si erreur de contrainte CHECK (ORA-02290)
        if (errorMsg.contains("CHK_JARDIN_TYPE", Qt::CaseInsensitive) || 
            errorMsg.contains("check constraint", Qt::CaseInsensitive) ||
            nativeCode == "2290") {
            QString detailedError = QString("Erreur de contrainte CHK_JARDIN_TYPE.\n\n")
                                  + "La valeur '" + m_typeChoix + "' n'est pas valide.\n\n"
                                  + "Valeurs acceptées:\n"
                                  + "• Publique\n"
                                  + "• Privee\n"
                                  + "• Autres\n\n"
                                  + "Valeur normalisée tentée: " + typeChoixNormalise + "\n\n"
                                  + "SOLUTION: Exécutez CORRIGER_CONTRAINTE_JARDIN.bat";
            if (errorText) *errorText = detailedError;
            qWarning() << "INSERT GEST_JARDIN - Erreur contrainte CHK_JARDIN_TYPE";
            qWarning() << "   Type original:" << m_typeChoix;
            qWarning() << "   Type normalisé:" << typeChoixNormalise;
            qWarning() << "   Longueur:" << typeChoixNormalise.length();
            qWarning() << "   Erreur complète:" << errorMsg;
        } else {
            if (errorText) *errorText = errorMsg;
            qWarning() << "INSERT GEST_JARDIN - Erreur:" << errorMsg;
        }
        qWarning() << "   Code erreur:" << nativeCode;
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        if (errorText) *errorText = QString("Erreur lors du commit: %1").arg(error);
        qCritical() << "Jardin::ajouter - Erreur commit:" << error;
        return false;
    }
    
    qDebug() << "✅ Jardin ajouté avec succès. ID:" << m_id;
    return true;
}

bool Jardin::modifier(int oldId, QString *errorText) const
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return false;
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            QString error = db.lastError().text();
            if (errorText) *errorText = QString("Impossible d'ouvrir la connexion: %1").arg(error);
            qCritical() << "Jardin::modifier - Impossible d'ouvrir la connexion:" << error;
            return false;
        }
    }
    
    db.transaction();

    if (m_id != oldId && idExists(m_id)) {
        if (errorText) *errorText = QObject::tr("L'ID %1 existe déjà").arg(m_id);
        return false;
    }

    // Normaliser TYPE_CHOIX selon la contrainte CHK_JARDIN_TYPE
    // La contrainte attend EXACTEMENT: 'Publique', 'Privee', 'Autres' (sensible à la casse)
    // IMPORTANT: Supprimer TOUS les espaces et caractères invisibles
    QString typeChoixNormalise = m_typeChoix.trimmed();
    
    // Supprimer tous les espaces (y compris les espaces insécables)
    typeChoixNormalise = typeChoixNormalise.simplified();
    typeChoixNormalise.replace(" ", "");
    typeChoixNormalise.replace(QChar(0x00A0), ""); // Espace insécable
    typeChoixNormalise.replace(QChar(0x2009), ""); // Espace fine
    typeChoixNormalise.replace(QChar(0x202F), ""); // Espace insécable étroite
    
    // Convertir en minuscules pour la comparaison (mais garder la casse exacte pour la base)
    QString typeChoixLower = typeChoixNormalise.toLower();
    
    // Mapper les valeurs possibles vers les valeurs acceptées par la contrainte
    // Vérifier d'abord les correspondances exactes (sans espaces)
    if (typeChoixNormalise.compare("Publique", Qt::CaseSensitive) == 0) {
        typeChoixNormalise = "Publique";
    }
    else if (typeChoixNormalise.compare("Privee", Qt::CaseSensitive) == 0) {
        typeChoixNormalise = "Privee";
    }
    else if (typeChoixNormalise.compare("Autres", Qt::CaseSensitive) == 0) {
        typeChoixNormalise = "Autres";
    }
    // Vérifier les variantes pour "Publique"
    else if (typeChoixLower.startsWith("public") || 
             typeChoixLower.startsWith("communal") ||
             typeChoixLower.startsWith("communa")) {
        typeChoixNormalise = "Publique";
    }
    // Vérifier les variantes pour "Privee"
    else if (typeChoixLower.startsWith("prive") || 
             typeChoixLower.startsWith("private")) {
        typeChoixNormalise = "Privee";
    }
    // Vérifier les variantes pour "Autres"
    else if (typeChoixLower.startsWith("autre") || 
             typeChoixLower.startsWith("other")) {
        typeChoixNormalise = "Autres";
    }
    // Si la valeur n'est pas reconnue, utiliser "Autres" par défaut
    else {
        qWarning() << "Jardin::modifier - Type non reconnu:" << typeChoixNormalise << ", utilisation de 'Autres'";
        qWarning() << "   Valeur originale:" << m_typeChoix;
        qWarning() << "   Longueur:" << typeChoixNormalise.length();
        qWarning() << "   Code ASCII:" << typeChoixNormalise.toUtf8().toHex();
        typeChoixNormalise = "Autres";
    }
    
    // Vérification finale: s'assurer que la valeur est exactement l'une des trois acceptées
    if (typeChoixNormalise != "Publique" && typeChoixNormalise != "Privee" && typeChoixNormalise != "Autres") {
        qCritical() << "ERREUR CRITIQUE: Normalisation échouée! Valeur:" << typeChoixNormalise;
        typeChoixNormalise = "Autres"; // Valeur par défaut sûre
    }
    
    qDebug() << "   Type original:" << m_typeChoix << "→ Normalisé:" << typeChoixNormalise;
    qDebug() << "   Longueur normalisée:" << typeChoixNormalise.length();

    QSqlQuery query(db);
    query.prepare("UPDATE GEST_JARDIN SET ID_JARDIN = :newId, EMPLACEMENT = :emplacement, "
                  "SUPERFICIE = :superficie, TYPE_SOL = :typeSol, TEMPERATURE_MOYENNE_SOL = :tempMoy, TYPE_CHOIX = :typeChoix WHERE ID_JARDIN = :oldId");
    query.bindValue(":newId", m_id);
    query.bindValue(":emplacement", m_emplacement.trimmed());
    query.bindValue(":superficie", m_superficie);
    query.bindValue(":typeSol", m_typeSol.trimmed());
    query.bindValue(":tempMoy", m_temperatureMoyenneSol);
    query.bindValue(":typeChoix", typeChoixNormalise);
    query.bindValue(":oldId", oldId);

    if (!query.exec()) {
        QString errorMsg = query.lastError().text();
        QString nativeCode = query.lastError().nativeErrorCode();
        
        db.rollback();
        
        // Si erreur de contrainte CHECK (ORA-02290)
        if (errorMsg.contains("CHK_JARDIN_TYPE", Qt::CaseInsensitive) || 
            errorMsg.contains("check constraint", Qt::CaseInsensitive) ||
            nativeCode == "2290") {
            QString detailedError = QString("Erreur de contrainte CHK_JARDIN_TYPE.\n\n")
                                  + "La valeur '" + m_typeChoix + "' n'est pas valide.\n\n"
                                  + "Valeurs acceptées:\n"
                                  + "• Publique\n"
                                  + "• Privee\n"
                                  + "• Autres\n\n"
                                  + "Valeur normalisée tentée: " + typeChoixNormalise + "\n\n"
                                  + "SOLUTION: Exécutez CORRIGER_CONTRAINTE_JARDIN.bat";
            if (errorText) *errorText = detailedError;
            qWarning() << "UPDATE GEST_JARDIN - Erreur contrainte CHK_JARDIN_TYPE";
            qWarning() << "   Type original:" << m_typeChoix;
            qWarning() << "   Type normalisé:" << typeChoixNormalise;
            qWarning() << "   Longueur:" << typeChoixNormalise.length();
            qWarning() << "   Erreur complète:" << errorMsg;
        } else {
            if (errorText) *errorText = errorMsg;
            qWarning() << "UPDATE GEST_JARDIN - Erreur:" << errorMsg;
        }
        qWarning() << "   Code erreur:" << nativeCode;
        return false;
    }
    
    // Vérifier que des lignes ont été affectées
    int rowsAffected = query.numRowsAffected();
    if (rowsAffected == 0) {
        db.rollback();
        if (errorText) *errorText = QObject::tr("Aucune modification effectuée. Vérifiez que l'ID existe.");
        qWarning() << "UPDATE GEST_JARDIN - Aucune ligne modifiée pour ID:" << oldId;
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        if (errorText) *errorText = QString("Erreur lors du commit: %1").arg(error);
        qCritical() << "Jardin::modifier - Erreur commit:" << error;
        return false;
    }
    
    qDebug() << "✅ Jardin modifié avec succès. ID:" << oldId << "→" << m_id;
    return true;
}

bool Jardin::supprimer(int id, QString *errorText)
{
    QSqlDatabase db = database();
    if (!db.isValid()) {
        if (errorText) *errorText = QObject::tr("Connexion base de données indisponible");
        return false;
    }
    
    if (!db.isOpen()) {
        if (!db.open()) {
            QString error = db.lastError().text();
            if (errorText) *errorText = QString("Impossible d'ouvrir la connexion: %1").arg(error);
            qCritical() << "Jardin::supprimer - Impossible d'ouvrir la connexion:" << error;
            return false;
        }
    }
    
    db.transaction();
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM GEST_JARDIN WHERE ID_JARDIN = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QString error = query.lastError().text();
        db.rollback();
        if (errorText) *errorText = error;
        qWarning() << "DELETE GEST_JARDIN - Erreur:" << error;
        qWarning() << "   Code erreur:" << query.lastError().nativeErrorCode();
        return false;
    }
    
    int rowsAffected = query.numRowsAffected();
    if (rowsAffected == 0) {
        db.rollback();
        if (errorText) *errorText = QObject::tr("Aucun jardin trouvé avec l'ID %1").arg(id);
        qWarning() << "DELETE GEST_JARDIN - Aucune ligne supprimée pour ID:" << id;
        return false;
    }
    
    if (!db.commit()) {
        QString error = db.lastError().text();
        db.rollback();
        if (errorText) *errorText = QString("Erreur lors du commit: %1").arg(error);
        qCritical() << "Jardin::supprimer - Erreur commit:" << error;
        return false;
    }
    
    qDebug() << "✅ Jardin supprimé avec succès. ID:" << id << "(" << rowsAffected << "ligne(s) affectée(s))";
    return true;
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
    if (!query.exec("SELECT ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX "
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
        jardin.m_temperatureMoyenneSol = query.value(4).toDouble();
        jardin.m_typeChoix = query.value(5).toString();
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
    if (!query.exec("SELECT ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX "
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
        jardin.m_temperatureMoyenneSol = query.value(4).toDouble();
        jardin.m_typeChoix = query.value(5).toString();
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
    if (!query.exec("SELECT ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX "
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
        jardin.m_temperatureMoyenneSol = query.value(4).toDouble();
        jardin.m_typeChoix = query.value(5).toString();
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
    if (!query.exec("SELECT ID_JARDIN, EMPLACEMENT, SUPERFICIE, TYPE_SOL, TEMPERATURE_MOYENNE_SOL, TYPE_CHOIX "
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
        jardin.m_temperatureMoyenneSol = query.value(4).toDouble();
        jardin.m_typeChoix = query.value(5).toString();
        result.append(jardin);
    }

    return result;
}
