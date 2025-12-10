// ===========================================================================================
// FONCTIONS POUR CHATBOT ET RECOMMANDATIONS IA - VÉHICULES
// À ajouter à la fin de mainwindow.cpp
// ===========================================================================================

// ========== CHATBOT VÉHICULE ==========
void MainWindow::on_btnChatbotVehicule_clicked()
{
    if (ui->stackedWidget_vehicules) {
        ui->stackedWidget_vehicules->setCurrentWidget(ui->pageChatbotVehicule);
    }
}

void MainWindow::on_btnBackFromChatVehicule_clicked()
{
    if (ui->stackedWidget_vehicules) {
        ui->stackedWidget_vehicules->setCurrentWidget(ui->pageGestionVehicules);
    }
}

void MainWindow::on_btnSendChatVehicule_clicked()
{
    if (!ui->textEditChatInputVehicule || !ui->textEditChatOutputVehicule) {
        return;
    }
    
    QString userMsg = ui->textEditChatInputVehicule->toPlainText().trimmed();
    if (userMsg.isEmpty()) {
        return;
    }
    
    // Afficher le message de l'utilisateur
    QString current = ui->textEditChatOutputVehicule->toPlainText();
    current += "\n🧑 Vous: " + userMsg + "\n";
    ui->textEditChatOutputVehicule->setPlainText(current);
    ui->textEditChatInputVehicule->clear();
    
    // Traiter le message (réponse locale ou IA)
    QString botResponse = processChatMessageVehicule(userMsg);
    
    current += "🤖 SmartHelp: " + botResponse + "\n";
    ui->textEditChatOutputVehicule->setPlainText(current);
    
    // Auto-scroll vers le bas
    QTextCursor cursor = ui->textEditChatOutputVehicule->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEditChatOutputVehicule->setTextCursor(cursor);
}

QString MainWindow::processChatMessageVehicule(const QString &msg)
{
    QString m = msg.toLower();

    // --- Aide sur l'ajout ---
    if (m.contains("ajouter") && m.contains("vehicule"))
        return "Pour ajouter un véhicule : remplissez les champs à gauche puis cliquez sur 'Ajouter'.";

    // --- Aide sur la modification ---
    if (m.contains("modifier") || m.contains("update"))
        return "Pour modifier : sélectionnez un véhicule dans le tableau puis cliquez sur 'Modifier'.";

    // --- Aide sur la suppression ---
    if (m.contains("supprimer") || m.contains("delete"))
        return "Pour supprimer : sélectionnez un véhicule puis cliquez sur 'Supprimer'.";

    // --- Aide sur la recherche ---
    if (m.contains("recherche") || m.contains("chercher") || m.contains("filtrer"))
        return "Tapez une immatriculation dans la barre de recherche pour filtrer les résultats.";

    // --- Aide sur l'état ---
    if (m.contains("etat") || m.contains("panne") || m.contains("use") || m.contains("neuf"))
        return "Les états disponibles sont : Neuf, Use, En panne.";

    // --- Aide sur la date de maintenance ---
    if (m.contains("date") || m.contains("maintenance") || m.contains("maint"))
        return "La date de maintenance doit être dans le passé. Vous pouvez trier par date via le bouton 'Tri par date'.";

    // --- Aide sur le tri ---
    if (m.contains("tri") || m.contains("trier"))
        return "Le bouton 'Tri par date' permet de trier les véhicules du plus ancien au plus récent et inversement.";

    // --- Message par défaut ---
    return "Je n'ai pas compris 😅. Essayez avec des mots comme : ajouter véhicule, modifier, supprimer, tri, recherche, date maintenance.";
}

// ========== RECOMMANDATIONS IA VÉHICULE ==========
void MainWindow::on_btnRecommandationVehicule_clicked()
{
    qDebug() << "🔎 [DEBUG] on_btnRecommandationVehicule_clicked() triggered";
    qDebug() << "🔎 [DEBUG] selectedImmatVehicule current value:" << selectedImmatVehicule;

    if (selectedImmatVehicule.isEmpty()) {
        qDebug() << "⚠️ [DEBUG] No vehicle selected before recommendation!";
        QMessageBox::warning(this,
                             "Erreur",
                             "Veuillez d'abord sélectionner un véhicule avant de demander une recommandation.");
        return;
    }

    // Construire le message pour l'IA à partir du véhicule courant
    QString prompt = buildMaintenancePromptFromCurrentVehicule();
    qDebug() << "📨 [DEBUG] Maintenance prompt envoyé à Azure :" << prompt;

    // Nettoyer la zone avant la réponse
    if (ui->textEditRecommandationVehicule) {
        ui->textEditRecommandationVehicule->clear();
        ui->textEditRecommandationVehicule->setPlainText("Génération de la recommandation en cours...");
    }

    // Lancer l'appel Azure
    sendRecommendationToAzureAI(prompt);

    // Afficher la page de recommandation
    if (ui->stackedWidget_vehicules) {
        ui->stackedWidget_vehicules->setCurrentWidget(ui->pageRecommandationVehicule);
    }
}

