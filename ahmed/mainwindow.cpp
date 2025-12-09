#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPixmap>
#include "vehicule.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QSqlError>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>
#include <QtCharts/QLegend>
#include <QVBoxLayout>
#include <QDialog>
#include <QRandomGenerator>
//using namespace QtCharts;
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    
    // Connexions manuelles des signaux (évite les warnings connectSlotsByName)
    if (ui->tribox) {
        connect(ui->tribox, &QComboBox::currentTextChanged,
                this, &MainWindow::on_tribox_changed);
    }

    if (ui->stackedWidget && ui->page_9) {
        ui->stackedWidget->setCurrentWidget(ui->page_9);
    }
    this->resize(1920, 1080);
    this->move(0, 0);
    networkManager = new QNetworkAccessManager(this);
    qDebug() << "🌐 QNetworkAccessManager initialisé !";



    QPixmap logo(":/images/Lg.png");
    ui->logo->setPixmap(logo);
    ui->logo->setScaledContents(true);
    ui->tableau_3->setSortingEnabled(true);

    connectButtons();
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
    connect(ui->tableau_3, &QTableWidget::cellClicked, this, [this](int row, int){
        qDebug() << "🟢 [DEBUG] cellClicked fired - row =" << row;

        // Ensure there are enough columns before accessing items
        if (ui->tableau_3->columnCount() < 8) {
            qDebug() << "⚠️ [DEBUG] Table doesn't have expected columns!";
            return;
        }

        // Fill inputs
        ui->immatline_2->setText(ui->tableau_3->item(row, 1)->text());
        ui->marqueline_2->setText(ui->tableau_3->item(row, 2)->text());
        ui->modeleline_2->setText(ui->tableau_3->item(row, 3)->text());
        ui->triemail_2->setCurrentText(ui->tableau_3->item(row, 4)->text());
        ui->Etatline_2->setCurrentText(ui->tableau_3->item(row, 5)->text());
        ui->serviceline_2->setText(ui->tableau_3->item(row, 6)->text());
        QString dateStr = ui->tableau_3->item(row, 7)->text();  // get date from table
        QDate date = QDate::fromString(dateStr, "dd/MM/yyyy");  // convert to QDate
        ui->datemaintline_2->setDate(date);
        ui->datemaintline_2->setDisplayFormat("dd/MM/yyyy");


        selectedImmat = ui->tableau_3->item(row, 1)->text();
        qDebug() << "✅ [DEBUG] selectedImmat set to:" << selectedImmat;
    });

}

MainWindow::~MainWindow()
{
    delete ui;


}

void MainWindow::connectButtons()
{
    connect(ui->btnEmployes,    &QPushButton::clicked, this, &MainWindow::onGestionEmployes);
    connect(ui->btnVehicules,   &QPushButton::clicked, this, &MainWindow::onGestionVehicules);
    connect(ui->btnResidents,   &QPushButton::clicked, this, &MainWindow::onGestionResidents);
    connect(ui->btnMaisons,     &QPushButton::clicked, this, &MainWindow::onGestionMaisons);
    connect(ui->btnJardins,     &QPushButton::clicked, this, &MainWindow::onGestionJardins);
    connect(ui->btnCabinets,    &QPushButton::clicked, this, &MainWindow::onGestionCabinets);
    connect(ui->btnDeconnexion, &QPushButton::clicked, this, &MainWindow::onDeconnexion);

}

void MainWindow::onGestionEmployes()
{
    ui->stackedWidget->setCurrentWidget(ui->pageEmployes);
}

void MainWindow::onGestionVehicules()
{
    ui->stackedWidget->setCurrentWidget(ui->pageVehicules);
    loadVehicules();
}

void MainWindow::onGestionResidents()
{
    ui->stackedWidget->setCurrentWidget(ui->pageResidents);
}

void MainWindow::onGestionMaisons()
{
    ui->stackedWidget->setCurrentWidget(ui->pageMaisons);
}

