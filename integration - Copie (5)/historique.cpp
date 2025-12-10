#include "historique.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

namespace {
const char *kTableName = "HIST_RESIDENT";
}

Historique::Historique()
    : m_dateAction(QDateTime::currentDateTime())
{
}

Historique::Historique(const QString &idHistorique, const QString &idResident, const QString &action, const QDateTime &dateAction)
    : m_idHistorique(idHistorique),
      m_idResident(idResident),
      m_action(action),
      m_dateAction(dateAction)
{
}

int Historique::obtenirProchainIdHistorique()
{
    // Essayer d'utiliser une séquence Oracle si elle existe
    QSqlQuery query;
    // Essayer d'abord avec le nom de la nouvelle séquence
    query.prepare("SELECT HISTORIQUE_RESIDENT_SEQ.NEXTVAL FROM DUAL");
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    // Essayer avec l'ancien nom de séquence pour compatibilité
    query.prepare("SELECT HIST_RESIDENT_SEQ.NEXTVAL FROM DUAL");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    // Si pas de séquence, utiliser le max + 1
    query.prepare(QStringLiteral("SELECT NVL(MAX(ID_HISTORIQUE), 0) + 1 FROM %1").arg(kTableName));
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    // Fallback : utiliser timestamp
    return static_cast<int>(QDateTime::currentMSecsSinceEpoch() % 2147483647);
}

bool Historique::ajouter(const Historique &historique)
{
    QSqlQuery query;
    int idHist = 0;
    
    // Si l'ID n'est pas fourni, générer un nouveau
    if (historique.idHistorique().isEmpty()) {
        idHist = obtenirProchainIdHistorique();
    } else {
        bool ok;
        idHist = historique.idHistorique().toInt(&ok);
        if (!ok) {
            idHist = obtenirProchainIdHistorique();
        }
    }
    
    // Convertir ID_RESIDENT en nombre
    bool ok;
    int idResident = historique.idResident().toInt(&ok);
    if (!ok) {
        qWarning() << "Historique::ajouter - ID_RESIDENT invalide:" << historique.idResident();
        return false;
    }
    
    query.prepare(QStringLiteral(
        "INSERT INTO %1 (ID_HISTORIQUE, ID_RESIDENT, ACTION, DATE_ACTION) "
        "VALUES (:id_historique, :id_resident, :action, :date_action)")
                      .arg(kTableName));
    query.bindValue(":id_historique", idHist);
    query.bindValue(":id_resident", idResident);
    query.bindValue(":action", historique.action());
    query.bindValue(":date_action", historique.dateAction());

    if (!query.exec()) {
        qWarning() << "Historique::ajouter - erreur SQL:" << query.lastError().text();
        qWarning() << "Requête:" << query.lastQuery();
        qWarning() << "Valeurs - ID_HISTORIQUE:" << idHist << "ID_RESIDENT:" << idResident << "ACTION:" << historique.action();
        return false;
    }
    qDebug() << "Historique ajouté avec succès - ID_HISTORIQUE:" << idHist << "ID_RESIDENT:" << idResident << "ACTION:" << historique.action();
    return true;
}

QList<Historique> Historique::recupererTout()
{
    QList<Historique> historiques;
    QSqlQuery query;
    
    // Essayer plusieurs variantes de noms de table
    QStringList nomsTables = {
        "HIST_RESIDENT",           // Nom utilisé dans le code
        "HIBA.HIST_RESIDENT",      // Avec schéma
        "HISTORIQUE_RESIDENT",     // Nom complet
        "HIBA.HISTORIQUE_RESIDENT" // Avec schéma complet
    };
    
    bool succes = false;
    QString derniereErreur;
    
    for (const QString &nomTable : nomsTables) {
        QString requete = QString("SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, DATE_ACTION FROM %1 ORDER BY DATE_ACTION DESC").arg(nomTable);
        qDebug() << "Historique::recupererTout - Tentative avec:" << nomTable;
        qDebug() << "Requête:" << requete;
        
        if (query.exec(requete)) {
            qDebug() << "✓ Succès avec:" << nomTable;
            succes = true;
            break;
        } else {
            derniereErreur = query.lastError().text();
            qWarning() << "✗ Échec avec" << nomTable << ":" << derniereErreur;
            qWarning() << "Code d'erreur:" << query.lastError().nativeErrorCode();
        }
    }
    
    if (!succes) {
        qWarning() << "Historique::recupererTout - Impossible de trouver la table d'historique!";
        qWarning() << "Tables essayées:" << nomsTables.join(", ");
        qWarning() << "Dernière erreur:" << derniereErreur;
        qWarning() << "";
        qWarning() << "VÉRIFICATIONS À FAIRE:";
        qWarning() << "1. La table HIST_RESIDENT ou HISTORIQUE_RESIDENT existe-t-elle?";
        qWarning() << "2. L'utilisateur connecté a-t-il les permissions?";
        qWarning() << "3. Le schéma est-il correct (HIBA)?";
        qWarning() << "4. Exécutez le script: diagnostic_historique.sql";
        return historiques;
    }

    int count = 0;
    while (query.next()) {
        Historique h;
        h.m_idHistorique = query.value(0).toString();
        h.m_idResident = query.value(1).toString();
        QString actionBase = query.value(2).toString();
        // Mapper les valeurs de la base vers l'interface
        if (actionBase == "Ajout") {
            h.m_action = "Ajouter";
        } else if (actionBase == "Modification") {
            h.m_action = "Modifier";
        } else if (actionBase == "Suppression") {
            h.m_action = "Supprimer";
        } else {
            h.m_action = actionBase; // Garder la valeur originale si non mappée
        }
        h.m_dateAction = query.value(3).toDateTime();
        
        qDebug() << "Historique trouvé - ID:" << h.m_idHistorique << "ID_Resident:" << h.m_idResident << "Action (base):" << actionBase << "-> (interface):" << h.m_action;
        historiques.append(h);
        count++;
    }
    
    qDebug() << "Historique::recupererTout - Total trouvé:" << count;
    return historiques;
}