void MainWindow::on_btnBackFromRecommandationVehicule_clicked()
{
    if (ui->stackedWidget_vehicules) {
        ui->stackedWidget_vehicules->setCurrentWidget(ui->pageGestionVehicules);
    }
}

QString MainWindow::buildMaintenancePromptFromCurrentVehicule() const
{
    QString immat   = ui->immatline_2 ? ui->immatline_2->text() : "";
    QString marque  = ui->marqueline_2 ? ui->marqueline_2->text() : "";
    QString modele  = ui->modeleline_2 ? ui->modeleline_2->text() : "";
    QString type    = ui->triemail_2 ? ui->triemail_2->currentText() : "";
    QString etat    = ui->Etatline_2 ? ui->Etatline_2->currentText() : "";
    QString service = ui->serviceline_2 ? ui->serviceline_2->text() : "";
    
    QDate date_maint;
    if (ui->datemaintline_2) {
        date_maint = QDate::fromString(ui->datemaintline_2->text(), "dd/MM/yyyy");
    }

    // 🔥 Get today's REAL date from system
    QString today = QDate::currentDate().toString("dd/MM/yyyy");

    QString vehiculeInfo = QString(
                               "Nous sommes le %1.\n"
                               "Données véhicule :\n"
                               "Immatriculation : %2\n"
                               "Marque : %3\n"
                               "Modèle : %4\n"
                               "Type : %5\n"
                               "État : %6\n"
                               "Service : %7\n"
                               "Date de dernière maintenance : %8\n\n"
                               "En te basant uniquement sur ces données, propose une recommandation d'entretien "
                               "pratique et cohérente avec la date du jour. Réponds en français, texte simple, "
                               "sans markdown, en maximum 10 lignes."
                               ).arg(
                                   today,
                                   immat,
                                   marque,
                                   modele,
                                   type,
                                   etat,
                                   service,
                                   date_maint.isValid() ? date_maint.toString("dd/MM/yyyy") : "inconnue"
                                   );

    return vehiculeInfo;
}

void MainWindow::sendMessageToAzureAI(const QString &message)
{
    qDebug() << "🚀 sendMessageToAzureAI() démarre (Chatbot)";

    QString endpoint   = "https://ai-kassem.cognitiveservices.azure.com";
    QString apiKey     = "445NLYUwthBdj5EbFvxbCxV2XSdJWKYartumAOvqEFMtKEofmdpuJQQJ99BGACfhMk5XJ3w3AAAAACOGRpAA";
    QString apiVersion = "2024-12-01-preview";
    QString model      = "gpt-4o";

    QString url = endpoint
                  + "/openai/deployments/"
                  + model
                  + "/chat/completions?api-version="
                  + apiVersion;

    QNetworkRequest request{ QUrl(url) };
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("api-key", apiKey.toUtf8());

    QString systemPrompt = R"(Tu es SmartHelp, assistant SmartCity pour la gestion des véhicules.
Réponds toujours en français, en texte simple, sans markdown, en maximum 5 lignes.)";

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = systemPrompt;

    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = message;

    QJsonArray arr;
    arr.append(systemMsg);
    arr.append(userMsg);

    QJsonObject payload;
    payload["messages"]    = arr;
    payload["temperature"] = 0.7;

    QByteArray jsonData = QJsonDocument(payload).toJson();
    QNetworkReply *reply = networkManagerVehicule->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "❌ ERROR Chatbot:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(data);
        QString bot = json["choices"][0]["message"]["content"].toString();

        if (ui->textEditChatOutputVehicule) {
            QString current = ui->textEditChatOutputVehicule->toPlainText();
            current += "🤖 SmartHelp IA: " + bot.trimmed() + "\n";
            ui->textEditChatOutputVehicule->setPlainText(current);
        }

        reply->deleteLater();
    });
}