void MainWindow::onGestionJardins()
{
    ui->stackedWidget->setCurrentWidget(ui->pageJardins);
}

void MainWindow::onGestionCabinets()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCabinets);
}

void MainWindow::onDeconnexion()
{
    close();
}
void MainWindow::on_btnChatbot_clicked()
{
    ui->stackedWidget_3->setCurrentWidget(ui->pageChatbot);
}

void MainWindow::on_btnBackFromChat_clicked()
{
    ui->stackedWidget_3->setCurrentWidget(ui->page_9);
}

void MainWindow::on_btnBackFromRecom_clicked()
{
    ui->stackedWidget_3->setCurrentWidget(ui->page_9);
}

void MainWindow::on_ajouter_3_clicked()
{
    QString immat = ui->immatline_2->text().trimmed();
    QString marque = ui->marqueline_2->text().trimmed();
    QString modele = ui->modeleline_2->text().trimmed();
    QString type = ui->triemail_2->currentText().trimmed();
    QString etatOriginal = ui->Etatline_2->currentText();   // valeur brute
    QString etat = etatOriginal.trimmed();                  // nettoyée
    QString service = ui->serviceline_2->text().trimmed();
    QDate date_maint = ui->datemaintline_2->date();

    // ==========================================================
    // 🔍 DEBUG : Pour afficher exactement ce que contient "etat"
    // ==========================================================
    qDebug() << "===== DEBUG ETAT =====";
    qDebug() << "ETAT (brut) =" << etatOriginal;
    qDebug() << "ETAT (trimmed) =" << etat;
    qDebug() << "Longueur (brut) =" << etatOriginal.size();
    qDebug() << "Longueur (trimmed) =" << etat.size();

    for (int i = 0; i < etatOriginal.size(); ++i) {
        qDebug() << "brut[" << i << "] =" << etatOriginal.at(i);
    }
    for (int i = 0; i < etat.size(); ++i) {
        qDebug() << "trim[" << i << "] =" << etat.at(i);
    }
    qDebug() << "========================";
    // FIN DEBUG
    // ==========================================================


    // === CONTRÔLES DE SAISIE ===
    if (immat.isEmpty() || marque.isEmpty() || modele.isEmpty() || service.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs doivent être remplis.");
        return;
    }

    // Vérif immatriculation
    QRegularExpression immatRegex("^[A-Za-z0-9\\-]{4,15}$");
    if (!immatRegex.match(immat).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Format d'immatriculation invalide (ex : 123-TN-456).");
        return;
    }

    // Vérif Type
    if (type.isEmpty() || type == "Type") {
        QMessageBox::warning(this, "Erreur", "Veuillez choisir un type de véhicule.");
        return;
    }

    // Vérif État (ComboBox) — version CORRIGÉE
    QString etatNorm = etat.toLower();  // normalisation

    if (etatNorm != "neuf" && etatNorm != "use" && etatNorm != "en panne") {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez sélectionner un état valide (Neuf, Use, En panne).");
        return;
    }


    // Vérif date
    if (!date_maint.isValid()) {
        QMessageBox::warning(this, "Erreur", "Date de maintenance invalide. Format attendu : jj/MM/aaaa");
        return;
    }
    if (date_maint > QDate::currentDate()) {
        QMessageBox::warning(this, "Erreur", "La date de maintenance ne peut pas être dans le futur.");
        return;
    }

    // Vérif doublon immat
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM VEHICULE WHERE IMMAT = :IMMAT");
    checkQuery.bindValue(":IMMAT", immat);
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Erreur", "Un véhicule avec cette immatriculation existe déjà !");
        return;
    }

    // === AJOUT FINAL ===
    Vehicule v(immat, marque, modele, type, etat, service, date_maint);

    if (v.ajouter()) {
        QMessageBox::information(this, "Succès", "Véhicule ajouté avec succès !");
        loadVehicules();
        qDebug() << "✅ Véhicule ajouté :" << immat << marque << modele << etat;
    } else {
        QMessageBox::critical(this, "Erreur SQL", "Échec de l'ajout du véhicule !");
    }
}

