#include "maison.h"

Maison::Maison() : id_maison(0), nbr_pieces(0) {}

Maison::Maison(QString adresse, QString statut, QString securite, int nbr_pieces, QString type)
{
    this->adresse = adresse.trimmed();
    this->statut = statut.trimmed();
    this->securite = securite.trimmed();
    this->nbr_pieces = nbr_pieces;
    this->type = type.trimmed();
}

// ✅ ADD
bool Maison::ajouter(int id_maison, QString *errorMessage)
{
    QSqlQuery query;
    QString lastError;
    bool useProvidedId = (id_maison > 0); // Si ID fourni et > 0, l'utiliser
    
    // Debug: Afficher les valeurs avant insertion
    qDebug() << "🔍 [DEBUG] Valeurs à insérer:";
    qDebug() << "  - ID maison:" << (useProvidedId ? QString::number(id_maison) : "AUTO");
    qDebug() << "  - Adresse:" << adresse;
    qDebug() << "  - Statut:" << statut;
    qDebug() << "  - Sécurité:" << securite;
    qDebug() << "  - Nbr pièces:" << nbr_pieces;
    qDebug() << "  - Type:" << type;
    
    // Si un ID est fourni, vérifier qu'il n'existe pas déjà
    if (useProvidedId) {
        if (idExists(id_maison)) {
            QString errorMsg = QString("L'ID %1 existe déjà dans la base de données. Veuillez utiliser un autre ID.").arg(id_maison);
            qDebug() << "❌" << errorMsg;
            if (errorMessage) *errorMessage = errorMsg;
            return false;
        }
    }
    
    int finalId = id_maison;
    
    // Si aucun ID n'est fourni, générer automatiquement
    if (!useProvidedId) {
        // Essayer d'abord avec la séquence (comme pour VEHICULES)
        query.prepare("INSERT INTO \"SYSTEM\".\"MAISON\" "
                      "(\"ID_MAISON\", \"ADRESSE\", \"STATUT\", \"SECURITE\", \"NBR_PIECES\", \"TYPE\") "
                      "VALUES (SEQ_MAISON.NEXTVAL, :ADRESSE, :STATUT, :SECURITE, :NBR_PIECES, :TYPE)");

        query.bindValue(":ADRESSE", adresse);
        query.bindValue(":STATUT", statut);
        query.bindValue(":SECURITE", securite);
        query.bindValue(":NBR_PIECES", nbr_pieces);
        query.bindValue(":TYPE", type);
        
        qDebug() << "🔍 [DEBUG] Valeurs bindées (avec séquence):";
        qDebug() << "  - :ADRESSE =" << query.boundValue(":ADRESSE");
        qDebug() << "  - :STATUT =" << query.boundValue(":STATUT");
        qDebug() << "  - :SECURITE =" << query.boundValue(":SECURITE");
        qDebug() << "  - :NBR_PIECES =" << query.boundValue(":NBR_PIECES");
        qDebug() << "  - :TYPE =" << query.boundValue(":TYPE");

        if (!query.exec()) {
            QString errorText = query.lastError().text();
            lastError = errorText;
            qDebug() << "❌ SQL Error (ajouter avec séquence):" << errorText;
            
            // Si la séquence n'existe pas, calculer le prochain ID manuellement
            if (errorText.contains("SEQ_MAISON", Qt::CaseInsensitive) || 
                errorText.contains("sequence", Qt::CaseInsensitive) ||
                errorText.contains("does not exist", Qt::CaseInsensitive)) {
                
                qDebug() << "⚠️ Séquence non trouvée, calcul du prochain ID...";
                
                // Récupérer le MAX(ID_MAISON) + 1
                query.clear();
                if (query.exec("SELECT NVL(MAX(\"ID_MAISON\"), 0) + 1 FROM \"SYSTEM\".\"MAISON\"")) {
                    if (query.next()) {
                        finalId = query.value(0).toInt();
                        qDebug() << "✅ Prochain ID calculé:" << finalId;
                    } else {
                        finalId = 1; // Par défaut si la table est vide
                    }
                } else {
                    finalId = 1; // Par défaut en cas d'erreur
                }
            } else {
                qDebug() << "❌ SQL Error (ajouter):" << errorText;
                qDebug() << "Requête:" << query.lastQuery();
                if (errorMessage) *errorMessage = errorText;
                return false;
            }
        } else {
            // Succès avec la séquence
            qDebug() << "✅ Maison ajoutée avec succès (séquence)";
            return true;
        }
    }
    
    // Utiliser l'ID fourni ou calculé
    query.clear();
    query.prepare("INSERT INTO \"SYSTEM\".\"MAISON\" "
                  "(\"ID_MAISON\", \"ADRESSE\", \"STATUT\", \"SECURITE\", \"NBR_PIECES\", \"TYPE\") "
                  "VALUES (:ID_MAISON, :ADRESSE, :STATUT, :SECURITE, :NBR_PIECES, :TYPE)");
    
    query.bindValue(":ID_MAISON", finalId);
    query.bindValue(":ADRESSE", adresse);
    query.bindValue(":STATUT", statut);
    query.bindValue(":SECURITE", securite);
    query.bindValue(":NBR_PIECES", nbr_pieces);
    query.bindValue(":TYPE", type);
    
    qDebug() << "🔍 [DEBUG] Valeurs bindées (avec ID):";
    qDebug() << "  - :ID_MAISON =" << query.boundValue(":ID_MAISON");
    qDebug() << "  - :ADRESSE =" << query.boundValue(":ADRESSE");
    qDebug() << "  - :STATUT =" << query.boundValue(":STATUT");
    qDebug() << "  - :SECURITE =" << query.boundValue(":SECURITE");
    qDebug() << "  - :NBR_PIECES =" << query.boundValue(":NBR_PIECES");
    qDebug() << "  - :TYPE =" << query.boundValue(":TYPE");
    
    if (!query.exec()) {
        lastError = query.lastError().text();
        qDebug() << "❌ SQL Error (ajouter avec ID):" << lastError;
        qDebug() << "Requête:" << query.lastQuery();
        if (errorMessage) *errorMessage = lastError;
        return false;
    }
    
    // Vérifier que l'insertion a bien fonctionné en récupérant la dernière ligne insérée
    qDebug() << "✅ Maison ajoutée avec succès";
    
    // Vérification: récupérer la dernière ligne insérée pour confirmer
    query.clear();
    if (query.exec("SELECT \"ID_MAISON\", \"ADRESSE\", \"STATUT\", \"SECURITE\", \"NBR_PIECES\", \"TYPE\" "
                   "FROM \"SYSTEM\".\"MAISON\" WHERE ROWNUM = 1 ORDER BY \"ID_MAISON\" DESC")) {
        if (query.next()) {
            qDebug() << "✅ [VERIFICATION] Dernière ligne insérée:";
            qDebug() << "  - ID:" << query.value(0).toString();
            qDebug() << "  - Adresse:" << query.value(1).toString();
            qDebug() << "  - Statut:" << query.value(2).toString();
            qDebug() << "  - Sécurité:" << query.value(3).toString();
            qDebug() << "  - Nbr pièces:" << query.value(4).toString();
            qDebug() << "  - Type:" << query.value(5).toString();
        }
    }
    
    return true;
}