void MainWindow::sendRecommendationToAzureAI(const QString &message)
{
    qDebug() << "🚀 sendRecommendationToAzureAI() démarre";

    QString endpoint   = "https://ai-kassem.cognitiveservices.azure.com";
    QString apiKey     = "445NLYUwthBdj5EbFvxbCxV2XSdJWKYartumAOvqEFMtKEofmdpuJQQJ99BGACfhMk5XJ3w3AAAAACOGRpAA";
    QString apiVersion = "2024-12-01-preview";
    QString model      = "gpt-4o";

    QString url = endpoint
                  + "/openai/deployments/"
                  + model
                  + "/chat/completions?api-version="
                  + apiVersion;

    QNetworkRequest request{ QUrl(url) };
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("api-key", apiKey.toUtf8());

    QString systemPrompt = R"(Tu es SmartHelp, assistant SmartCity.
Tu donnes des recommandations de maintenance pour les véhicules.
Réponds toujours en français, en texte simple, sans markdown, en maximum 10 lignes.)";

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = systemPrompt;

    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = message;

    QJsonArray arr;
    arr.append(systemMsg);
    arr.append(userMsg);

    QJsonObject payload;
    payload["messages"]    = arr;
    payload["temperature"] = 0.4;

    QByteArray jsonData = QJsonDocument(payload).toJson();
    QNetworkReply *reply = networkManagerVehicule->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "❌ ERROR Recommandation:" << reply->errorString();
            if (ui->textEditRecommandationVehicule) {
                ui->textEditRecommandationVehicule->setPlainText("Erreur Azure : " + reply->errorString());
            }
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(data);
        QString bot = json["choices"][0]["message"]["content"].toString();

        if (ui->textEditRecommandationVehicule) {
            ui->textEditRecommandationVehicule->setPlainText(bot.trimmed());
        }

        reply->deleteLater();
    });
}

// ========== STATISTIQUES VÉHICULE ==========
QChartView* MainWindow::createVehiculePieChart()
{
    QSqlQuery query;
    QMap<QString, int> etatCount;

    if (!query.exec("SELECT ETAT, COUNT(*) FROM GEST_VEHICULE GROUP BY ETAT")) {
        qDebug() << "❌ Erreur statistiques:" << query.lastError().text();
        return nullptr;
    }

    while (query.next()) {
        QString etat = query.value(0).toString();
        int count = query.value(1).toInt();
        etatCount[etat] = count;
    }

    QPieSeries *series = new QPieSeries();
    for (auto it = etatCount.begin(); it != etatCount.end(); ++it) {
        series->append(it.key() + " (" + QString::number(it.value()) + ")", it.value());
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des véhicules par état");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

void MainWindow::on_btnStatistiquesVehicule_clicked()
{
    QChartView *chartView = createVehiculePieChart();
    if (!chartView) {
        QMessageBox::warning(this, "Erreur", "Impossible de générer les statistiques.");
        return;
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Statistiques Véhicules");
    dialog->resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    dialog->setLayout(layout);

    dialog->exec();
}

// ========== TRI PAR DATE ==========
void MainWindow::on_btnTriDateVehicule_clicked()
{
    if (!ui->tableau_3) {
        return;
    }

    triCroissantVehicule = !triCroissantVehicule;

    QSqlQuery query;
    QString order = triCroissantVehicule ? "ASC" : "DESC";
    query.prepare("SELECT IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT "
                  "FROM GEST_VEHICULE ORDER BY DATE_MAINT " + order);

    if (!query.exec()) {
        qDebug() << "❌ Erreur tri:" << query.lastError().text();
        return;
    }

    ui->tableau_3->setRowCount(0);
    int row = 0;
    while (query.next()) {
        ui->tableau_3->insertRow(row);
        for (int col = 0; col < 7; ++col) {
            QString value = query.value(col).toString();
            if (col == 6 && !value.isEmpty()) { // Date
                QDate date = query.value(col).toDate();
                value = date.toString("dd/MM/yyyy");
            }
            ui->tableau_3->setItem(row, col, new QTableWidgetItem(value));
        }
        row++;
    }

    QString btnText = triCroissantVehicule ? "Tri Date ↑" : "Tri Date ↓";
    if (ui->btnTriDateVehicule) {
        ui->btnTriDateVehicule->setText(btnText);
    }
}

// ========== RECHERCHE VÉHICULE ==========
void MainWindow::on_lineEditRechercheVehicule_textChanged(const QString &text)
{
    if (!ui->tableau_3) {
        return;
    }

    QString searchText = text.trimmed();
    if (searchText.isEmpty()) {
        chargerVehicules();
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT "
                  "FROM GEST_VEHICULE WHERE UPPER(IMMAT) LIKE :search "
                  "ORDER BY ID_VEHI DESC");
    query.bindValue(":search", "%" + searchText.toUpper() + "%");

    if (!query.exec()) {
        qDebug() << "❌ Erreur recherche:" << query.lastError().text();
        return;
    }

    ui->tableau_3->setRowCount(0);
    int row = 0;
    while (query.next()) {
        ui->tableau_3->insertRow(row);
        for (int col = 0; col < 7; ++col) {
            QString value = query.value(col).toString();
            if (col == 6 && !value.isEmpty()) {
                QDate date = query.value(col).toDate();
                value = date.toString("dd/MM/yyyy");
            }
            ui->tableau_3->setItem(row, col, new QTableWidgetItem(value));
        }
        row++;
    }
}