void MainWindow::on_supprimer_3_clicked()
{
    // Example: delete selected row from database and refresh table
    int row = ui->tableau_3->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Suppression", "Veuillez sélectionner un véhicule à supprimer.");
        return;
    }

    QString immat = ui->tableau_3->item(row, 1)->text(); // Assuming first column is immat
    Vehicule v;
    bool success = v.supprimer(immat); // Use your Vehicule::supprimer(QString) function

    if (success) {
        ui->tableau_3->removeRow(row);
        QMessageBox::information(this, "Suppression", "Véhicule supprimé avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "La suppression a échoué !");
    }
}


void MainWindow::on_modifier_3_clicked()
{
    qDebug() << "🔎 [DEBUG] on_modifier_3_clicked() triggered";
    qDebug() << "🔎 [DEBUG] selectedImmat current value:" << selectedImmat;

    if (selectedImmat.isEmpty()) {
        qDebug() << "⚠️ [DEBUG] No vehicle selected before modifying!";
        QMessageBox::warning(this, "Erreur", "Veuillez d'abord sélectionner un véhicule à modifier.");
        return;
    }

    // Read current form values
    QString immat   = ui->immatline_2->text();
    QString marque  = ui->marqueline_2->text();
    QString modele  = ui->modeleline_2->text();
    QString type    = ui->triemail_2->currentText();
    QString etat    = ui->Etatline_2->currentText();
    QString service = ui->serviceline_2->text();
    QDate   date_maint = QDate::fromString(ui->datemaintline_2->text(), "dd/MM/yyyy");

    qDebug() << "✅ [DEBUG] Form values:"
             << "Immat:" << immat
             << "| Marque:" << marque
             << "| Modele:" << modele
             << "| Type:" << type
             << "| Etat:" << etat
             << "| Service:" << service
             << "| Date:" << date_maint.toString("dd/MM/yyyy");

    Vehicule v(immat, marque, modele, type, etat, service, date_maint);

    bool success = v.modifier(selectedImmat);
    qDebug() << "🧩 [DEBUG] modifier() called with oldImmat =" << selectedImmat
             << "| result =" << success;

    if (success) {
        // ✅ Instant UI update of the selected row (no full reload needed)
        int row = ui->tableau_3->currentRow();
        if (row >= 0 && row < ui->tableau_3->rowCount()) {
            // Assuming columns: 0=ID (hidden), 1=IMMAT, 2=MARQUE, 3=MODELE, 4=TYPE, 5=ETAT, 6=SERVICE, 7=DATE
            if (ui->tableau_3->columnCount() >= 8) {
                ui->tableau_3->item(row, 1)->setText(immat);
                ui->tableau_3->item(row, 2)->setText(marque);
                ui->tableau_3->item(row, 3)->setText(modele);
                ui->tableau_3->item(row, 4)->setText(type);
                ui->tableau_3->item(row, 5)->setText(etat);
                ui->tableau_3->item(row, 6)->setText(service);
                ui->tableau_3->item(row, 7)->setText(date_maint.toString("dd/MM/yyyy"));
                qDebug() << "✅ [DEBUG] UI row updated instantly at row" << row;
            } else {
                qDebug() << "⚠️ [DEBUG] Column count is less than expected; skipping instant UI update.";
            }
        } else {
            qDebug() << "⚠️ [DEBUG] currentRow() invalid; skipping instant UI update.";
        }

        // And also refresh from DB to stay 100% in sync (optional but safe)
        loadVehicules();

        QMessageBox::information(this, "Succès", "Véhicule modifié avec succès !");
        selectedImmat.clear();  // reset after update
    } else {
        QMessageBox::warning(this, "Erreur", "Échec de la modification du véhicule !");
    }
}