// ✅ UPDATE
bool Maison::modifier(int oldId, int newId, QString *errorMessage)
{
    QSqlQuery query;
    
    // Debug: Afficher les valeurs avant modification
    qDebug() << "🔍 [DEBUG] Valeurs à modifier:";
    qDebug() << "  - Ancien ID (WHERE):" << oldId;
    qDebug() << "  - Nouvel ID:" << (newId > 0 ? QString::number(newId) : "Garde l'ancien");
    qDebug() << "  - Adresse:" << adresse;
    qDebug() << "  - Statut:" << statut;
    qDebug() << "  - Sécurité:" << securite;
    qDebug() << "  - Nbr pièces:" << nbr_pieces;
    qDebug() << "  - Type:" << type;
    
    // Si newId > 0 et différent de oldId, vérifier qu'il n'existe pas déjà
    if (newId > 0 && newId != oldId) {
        if (idExists(newId)) {
            QString errorMsg = QString("L'ID %1 existe déjà dans la base de données. Veuillez utiliser un autre ID.").arg(newId);
            qDebug() << "❌" << errorMsg;
            if (errorMessage) *errorMessage = errorMsg;
            return false;
        }
    }
    
    // Si newId > 0, on met à jour aussi l'ID, sinon on garde l'ancien
    if (newId > 0 && newId != oldId) {
        // Mettre à jour l'ID aussi
        query.prepare("UPDATE \"SYSTEM\".\"MAISON\" SET "
                      "\"ID_MAISON\" = :NEW_ID, "
                      "\"ADRESSE\" = :ADRESSE, "
                      "\"STATUT\" = :STATUT, "
                      "\"SECURITE\" = :SECURITE, "
                      "\"NBR_PIECES\" = :NBR_PIECES, "
                      "\"TYPE\" = :TYPE "
                      "WHERE \"ID_MAISON\" = :OLD_ID");
        
        query.bindValue(":NEW_ID", newId);
        query.bindValue(":OLD_ID", oldId);
    } else {
        // Garder l'ancien ID
        query.prepare("UPDATE \"SYSTEM\".\"MAISON\" SET "
                      "\"ADRESSE\" = :ADRESSE, "
                      "\"STATUT\" = :STATUT, "
                      "\"SECURITE\" = :SECURITE, "
                      "\"NBR_PIECES\" = :NBR_PIECES, "
                      "\"TYPE\" = :TYPE "
                      "WHERE \"ID_MAISON\" = :OLD_ID");
        
        query.bindValue(":OLD_ID", oldId);
    }

    query.bindValue(":ADRESSE", adresse);
    query.bindValue(":STATUT", statut);
    query.bindValue(":SECURITE", securite);
    query.bindValue(":NBR_PIECES", nbr_pieces);
    query.bindValue(":TYPE", type);
    
    qDebug() << "🔍 [DEBUG] Valeurs bindées:";
    if (newId > 0 && newId != oldId) {
        qDebug() << "  - :NEW_ID =" << query.boundValue(":NEW_ID");
    }
    qDebug() << "  - :OLD_ID =" << query.boundValue(":OLD_ID");
    qDebug() << "  - :ADRESSE =" << query.boundValue(":ADRESSE");
    qDebug() << "  - :STATUT =" << query.boundValue(":STATUT");
    qDebug() << "  - :SECURITE =" << query.boundValue(":SECURITE");
    qDebug() << "  - :NBR_PIECES =" << query.boundValue(":NBR_PIECES");
    qDebug() << "  - :TYPE =" << query.boundValue(":TYPE");

    if (!query.exec()) {
        QString errorText = query.lastError().text();
        qDebug() << "❌ SQL Error (modifier):" << errorText;
        qDebug() << "Requête:" << query.lastQuery();
        if (errorMessage) *errorMessage = errorText;
        return false;
    }
    
    // Vérifier que la modification a bien fonctionné (utiliser le nouvel ID si changé)
    int checkId = (newId > 0 && newId != oldId) ? newId : oldId;
    query.clear();
    if (query.exec(QString("SELECT \"ID_MAISON\", \"ADRESSE\", \"STATUT\", \"SECURITE\", \"NBR_PIECES\", \"TYPE\" "
                           "FROM \"SYSTEM\".\"MAISON\" WHERE \"ID_MAISON\" = %1").arg(checkId))) {
        if (query.next()) {
            qDebug() << "✅ [VERIFICATION] Maison modifiée:";
            qDebug() << "  - ID:" << query.value(0).toString();
            qDebug() << "  - Adresse:" << query.value(1).toString();
            qDebug() << "  - Statut:" << query.value(2).toString();
            qDebug() << "  - Sécurité:" << query.value(3).toString();
            qDebug() << "  - Nbr pièces:" << query.value(4).toString();
            qDebug() << "  - Type:" << query.value(5).toString();
        }
    }
    
    qDebug() << "✅ Maison modifiée avec succès";
    return true;
}

