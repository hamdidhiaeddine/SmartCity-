#include "recommandationplante.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QRandomGenerator>
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

RecommandationPlante::RecommandationPlante()
    : m_idPlante(-1)
    , m_typeSol()
    , m_recommandation()
    , m_nomPlante()
    , m_compatibilite("Moyenne")
    , m_saison()
{
}

RecommandationPlante::RecommandationPlante(int idPlante,
                                           const QString &typeSol,
                                           const QString &recommandation,
                                           const QString &nomPlante,
                                           const QString &compatibilite,
                                           const QString &saison)
    : m_idPlante(idPlante)
    , m_typeSol(typeSol)
    , m_recommandation(recommandation)
    , m_nomPlante(nomPlante)
    , m_compatibilite(compatibilite.isEmpty() ? "Moyenne" : compatibilite)
    , m_saison(saison)
{
}

int RecommandationPlante::idPlante() const { return m_idPlante; }
QString RecommandationPlante::typeSol() const { return m_typeSol; }
QString RecommandationPlante::recommandation() const { return m_recommandation; }
QString RecommandationPlante::nomPlante() const { return m_nomPlante; }
QString RecommandationPlante::compatibilite() const { return m_compatibilite; }
QString RecommandationPlante::saison() const { return m_saison; }

void RecommandationPlante::setIdPlante(int value) { m_idPlante = value; }
void RecommandationPlante::setTypeSol(const QString &value) { m_typeSol = value; }
void RecommandationPlante::setRecommandation(const QString &value) { m_recommandation = value; }
void RecommandationPlante::setNomPlante(const QString &value) { m_nomPlante = value; }
void RecommandationPlante::setCompatibilite(const QString &value) { m_compatibilite = value; }
void RecommandationPlante::setSaison(const QString &value) { m_saison = value; }

QSqlDatabase RecommandationPlante::database()
{
    return resolveDb();
}

QStringList RecommandationPlante::typesDesolDisponibles()
{
    return QStringList{"Argileux", "Sableux", "Limoneux", "Calcaire", "Humifère"};
}

QStringList RecommandationPlante::compatibilitesDisponibles()
{
    return QStringList{"Excellente", "Bonne", "Moyenne", "Faible"};
}

QStringList RecommandationPlante::saisonsDisponibles()
{
    return QStringList{"Printemps", "Été", "Automne", "Hiver"};
}

int RecommandationPlante::getNextId()
{
    QSqlDatabase db = database();
    if (!db.isOpen()) return 1;
    
    QSqlQuery query(db);
    if (query.exec("SELECT NVL(MAX(ID_PLANTE), 0) + 1 FROM RECOMMANDATION_PLANTE") && query.next()) {
        return query.value(0).toInt();
    }
    return 1;
}

// Données de plantes prédéfinies pour la génération aléatoire
struct PlantData {
    QString nomPlante;
    QString recommandation;
    QString compatibilite;
    QString saison;
};