void MainWindow::on_exporter_3_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";

    QPdfWriter pdf(fileName);

    // SAFE PARAMETERS
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setResolution(96);  // Avoids file corruption
    pdf.setPageMargins(QMarginsF(20, 20, 20, 20));

    QPainter painter(&pdf);
    if (!painter.isActive()) {
        QMessageBox::critical(this, "Erreur", "Impossible d’écrire dans le fichier PDF !");
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing);

    // ----- TITLE -----
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(0, 0, pdf.width(), 80, Qt::AlignCenter, "Liste des Véhicules");

    // ----- TABLE SETTINGS -----
    int y = 100;
    int rowHeight = 30;

    // You have 8 VISIBLE columns (skip hidden column 0 and 8)
    QList<int> visibleColumns = {1, 2, 3, 4, 5, 6, 7};
    int colCount = visibleColumns.size();

    int colWidth = pdf.width() / colCount;

    // ----- HEADER -----
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    for (int i = 0; i < colCount; i++) {
        int col = visibleColumns[i];
        QString header = ui->tableau_3->horizontalHeaderItem(col)->text();
        painter.drawRect(i * colWidth, y, colWidth, rowHeight);
        painter.drawText(i * colWidth, y, colWidth, rowHeight, Qt::AlignCenter, header);
    }
    y += rowHeight;

    painter.setFont(QFont("Arial", 10));

    // ----- ROWS -----
    for (int row = 0; row < ui->tableau_3->rowCount(); row++) {

        // PAGE BREAK
        if (y + rowHeight > pdf.height() - 40) {
            pdf.newPage();
            y = 20;
        }

        for (int i = 0; i < colCount; i++) {
            int col = visibleColumns[i];
            QString text = ui->tableau_3->item(row, col)->text();

            painter.drawRect(i * colWidth, y, colWidth, rowHeight);
            painter.drawText(i * colWidth + 5, y + 20, text);
        }

        y += rowHeight;
    }

    painter.end();
    QMessageBox::information(this, "Export PDF", "Le tableau a été exporté avec succès !");
}
void MainWindow::loadVehicules()
{
    ui->tableau_3->clear();
    ui->tableau_3->setRowCount(0);
    ui->tableau_3->setColumnCount(9); // +1 colonne cachée

    QStringList headers;
    headers << "ID" << "Immatriculation" << "Marque" << "Modèle"
            << "Type" << "État" << "Service" << "Date de maintenance" << "ORDER_DATE";

    ui->tableau_3->setHorizontalHeaderLabels(headers);

    QSqlQuery q;
    if (!q.exec("SELECT ID_VEHI, IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, "
                "TO_CHAR(DATE_MAINT, 'DD/MM/YYYY') AS DATE_FR, "
                "TO_CHAR(DATE_MAINT, 'YYYY-MM-DD') AS DATE_ORDER "
                "FROM VEHICULE ORDER BY ID_VEHI"))
    {
        QMessageBox::critical(this, "Erreur SQL", q.lastError().text());
        return;
    }

    int row = 0;
    while (q.next()) {
        ui->tableau_3->insertRow(row);

        // Remplir les colonnes visibles
        for (int col = 0; col < 8; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(q.value(col).toString());
            ui->tableau_3->setItem(row, col, item);
        }

        // Colonne cachée ORDER_DATE pour tri
        QTableWidgetItem *dateItem = new QTableWidgetItem(q.value(8).toString());
        ui->tableau_3->setItem(row, 8, dateItem);
        
        // Stocker la date dans Qt::UserRole de la colonne visible pour tri correct
        QDate d = QDate::fromString(q.value(7).toString(), "dd/MM/yyyy");
        if (d.isValid()) {
            ui->tableau_3->item(row, 7)->setData(Qt::UserRole, d.toString("yyyy-MM-dd"));
        }

        row++;
    }

    ui->tableau_3->setColumnHidden(0, true);  // cacher ID
    ui->tableau_3->setColumnHidden(8, true);  // cacher ORDER_DATE
    ui->tableau_3->resizeColumnsToContents();
}