// ✅ DELETE
bool Maison::supprimer(int id, QString *errorMessage)
{
    QSqlQuery query;
    query.prepare("DELETE FROM \"SYSTEM\".\"MAISON\" WHERE \"ID_MAISON\" = :ID");
    query.bindValue(":ID", id);

    if (!query.exec()) {
        QString errorText = query.lastError().text();
        qDebug() << "❌ SQL Error (supprimer):" << errorText;
        if (errorMessage) *errorMessage = errorText;
        return false;
    }
    qDebug() << "✅ Maison supprimée avec succès";
    return true;
}

// ✅ DISPLAY
void Maison::afficher(QTableWidget *table)
{
    if (!table) {
        qDebug() << "❌ Table widget is null";
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT \"ID_MAISON\", \"ADRESSE\", \"STATUT\", \"SECURITE\", \"NBR_PIECES\", \"TYPE\" "
                  "FROM \"SYSTEM\".\"MAISON\" ORDER BY \"ID_MAISON\" DESC");

    if (!query.exec()) {
        qDebug() << "❌ SQL Error (afficher):" << query.lastError().text();
        return;
    }

    table->setRowCount(0);
    table->setColumnCount(6);

    QStringList headers = {"ID maison", "Adresse", "Statut", "Sécurité", "Nbr pièces", "Type"};
    table->setHorizontalHeaderLabels(headers);

    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < 6; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            table->setItem(row, col, item);
        }
        row++;
    }

    qDebug() << "✅ Données MAISON chargées:" << row << "lignes";
}