static QMap<QString, QList<PlantData>> getPlantesParTypeSol()
{
    QMap<QString, QList<PlantData>> plantes;
    
    // Sol Argileux
    plantes["Argileux"] = {
        {"Rose", "Idéale pour sols argileux, nécessite un bon drainage", "Excellente", "Printemps"},
        {"Iris", "Supporte bien l'humidité des sols argileux", "Bonne", "Automne"},
        {"Saule", "Arbre parfait pour sols lourds et humides", "Excellente", "Printemps"},
        {"Aster", "Fleur robuste adaptée aux sols lourds", "Bonne", "Automne"},
        {"Hosta", "Plante d'ombre excellente pour sol argileux", "Excellente", "Printemps"},
        {"Pivoine", "Fleur majestueuse qui prospère en sol argileux", "Excellente", "Printemps"}
    };
    
    // Sol Sableux
    plantes["Sableux"] = {
        {"Lavande", "Excellent drainage, supporte la sécheresse", "Excellente", "Printemps"},
        {"Romarin", "Aromatique méditerranéen idéal", "Excellente", "Printemps"},
        {"Cactus", "Parfait pour sols très drainants", "Excellente", "Été"},
        {"Thym", "Aromatique résistant à la sécheresse", "Excellente", "Printemps"},
        {"Olivier", "Arbre méditerranéen pour sol bien drainé", "Bonne", "Printemps"},
        {"Agave", "Plante succulente idéale pour sol sableux", "Excellente", "Été"}
    };
    
    // Sol Limoneux
    plantes["Limoneux"] = {
        {"Tomate", "Sol idéal pour les légumes", "Excellente", "Printemps"},
        {"Carotte", "Légume racine parfait pour sol meuble", "Excellente", "Printemps"},
        {"Fraisier", "Fruit délicieux en sol fertile", "Bonne", "Printemps"},
        {"Laitue", "Légume feuille qui adore le sol limoneux", "Excellente", "Printemps"},
        {"Pomme de terre", "Tubercule parfait pour ce type de sol", "Excellente", "Printemps"},
        {"Courgette", "Légume productif en sol riche", "Excellente", "Été"}
    };
    
    // Sol Calcaire
    plantes["Calcaire"] = {
        {"Clématite", "Grimpante qui aime le calcaire", "Excellente", "Automne"},
        {"Lilas", "Arbuste parfumé pour sols alcalins", "Excellente", "Printemps"},
        {"Buis", "Arbuste persistant classique", "Bonne", "Printemps"},
        {"Forsythia", "Arbuste à floraison jaune printanière", "Bonne", "Printemps"},
        {"Lavande", "Aime les sols calcaires et secs", "Excellente", "Printemps"},
        {"Géranium", "Fleur résistante pour sol alcalin", "Bonne", "Été"}
    };
    
    // Sol Humifère
    plantes["Humifère"] = {
        {"Hortensia", "Adore les sols riches en humus", "Excellente", "Printemps"},
        {"Rhododendron", "Arbuste acidophile parfait", "Excellente", "Automne"},
        {"Fougère", "Plante d'ombre pour sol humide", "Excellente", "Printemps"},
        {"Azalée", "Arbuste coloré pour sol acide", "Excellente", "Printemps"},
        {"Camélia", "Arbuste à fleurs pour sol humifère", "Excellente", "Automne"},
        {"Magnolia", "Arbre majestueux pour sol riche", "Bonne", "Printemps"}
    };
    
    return plantes;
}

RecommandationPlante RecommandationPlante::genererRecommandationAleatoire(const QString &typeSol)
{
    static QMap<QString, QList<PlantData>> plantesParSol = getPlantesParTypeSol();
    
    QString solNormalise = typeSol;
    // Normaliser le type de sol
    for (const QString &sol : typesDesolDisponibles()) {
        if (typeSol.contains(sol, Qt::CaseInsensitive)) {
            solNormalise = sol;
            break;
        }
    }
    
    QList<PlantData> plantesDisponibles = plantesParSol.value(solNormalise);
    
    if (plantesDisponibles.isEmpty()) {
        // Si pas de plantes spécifiques, utiliser des plantes génériques
        plantesDisponibles = {
            {"Plante adaptée", "Recommandation générique pour ce type de sol", "Moyenne", "Printemps"}
        };
    }
    
    // Sélection aléatoire
    int index = QRandomGenerator::global()->bounded(plantesDisponibles.size());
    const PlantData &plante = plantesDisponibles[index];
    
    int nextId = getNextId();
    
    return RecommandationPlante(nextId, solNormalise, plante.recommandation, 
                                 plante.nomPlante, plante.compatibilite, plante.saison);
}

QList<RecommandationPlante> RecommandationPlante::genererRecommandationsAleatoires(const QString &typeSol, int count)
{
    QList<RecommandationPlante> recommandations;
    QSet<QString> plantesUtilisees;
    
    static QMap<QString, QList<PlantData>> plantesParSol = getPlantesParTypeSol();
    
    QString solNormalise = typeSol;
    for (const QString &sol : typesDesolDisponibles()) {
        if (typeSol.contains(sol, Qt::CaseInsensitive)) {
            solNormalise = sol;
            break;
        }
    }
    
    QList<PlantData> plantesDisponibles = plantesParSol.value(solNormalise);
    int maxCount = qMin(count, plantesDisponibles.size());
    
    int nextId = getNextId();
    
    while (recommandations.size() < maxCount && plantesUtilisees.size() < plantesDisponibles.size()) {
        int index = QRandomGenerator::global()->bounded(plantesDisponibles.size());
        const PlantData &plante = plantesDisponibles[index];
        
        if (!plantesUtilisees.contains(plante.nomPlante)) {
            plantesUtilisees.insert(plante.nomPlante);
            recommandations.append(RecommandationPlante(nextId++, solNormalise, plante.recommandation,
                                                         plante.nomPlante, plante.compatibilite, plante.saison));
        }
    }
    
    return recommandations;
}