QChartView* MainWindow::createVehiculePieChart()
{
    // 🔹 Create the data series
    QPieSeries* series = new QPieSeries();

    // 🔹 Fetch data from the database (STAT PAR ÉTAT)
    QSqlQuery query;
    if (!query.exec("SELECT ETAT, COUNT(*) FROM VEHICULE GROUP BY ETAT")) {
        qDebug() << "❌ SQL Error (stats ETAT):" << query.lastError().text();
        return nullptr;
    }

    int total = 0;
    QMap<QString, int> data;
    while (query.next()) {
        QString etat = query.value(0).toString();
        int count = query.value(1).toInt();
        data[etat] = count;
        total += count;
    }

    // 🔹 Create slices dynamically
    for (auto it = data.begin(); it != data.end(); ++it) {
        double perc = (total > 0) ? (double(it.value()) / total) * 100.0 : 0.0;
        QString label = QString("%1 - %2 véhicules (%3%)")
                            .arg(it.key())
                            .arg(it.value())
                            .arg(QString::number(perc, 'f', 1));

        QPieSlice* slice = series->append(label, it.value());
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
        slice->setLabelFont(QFont("Arial", 10, QFont::Bold));

        QColor color = QColor::fromHsv(
            QRandomGenerator::global()->bounded(360),
            200,
            250
            );
        slice->setBrush(color);

        QObject::connect(slice, &QPieSlice::hovered, [slice](bool hovered) {
            if (hovered) {
                slice->setExploded(true);
                slice->setExplodeDistanceFactor(0.15);
                slice->setLabelFont(QFont("Arial", 11, QFont::Bold));
            } else {
                slice->setExploded(false);
                slice->setLabelFont(QFont("Arial", 10, QFont::Normal));
            }
        });
    }

    // 🔹 Create chart
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des véhicules par état (%)");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 9));
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setTheme(QChart::ChartThemeLight);

    // 🔹 Create chart view
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 🔹 Create dialog container
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Statistiques des véhicules par état");
    dialog->resize(800, 600);
    dialog->setModal(true);

    QPushButton* closeButton = new QPushButton("Fermer");
    closeButton->setFixedWidth(120);
    closeButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; border-radius: 8px; padding: 6px; }"
        "QPushButton:hover { background-color: #d32f2f; }"
        );

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    QObject::connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    QRect parentRect = this->geometry();
    int x = parentRect.center().x() - dialog->width() / 2;
    int y = parentRect.center().y() - dialog->height() / 2;
    dialog->move(x, y);

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();

    return nullptr;
}
void MainWindow::on_pushButton_3_clicked()
{
    createVehiculePieChart();  // ✅ just call it, no need for message box
}