QList<Historique> Historique::filtrerParAction(const QString &action)
{
    if (action.isEmpty() || action == "Tout") {
        return recupererTout();
    }
    
    // Mapper les valeurs de la combobox aux valeurs de la base
    QString actionBase = action;
    if (action == "Ajouter") {
        actionBase = "Ajout";
    } else if (action == "Modifier") {
        actionBase = "Modification";
    } else if (action == "Supprimer") {
        actionBase = "Suppression";
    }
    
    QList<Historique> historiques;
    QSqlQuery query;
    
    // Essayer plusieurs variantes de noms de table
    QStringList nomsTables = {
        "HIST_RESIDENT",
        "HIBA.HIST_RESIDENT",
        "HISTORIQUE_RESIDENT",
        "HIBA.HISTORIQUE_RESIDENT"
    };
    
    bool succes = false;
    QString derniereErreur;
    
    for (const QString &nomTable : nomsTables) {
        QString requete = QString("SELECT ID_HISTORIQUE, ID_RESIDENT, ACTION, DATE_ACTION FROM %1 WHERE ACTION = :action ORDER BY DATE_ACTION DESC").arg(nomTable);
        query.prepare(requete);
        query.bindValue(":action", actionBase);
        
        qDebug() << "Historique::filtrerParAction - Tentative avec:" << nomTable;
        qDebug() << "Filtre:" << action << "->" << actionBase;
        qDebug() << "Requête:" << requete;
        
        if (query.exec()) {
            qDebug() << "✓ Succès avec:" << nomTable;
            succes = true;
            break;
        } else {
            derniereErreur = query.lastError().text();
            qWarning() << "✗ Échec avec" << nomTable << ":" << derniereErreur;
        }
    }
    
    if (!succes) {
        qWarning() << "Historique::filtrerParAction - Impossible de trouver la table!";
        qWarning() << "Dernière erreur:" << derniereErreur;
        return historiques;
    }
    
    // Vérifier combien de lignes correspondent
    qDebug() << "Requête exécutée, vérification des résultats...";

    int count = 0;
    while (query.next()) {
        Historique h;
        h.m_idHistorique = query.value(0).toString();
        h.m_idResident = query.value(1).toString();
        QString actionBase = query.value(2).toString();
        // Mapper les valeurs de la base vers l'interface
        if (actionBase == "Ajout") {
            h.m_action = "Ajouter";
        } else if (actionBase == "Modification") {
            h.m_action = "Modifier";
        } else if (actionBase == "Suppression") {
            h.m_action = "Supprimer";
        } else {
            h.m_action = actionBase; // Garder la valeur originale si non mappée
        }
        h.m_dateAction = query.value(3).toDateTime();
        historiques.append(h);
        count++;
    }
    
    qDebug() << "Historique::filtrerParAction - Total trouvé:" << count;
    if (count == 0) {
        qDebug() << "Aucun historique trouvé pour l'action:" << actionBase;
        qDebug() << "Vérifiez que des actions de ce type existent dans la base de données.";
    }
    return historiques;
}

void Historique::vider()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("DELETE FROM %1").arg(kTableName));
    
    if (!query.exec()) {
        qWarning() << "Historique::vider - erreur SQL:" << query.lastError().text();
    }
}