// Validation methods
bool Maison::validateAdresse(const QString &adresse, QString &error)
{
    QString trimmed = adresse.trimmed();
    if (trimmed.isEmpty()) {
        error = "L'adresse ne peut pas être vide.";
        return false;
    }
    if (trimmed.length() > 20) {
        error = "L'adresse ne peut pas dépasser 20 caractères.";
        return false;
    }
    return true;
}

bool Maison::validateStatut(const QString &statut, QString &error)
{
    QString trimmed = statut.trimmed();
    if (trimmed.isEmpty()) {
        error = "Le statut ne peut pas être vide.";
        return false;
    }
    QStringList validStatuts = {"occupé", "vide", "en maintenance"};
    if (!validStatuts.contains(trimmed, Qt::CaseInsensitive)) {
        error = "Le statut doit être: occupé, vide, ou en maintenance.";
        return false;
    }
    return true;
}

bool Maison::validateSecurite(const QString &securite, QString &error)
{
    QString trimmed = securite.trimmed();
    if (trimmed.isEmpty()) {
        error = "Le niveau de sécurité ne peut pas être vide.";
        return false;
    }
    if (trimmed.length() > 20) {
        error = "Le niveau de sécurité ne peut pas dépasser 20 caractères.";
        return false;
    }
    return true;
}

bool Maison::validateNbrPieces(int nbr_pieces, QString &error)
{
    if (nbr_pieces < 0) {
        error = "Le nombre de pièces ne peut pas être négatif.";
        return false;
    }
    if (nbr_pieces > 20) {
        error = "Le nombre de pièces ne peut pas dépasser 20.";
        return false;
    }
    return true;
}

bool Maison::validateType(const QString &type, QString &error)
{
    QString trimmed = type.trimmed();
    if (trimmed.isEmpty()) {
        error = "Le type ne peut pas être vide.";
        return false;
    }
    QStringList validTypes = {"App", "Villa"};
    if (!validTypes.contains(trimmed, Qt::CaseInsensitive)) {
        error = "Le type doit être: App ou Villa.";
        return false;
    }
    return true;
}

// Vérifie si un ID existe déjà dans la base de données
bool Maison::idExists(int id)
{
    QSqlQuery query;
    query.prepare("SELECT 1 FROM \"SYSTEM\".\"MAISON\" WHERE \"ID_MAISON\" = :ID");
    query.bindValue(":ID", id);
    
    if (query.exec() && query.next()) {
        qDebug() << "⚠️ ID" << id << "existe déjà dans la base de données";
        return true;
    }
    
    return false;
}