void MainWindow::on_lineEdit_2_textChanged(const QString &text)
{
    QString search = text.trimmed().toLower();
    search.remove("-"); // ignore les tirets

    for (int row = 0; row < ui->tableau_3->rowCount(); ++row) {

        bool match = false;

        // Reset couleurs
        for (int col = 0; col < ui->tableau_3->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableau_3->item(row, col);
            if (item) {
                item->setBackground(Qt::white);
                item->setForeground(Qt::black);
            }
        }

        // Recherche dans toutes les colonnes
        for (int col = 0; col < ui->tableau_3->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableau_3->item(row, col);

            if (item) {
                QString cell = item->text().toLower();
                cell.remove("-");

                if (cell.contains(search)) {
                    match = true;
                    item->setBackground(QColor("#87CEFA")); // surligne
                    item->setForeground(Qt::black);
                }
            }
        }

        ui->tableau_3->setRowHidden(row, !match);
    }
}
void MainWindow::on_tribox_changed(const QString &mode)
{
    int marqueCol = 2;  // Colonne Marque
    int typeCol = 4;    // Colonne Type
    int dateCol = 7;    // Colonne Date de maintenance
    
    // Désactiver le tri automatique temporairement
    ui->tableau_3->setSortingEnabled(false);

    // Tri par Marque
    if (mode.contains("Marque A → Z") || mode.contains("Marque A->Z")) {
        ui->tableau_3->sortItems(marqueCol, Qt::AscendingOrder);
        qDebug() << "Tri par marque A→Z";
    }
    else if (mode.contains("Marque Z → A") || mode.contains("Marque Z->A")) {
        ui->tableau_3->sortItems(marqueCol, Qt::DescendingOrder);
        qDebug() << "Tri par marque Z→A";
    }
    // Tri par Type
    else if (mode.contains("Type A → Z") || mode.contains("Type A->Z")) {
        ui->tableau_3->sortItems(typeCol, Qt::AscendingOrder);
        qDebug() << "Tri par type A→Z";
    }
    else if (mode.contains("Type Z → A") || mode.contains("Type Z->A")) {
        ui->tableau_3->sortItems(typeCol, Qt::DescendingOrder);
        qDebug() << "Tri par type Z→A";
    }
    // Tri par Date
    else if (mode.contains("Date croissant") || mode.contains("Croissant")) {
        ui->tableau_3->sortItems(dateCol, Qt::AscendingOrder);
        qDebug() << "Tri par date croissant (anciennes → récentes)";
    }
    else if (mode.contains("Date décroissant") || mode.contains("Décroissant")) {
        ui->tableau_3->sortItems(dateCol, Qt::DescendingOrder);
        qDebug() << "Tri par date décroissant (récentes → anciennes)";
    }
    // Réinitialiser le tri (ordre par défaut)
    else if (mode.contains("Défaut") || mode.contains("Aucun")) {
        loadVehicules(); // Recharge les données dans l'ordre par défaut (par ID)
        qDebug() << "Tri réinitialisé (ordre par défaut)";
        return;
    }
    
    // Réactiver le tri si nécessaire
    ui->tableau_3->setSortingEnabled(true);
}

void MainWindow::on_btnTriDate_clicked()
{
    int dateCol = 7; // Colonne de date de maintenance
    
    // Alterner entre croissant et décroissant
    static bool triCroissant = true;
    
    ui->tableau_3->setSortingEnabled(false);
    
    if (triCroissant) {
        ui->tableau_3->sortItems(dateCol, Qt::AscendingOrder);
        qDebug() << "Tri par date croissant (anciennes → récentes)";
    } else {
        ui->tableau_3->sortItems(dateCol, Qt::DescendingOrder);
        qDebug() << "Tri par date décroissant (récentes → anciennes)";
    }
    
    ui->tableau_3->setSortingEnabled(true);
    triCroissant = !triCroissant;
}

void MainWindow::on_btnSendChat_clicked()