bool RecommandationPlante::ajouter(QString *errorText) const
{
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, RECOMMANDATION, NOM_PLANTE, COMPATIBILITE, SAISON) "
        "VALUES (:id, :typeSol, :reco, :nomPlante, :compat, :saison)"));
    query.bindValue(QStringLiteral(":id"), m_idPlante);
    query.bindValue(QStringLiteral(":typeSol"), m_typeSol);
    query.bindValue(QStringLiteral(":reco"), m_recommandation);
    query.bindValue(QStringLiteral(":nomPlante"), m_nomPlante.isEmpty() ? QVariant() : m_nomPlante);
    query.bindValue(QStringLiteral(":compat"), m_compatibilite);
    query.bindValue(QStringLiteral(":saison"), m_saison.isEmpty() ? QVariant() : m_saison);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return false;
    }
    return true;
}

bool RecommandationPlante::modifier(int oldId, QString *errorText) const
{
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "UPDATE RECOMMANDATION_PLANTE SET ID_PLANTE = :newId, TYPE_SOL = :typeSol, "
        "RECOMMANDATION = :reco, NOM_PLANTE = :nomPlante, COMPATIBILITE = :compat, SAISON = :saison "
        "WHERE ID_PLANTE = :oldId"));
    query.bindValue(QStringLiteral(":newId"), m_idPlante);
    query.bindValue(QStringLiteral(":typeSol"), m_typeSol);
    query.bindValue(QStringLiteral(":reco"), m_recommandation);
    query.bindValue(QStringLiteral(":nomPlante"), m_nomPlante.isEmpty() ? QVariant() : m_nomPlante);
    query.bindValue(QStringLiteral(":compat"), m_compatibilite);
    query.bindValue(QStringLiteral(":saison"), m_saison.isEmpty() ? QVariant() : m_saison);
    query.bindValue(QStringLiteral(":oldId"), oldId);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return false;
    }
    return true;
}

bool RecommandationPlante::supprimer(int id, QString *errorText)
{
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM RECOMMANDATION_PLANTE WHERE ID_PLANTE = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return false;
    }
    return true;
}

bool RecommandationPlante::idExists(int id)
{
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM RECOMMANDATION_PLANTE WHERE ID_PLANTE = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

QList<RecommandationPlante> RecommandationPlante::fetchAll(QString *errorText)
{
    QList<RecommandationPlante> list;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return list;
    }

    QSqlQuery query(db);
    if (!query.exec(QStringLiteral(
        "SELECT ID_PLANTE, TYPE_SOL, RECOMMANDATION, NOM_PLANTE, COMPATIBILITE, SAISON "
        "FROM RECOMMANDATION_PLANTE ORDER BY TYPE_SOL, COMPATIBILITE, ID_PLANTE"))) {
        if (errorText) *errorText = query.lastError().text();
        return list;
    }

    while (query.next()) {
        RecommandationPlante rp(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toString()
        );
        list.append(rp);
    }
    return list;
}

QList<RecommandationPlante> RecommandationPlante::fetchByTypeSol(const QString &typeSol, QString *errorText)
{
    QList<RecommandationPlante> list;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        if (errorText) *errorText = QStringLiteral("Base de données non connectée");
        return list;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT ID_PLANTE, TYPE_SOL, RECOMMANDATION, NOM_PLANTE, COMPATIBILITE, SAISON "
        "FROM RECOMMANDATION_PLANTE "
        "WHERE UPPER(TYPE_SOL) LIKE UPPER(:typeSol) ORDER BY COMPATIBILITE, ID_PLANTE"));
    query.bindValue(QStringLiteral(":typeSol"), QStringLiteral("%%1%").arg(typeSol));

    if (!query.exec()) {
        if (errorText) *errorText = query.lastError().text();
        return list;
    }

    while (query.next()) {
        RecommandationPlante rp(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toString()
        );
        list.append(rp);
    }
    return list;
}