{
    QString userMessage = ui->chatInput->text().trimmed();
    if (userMessage.isEmpty())
        return;

    ui->chatHistory->append("<b>Vous :</b> " + userMessage);
    ui->chatInput->clear();

    sendMessageToAzureAI(userMessage);
}
void MainWindow::sendMessageToAzureAI(const QString &message)
{
    qDebug() << "🚀 sendMessageToAzureAI() démarre";
//preparation des informations de connextion azur
    QString endpoint = "https://ai-kassem.cognitiveservices.azure.com"; // url de service
    QString apiKey   = "445NLYUwthBdj5EbFvxbCxV2XSdJWKYartumAOvqEFMtKEofmdpuJQQJ99BGACfhMk5XJ3w3AAAAACOGRpAA";
    QString apiVersion = "2024-12-01-preview";
    QString model = "gpt-4o";

    QString url = endpoint + "/openai/deployments/" + model + "/chat/completions?api-version=" + apiVersion;

    qDebug() << "🌍 URL Azure =" << url;
//creaton de la requette HTTP
    QNetworkRequest request{ QUrl(url) };
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("api-key", apiKey.toUtf8());

    // role de l’IA
    QString systemPrompt = R"(
Tu es SmartHelp, assistant SmartCity. Réponds toujours en texte simple, sans markdown, en maximum 3 lignes. Donne des explications claires et courtes sur l’utilisation des fonctions, champs, erreurs ou actions dans l’application.
Ton rôle est d’aider l’utilisateur à comprendre et utiliser les fonctionnalités de l’application de manière simple, claire et utile.

Voici ce que tu dois faire :

1. Expliquer les modules et fonctionnalités de l’application SmartCity, notamment :
   • Gestion des véhicules (ajouter, modifier, supprimer, rechercher)
   • Gestion des résidents
   • Gestion des employés
   • Gestion des incidents et maintenance
   • Historique, logs et états des objets
   • Interaction avec l’interface Qt : boutons, champs, tables, filtres

2. Fournir des explications détaillées sur chaque champ d’un formulaire :
   • immatriculation
   • marque
   • modèle
   • type
   • état
   • service
   • date
   • identifiants liés (ID_RES, ID_EMP, etc.)

3. Guider l’utilisateur dans les actions :
   • comment ajouter un véhicule
   • comment modifier un véhicule
   • comment supprimer un véhicule
   • comment rechercher un véhicule
   • comment éviter les erreurs

4. Décrire les erreurs possibles :
   • champ vide
   • format incorrect
   • date invalide (ex : date future)
   • doublon d’immatriculation
   • ID inexistant
   • échec de connexion à la base Oracle

5. Toujours répondre :
   • poliment
   • clairement
   • avec des étapes simples
   • en donnant des exemples quand nécessaire

6. Ne pas inventer des fonctionnalités inexistantes. Si ce n'est pas dans SmartCity, tu expliques calmement que ce n’est pas prévu.

Ton objectif : être un assistant intégré, très utile, qui explique l’utilisation de l’application SmartCity comme si tu étais un guide interactif.
)";

    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = systemPrompt;

    // --- JSON ---
    QJsonObject userMsg;

    userMsg["role"] = "user";
    userMsg["content"] = message;

    QJsonArray arr;
    arr.append(systemMsg);
    arr.append(userMsg);

    QJsonObject payload;
    payload["messages"] = arr;
    payload["temperature"] = 0.7;

    QByteArray jsonData = QJsonDocument(payload).toJson();

    qDebug() << "📦 Payload =" << jsonData;

    // --- SEND WITH CRASH DETECTION ---
    QNetworkReply *reply = nullptr;

    try {
        reply = networkManager->post(request, jsonData);
    }
    catch(...) {
        qDebug() << "💥 EXCEPTION : Crash pendant POST() !";
        return;
    }

    connect(reply, &QNetworkReply::errorOccurred,
            [](QNetworkReply::NetworkError code){
                qDebug() << "❌ ERROR immediate =" << code;
            });

    connect(reply, &QNetworkReply::finished, [this, reply]() {

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "❌ ERROR :" << reply->errorString();
            ui->chatHistory->append("<b>Bot :</b> Erreur : " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        qDebug() << "📩 Réponse =" << data;

        QJsonDocument json = QJsonDocument::fromJson(data);
        QString bot = json["choices"][0]["message"]["content"].toString();

        ui->chatHistory->append("<b>Bot :</b> " + bot);

        reply->deleteLater();
    });
}



QString MainWindow::processChatMessage(const QString &msg)
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
    return "Je n’ai pas compris 😅. Essayez avec des mots comme : ajouter véhicule, modifier, supprimer, tri, recherche, date maintenance.";
}

QString MainWindow::buildMaintenancePromptFromCurrentVehicle() const
{
    QString immat   = ui->immatline_2->text();
    QString marque  = ui->marqueline_2->text();
    QString modele  = ui->modeleline_2->text();
    QString type    = ui->triemail_2->currentText();
    QString etat    = ui->Etatline_2->currentText();
    QString service = ui->serviceline_2->text();
    QDate   date_maint = QDate::fromString(ui->datemaintline_2->text(), "dd/MM/yyyy");

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

void MainWindow::sendRecommendationToAzureAI(const QString &message)
{
    qDebug() << "🚀 sendRecommendationToAzureAI() démarre";

    QString endpoint   = "https://ai-kassem.cognitiveservices.azure.com";
    QString apiKey     = "445NLYUwthBdj5EbFvxbCxV2XSdJWKYartumAOvqEFMtKEofmdpuJQQJ99BGACfhMk5XJ3w3AAAAACOGRpAA"; // ⚠️ garde ton vrai key en privé dans ton projet
    QString apiVersion = "2024-12-01-preview";
    QString model      = "gpt-4o";

    QString url = endpoint
                  + "/openai/deployments/"
                  + model
                  + "/chat/completions?api-version="
                  + apiVersion;

    qDebug() << "🌍 URL Azure =" << url;

    QNetworkRequest request{ QUrl(url) };
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("api-key", apiKey.toUtf8());

    // SYSTEM MESSAGE (spécial maintenance, max 3 lignes, texte simple)
    QString systemPrompt = R"(Tu es SmartHelp, assistant SmartCity.
Tu donnes des recommandations de maintenance pour les véhicules.
Réponds toujours en français, en texte simple, sans markdown, en maximum 10 lignes.)";

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = systemPrompt;

    // USER MESSAGE (les infos du véhicule + consigne déjà incluses dans message)
    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = message;

    QJsonArray arr;
    arr.append(systemMsg);
    arr.append(userMsg);

    QJsonObject payload;
    payload["messages"]    = arr;
    payload["temperature"] = 0.4; // un peu plus sérieux pour de la maintenance

    QByteArray jsonData = QJsonDocument(payload).toJson();
    qDebug() << "📦 Payload =" << jsonData;

    QNetworkReply *reply = nullptr;

    try {
        reply = networkManager->post(request, jsonData);
    }
    catch (...) {
        qDebug() << "💥 EXCEPTION : Crash pendant POST() !";
        return;
    }

    connect(reply, &QNetworkReply::errorOccurred,
            [](QNetworkReply::NetworkError code){
                qDebug() << "❌ ERROR immediate =" << code;
            });

    connect(reply, &QNetworkReply::finished, [this, reply]() {

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "❌ ERROR :" << reply->errorString();
            ui->textRecom->setPlainText("Erreur Azure : " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        qDebug() << "📩 Réponse =" << data;

        QJsonDocument json = QJsonDocument::fromJson(data);
        QString bot = json["choices"][0]["message"]["content"].toString();

        // 📝 Afficher la reco dans la zone de texte
        ui->textRecom->setPlainText(bot.trimmed());

        reply->deleteLater();
    });
}
void MainWindow::on_btnRecom_clicked()
{
    qDebug() << "🔎 [DEBUG] on_btnRecom_clicked() triggered";
    qDebug() << "🔎 [DEBUG] selectedImmat current value:" << selectedImmat;

    if (selectedImmat.isEmpty()) {
        qDebug() << "⚠️ [DEBUG] No vehicle selected before recommendation!";
        QMessageBox::warning(this,
                             "Erreur",
                             "Veuillez d'abord sélectionner un véhicule avant de demander une recommandation.");
        return;
    }

    // Construire le message pour l'IA à partir du véhicule courant
    QString prompt = buildMaintenancePromptFromCurrentVehicle();
    qDebug() << "📨 [DEBUG] Maintenance prompt envoyé à Azure :" << prompt;

    // Optionnel : nettoyer la zone avant la réponse
    ui->textRecom->clear();
    ui->textRecom->setPlainText("Génération de la recommandation en cours...");

    // Lancer l'appel Azure
    sendRecommendationToAzureAI(prompt);

    // Afficher la page de recommandation
    ui->stackedWidget_3->setCurrentWidget(ui->page_11);
}






