#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connection.h"
#include "maison.h"
#include "alerte.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QDesktopServices>
#include <QUrl>

#include <QMessageBox>
#include <QSqlQueryModel>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include <QDate>
#include <QPrinter>
#include <QTextDocument>
#include <QGraphicsView>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QPixmap>
#include <QtMath>

/* ============================================================
 *                     CONSTRUCTEUR
 * ============================================================ */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sceneCarte = new QGraphicsScene(this);
    viewCarte = new QGraphicsView(ui->frame_2);
    viewCarte->setGeometry(QRect(700, 110, 370, 230));
    viewCarte->setScene(sceneCarte);
    viewCarte->hide();
    net = new QNetworkAccessManager(this);

    connectButtons();
    connectMaisonButtons();
    connectAlerteButtons();

    refreshTableMaison();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* ============================================================
 *                         NAVIGATION
 * ============================================================ */
void MainWindow::connectButtons()
{
    connect(ui->pushButton,   &QPushButton::clicked, this, &MainWindow::onGestionEmployes);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onGestionVehicules);
}

void MainWindow::onGestionEmployes()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::onGestionVehicules()
{
    ui->stackedWidget->setCurrentIndex(1);
}

/* ============================================================
 *                      CRUD MAISON
 * ============================================================ */
void MainWindow::connectMaisonButtons()
{
    connect(ui->ajouter_7,   &QPushButton::clicked, this, &MainWindow::onAjouterMaison);
    connect(ui->modifier_7,  &QPushButton::clicked, this, &MainWindow::onModifierMaison);
    connect(ui->supprimer_6, &QPushButton::clicked, this, &MainWindow::onSupprimerMaison);

    connect(ui->tableau_7, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onTableMaisonSelectionChanged);

    connect(ui->rechercheMaisons, &QLineEdit::textChanged,
            this, &MainWindow::onRechercheMaisons);

    connect(ui->triMaisons, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onTriMaisons);

    connect(ui->btnExporterMaisons, &QPushButton::clicked,
            this, &MainWindow::onExporterMaisons);
}

/* ============================================================
 *                   GESTION ALERTES
 * ============================================================ */
void MainWindow::connectAlerteButtons()
{
    connect(ui->btnRetourAlertes, &QPushButton::clicked,
            this, &MainWindow::onRetourAlertes);

    connect(ui->btnAfficherCarte, &QPushButton::clicked,
            this, &MainWindow::onAfficherCarte);
}

void MainWindow::on_Alertes_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->stackedWidget_5->setCurrentIndex(1);
    refreshAlertes();
}

/* ============================================================
 *                    AJOUT MAISON
 * ============================================================ */
void MainWindow::onAjouterMaison()
{
    int id          = ui->idmaisonline->text().toInt();
    QString adresse = ui->adresseline_2->text();
    int securite    = ui->nivsecline->text().toInt();
    QString statut  = ui->statuschoix->currentText();
    QString typeMaison = ui->typechoix->currentText();
    int nbrPieces = ui->nbrpieceschoix->currentText().toInt();

    if (id <= 0 || adresse.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez saisir un ID et une adresse.");
        return;
    }

    if (securite < 1 || securite > 5) {
        QMessageBox::warning(this, "Erreur", "Le niveau de sécurité doit être entre 1 et 5.");
        return;
    }

    Maison m(id, adresse, securite, statut, typeMaison, nbrPieces);

    if (!m.ajouter()) {
        QMessageBox::warning(this, "Erreur", "Échec de l'ajout.");
        return;
    }

    if (securite < 2) {
        Alerte a;
        a.setIdMaison(id);
        a.setZone(adresse);
        a.setNiveau(securite == 1 ? 1 : 2);  // S'assurer que le niveau est au moins 1
        a.setStatut("En attente");
        a.setDateAlerte(QDate::currentDate());
        a.ajouter();
    }

    refreshTableMaison();
    QMessageBox::information(this, "Ajout", "Maison ajoutée avec succès.");
}

/* ============================================================
 *                    MODIFIER MAISON
 * ============================================================ */
void MainWindow::onModifierMaison()
{
    int id = ui->idmaisonline->text().toInt();

    if (id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une maison.");
        return;
    }

    int securite = ui->nivsecline->text().toInt();
    
    if (securite < 1 || securite > 5) {
        QMessageBox::warning(this, "Erreur", "Le niveau de sécurité doit être entre 1 et 5.");
        return;
    }

    Maison m;
    m.setId(id);
    m.setAdresse(ui->adresseline_2->text());
    m.setSecurite(securite);
    m.setStatut(ui->statuschoix->currentText());
    m.setType(ui->typechoix->currentText());
    m.setNbrPieces(ui->nbrpieceschoix->currentText().toInt());

    if (m.modifier(id)) {

        if (securite < 2 && !Alerte::existeAlertePourMaison(id)) {
            Alerte(id, ui->adresseline_2->text(), securite, "En attente").ajouter();
        }

        refreshTableMaison();
        QMessageBox::information(this, "Modification", "Maison modifiée.");
    } else {
        QMessageBox::warning(this, "Erreur", "Échec de la modification.");
    }
}

/* ============================================================
 *                    SUPPRIMER MAISON
 * ============================================================ */
void MainWindow::onSupprimerMaison()
{
    int id = ui->idmaisonline->text().toInt();

    if (id <= 0) {
        QMessageBox::warning(this, "Erreur", "Sélectionnez une maison.");
        return;
    }

    Maison m;

    if (m.supprimer(id)) {
        refreshTableMaison();
        clearMaisonInputs();
        QMessageBox::information(this, "Suppression", "Maison supprimée.");
    }
    else {
        QMessageBox::warning(this, "Erreur", "Échec de la suppression.");
    }
}

/* ============================================================
 *                 CHARGEMENT DES INPUTS
 * ============================================================ */
void MainWindow::onTableMaisonSelectionChanged()
{
    if (ui->tableau_7->selectedItems().isEmpty())
        return;

    int row = ui->tableau_7->currentRow();
    int id  = ui->tableau_7->item(row, 0)->text().toInt();

    loadMaisonToInputs(id);
}

void MainWindow::loadMaisonToInputs(int id)
{
    Maison m;
    if (!m.rechercher(id)) return;

    ui->idmaisonline->setText(QString::number(m.getId()));
    ui->adresseline_2->setText(m.getAdresse());
    ui->nivsecline->setText(QString::number(m.getSecurite()));
    ui->statuschoix->setCurrentText(m.getStatut());
    ui->typechoix->setCurrentText(m.getType());
    ui->nbrpieceschoix->setCurrentText(QString::number(m.getNbrPieces()));
}

/* ============================================================
 *                AFFICHER TABLE MAISONS
 * ============================================================ */
void MainWindow::refreshTableMaison()
{
    ui->tableau_7->clearContents();
    ui->tableau_7->setRowCount(0);

    QSqlQueryModel *model = Maison::afficher();

    for (int i = 0; i < model->rowCount(); ++i) {
        ui->tableau_7->insertRow(i);

        for (int j = 0; j < model->columnCount(); ++j) {
            ui->tableau_7->setItem(
                i, j,
                new QTableWidgetItem(model->data(model->index(i, j)).toString())
                );
        }
    }
}

/* ============================================================
 *                   STATISTIQUES
 * ============================================================ */
QMap<QString, int> MainWindow::getStatistiquesStatus()
{
    Maison m;
    return m.getStatistiquesStatus();
}

QMap<QString, int> MainWindow::getStatistiquesNiveauSecurite()
{
    Maison m;
    return m.getStatistiquesNiveauSecurite();
}

/* ------------------------------------------------------------ */
void MainWindow::on_statistique_currentIndexChanged(int index)
{
    if (index == 0) return;

    QDialog *d = new QDialog(this);
    d->setWindowTitle("Statistiques des maisons");
    d->setMinimumSize(650, 520);

    QVBoxLayout *layout = new QVBoxLayout(d);
    QPieSeries *series = new QPieSeries();
    QString title;
    int total = 0;

    if (index == 1)
    {
        auto s = getStatistiquesStatus();
        total = s["total"];
        title = "Statistiques des statuts";

        if (s["actif"] > 0)        series->append("Actif", s["actif"]);
        if (s["inactif"] > 0)      series->append("Inactif", s["inactif"]);
        if (s["maintenance"] > 0)  series->append("En maintenance", s["maintenance"]);
        if (s["vendu"] > 0)        series->append("Vendu", s["vendu"]);
    }

    else if (index == 2)
    {
        auto s = getStatistiquesNiveauSecurite();
        total = s["total"];
        title = "Niveau de sécurité";

        if (s["danger"] > 0)    series->append("Danger (<2)", s["danger"]);
        if (s["moyen"] > 0)     series->append("Moyen (2-3)", s["moyen"]);
        if (s["bon"] > 0)       series->append("Bon (>3)", s["bon"]);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title + " — Total : " + QString::number(total));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setVisible(true);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);

    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok);
    layout->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, d, &QDialog::accept);

    d->exec();
}

/* ============================================================
 *                RECHERCHE, TRI, EXPORT
 * ============================================================ */
void MainWindow::clearMaisonInputs()
{
    ui->idmaisonline->clear();
    ui->adresseline_2->clear();
    ui->nivsecline->clear();
    ui->statuschoix->setCurrentIndex(0);
    ui->typechoix->setCurrentIndex(0);
    ui->nbrpieceschoix->setCurrentIndex(0);
}

void MainWindow::onRechercheMaisons()
{
    const QString q = ui->rechercheMaisons->text().trimmed();
    const bool empty = q.isEmpty();
    for (int r = 0; r < ui->tableau_7->rowCount(); ++r) {
        bool match = empty;
        if (!empty) {
            for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
                QTableWidgetItem *it = ui->tableau_7->item(r, c);
                if (it && it->text().contains(q, Qt::CaseInsensitive)) { match = true; break; }
            }
        }
        ui->tableau_7->setRowHidden(r, !match);
    }
}

void MainWindow::onTriMaisons()
{
    int idx = ui->triMaisons->currentIndex();
    switch (idx) {
    case 1: ui->tableau_7->sortItems(0, Qt::AscendingOrder); break;
    case 2: ui->tableau_7->sortItems(0, Qt::DescendingOrder); break;
    case 3: ui->tableau_7->sortItems(1, Qt::AscendingOrder); break;
    case 4: ui->tableau_7->sortItems(2, Qt::AscendingOrder); break;
    case 5: ui->tableau_7->sortItems(2, Qt::DescendingOrder); break;
    case 6: ui->tableau_7->sortItems(3, Qt::AscendingOrder); break;
    case 7: ui->tableau_7->sortItems(4, Qt::AscendingOrder); break;
    default: break;
    }
}

void MainWindow::onExporterMaisons()
{
    bool ok = false;
    const QString choice = QInputDialog::getItem(
        this, "Exporter les maisons", "Format", {"PDF", "CSV"}, 0, false, &ok);
    if (!ok || choice.isEmpty()) return;
    if (choice == "PDF") exporterMaisonsPDF(); else exporterMaisonsCSV();
}

void MainWindow::exporterMaisonsPDF()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter PDF", QString(), "PDF (*.pdf)");
    if (path.isEmpty()) return;

    QString html;
    html += "<h2>Liste des maisons</h2>";
    html += "<table border='1' cellspacing='0' cellpadding='4'>";
    html += "<tr>";
    for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
        QTableWidgetItem *h = ui->tableau_7->horizontalHeaderItem(c);
        html += "<th>" + (h ? h->text() : QString("Col %1").arg(c)) + "</th>";
    }
    html += "</tr>";
    for (int r = 0; r < ui->tableau_7->rowCount(); ++r) {
        if (ui->tableau_7->isRowHidden(r)) continue;
        html += "<tr>";
        for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
            QTableWidgetItem *it = ui->tableau_7->item(r, c);
            html += "<td>" + (it ? it->text().toHtmlEscaped() : QString()) + "</td>";
        }
        html += "</tr>";
    }
    html += "</table>";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);
}

void MainWindow::exporterMaisonsCSV()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter CSV", QString(), "CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
        QTableWidgetItem *h = ui->tableau_7->horizontalHeaderItem(c);
        out << (h ? h->text() : QString("Col %1").arg(c));
        if (c < ui->tableau_7->columnCount() - 1) out << ",";
    }
    out << "\n";
    for (int r = 0; r < ui->tableau_7->rowCount(); ++r) {
        if (ui->tableau_7->isRowHidden(r)) continue;
        for (int c = 0; c < ui->tableau_7->columnCount(); ++c) {
            QTableWidgetItem *it = ui->tableau_7->item(r, c);
            out << (it ? it->text() : QString());
            if (c < ui->tableau_7->columnCount() - 1) out << ",";
        }
        out << "\n";
    }
    file.close();
}

void MainWindow::refreshAlertes()
{
    QSqlQuery query("SELECT ZONE, NIVEAU, STATUT, DATE_ALERTE FROM GEST_ALERTES",
                    QSqlDatabase::database("qt_oracle"));

    ui->tableau_10->clearContents();
    ui->tableau_10->setRowCount(0);

    QTableWidgetItem *headerZone = ui->tableau_10->horizontalHeaderItem(0);
    if (headerZone) headerZone->setText("zone");
    QTableWidgetItem *headerNiv = ui->tableau_10->horizontalHeaderItem(1);
    if (headerNiv) headerNiv->setText("niveau");
    QTableWidgetItem *headerStatut = ui->tableau_10->horizontalHeaderItem(2);
    if (headerStatut) headerStatut->setText("statut");
    QTableWidgetItem *headerDate = ui->tableau_10->horizontalHeaderItem(3);
    if (headerDate) headerDate->setText("date");
    QTableWidgetItem *headerLoc = ui->tableau_10->horizontalHeaderItem(4);
    if (headerLoc) headerLoc->setText("Localisation");

    int row = 0;
    while (query.next()) {
        ui->tableau_10->insertRow(row);

        QString zone   = query.value(0).toString();
        QString niveau = query.value(1).toString();
        QString statut = query.value(2).toString();
        QString dateA  = query.value(3).toString();

        ui->tableau_10->setItem(row, 0, new QTableWidgetItem(zone));
        ui->tableau_10->setItem(row, 1, new QTableWidgetItem(niveau));
        ui->tableau_10->setItem(row, 2, new QTableWidgetItem(statut));
        ui->tableau_10->setItem(row, 3, new QTableWidgetItem(dateA));

        QPushButton *btn = new QPushButton("Localiser");
        btn->setStyleSheet("background-color: #007bff; color: white; border-radius: 6px; padding: 4px 8px;");
        connect(btn, &QPushButton::clicked, this, [this, zone]() { loadMapForZone(zone); });
        ui->tableau_10->setCellWidget(row, 4, btn);

        row++;
    }

    ui->tableau_10->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::onAfficherCarte()
{
    int row = ui->tableau_10->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Carte", "Veuillez sélectionner une alerte.");
        return;
    }
    QString zone = ui->tableau_10->item(row, 0)->text();
    loadMapForZone(zone);
}

void MainWindow::loadMapForZone(const QString &zone)
{
    QUrl geocode("https://nominatim.openstreetmap.org/search");
    QUrlQuery query;
    query.addQueryItem("q", zone);
    query.addQueryItem("format", "json");
    query.addQueryItem("limit", "1");
    geocode.setQuery(query);
    QNetworkRequest req(geocode);
    req.setRawHeader("User-Agent", QByteArray("AtelierConnexion/1.0"));
    QNetworkReply *r = net->get(req);
    connect(r, &QNetworkReply::finished, this, [this, r]() {
        if (r->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "Carte", "Géocodage indisponible.");
            r->deleteLater();
            return;
        }
        QByteArray data = r->readAll();
        r->deleteLater();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray() || doc.array().isEmpty()) {
            QMessageBox::information(this, "Carte", "Adresse introuvable.");
            return;
        }
        QJsonObject obj = doc.array().first().toObject();
        QString lat = obj.value("lat").toString();
        QString lon = obj.value("lon").toString();
        QUrl imgUrl(QString("http://staticmap.openstreetmap.de/staticmap.php?center=%1,%2&zoom=14&size=370x230&markers=%1,%2,red-pushpin").arg(lat, lon));
        QNetworkRequest imgReq(imgUrl);
        imgReq.setRawHeader("User-Agent", QByteArray("AtelierConnexion/1.0"));
        QNetworkReply *ri = net->get(imgReq);
        connect(ri, &QNetworkReply::finished, this, [this, ri, lat, lon]() {
            if (ri->error() == QNetworkReply::NoError) {
                QByteArray bytes = ri->readAll();
                ri->deleteLater();
                QPixmap pix;
                if (pix.loadFromData(bytes)) {
                    sceneCarte->clear();
                    sceneCarte->addPixmap(pix);
                    viewCarte->show();
                    return;
                }
            }
            ri->deleteLater();
            bool okLat = false, okLon = false;
            double dLat = lat.toDouble(&okLat);
            double dLon = lon.toDouble(&okLon);
            if (!okLat || !okLon) {
                QMessageBox::warning(this, "Carte", "Localisation introuvable.");
                return;
            }
            int zoom = 14;
            double latRad = qDegreesToRadians(dLat);
            double n = (1 << zoom);
            int xTile = qFloor((dLon + 180.0) / 360.0 * n);
            int yTile = qFloor((1.0 - log(tan(latRad) + 1.0 / cos(latRad)) / M_PI) / 2.0 * n);
            QUrl tileUrl(QString("http://tile.openstreetmap.org/%1/%2/%3.png").arg(QString::number(zoom), QString::number(xTile), QString::number(yTile)));
            QNetworkRequest tileReq(tileUrl);
            tileReq.setRawHeader("User-Agent", QByteArray("AtelierConnexion/1.0"));
            QNetworkReply *rt = net->get(tileReq);
            connect(rt, &QNetworkReply::finished, this, [this, rt, dLat, dLon, zoom, xTile, yTile]() {
                if (rt->error() != QNetworkReply::NoError) {
                    QMessageBox::warning(this, "Carte", "Impossible de charger la localisation.");
                    rt->deleteLater();
                    return;
                }
                QByteArray tileBytes = rt->readAll();
                rt->deleteLater();
                QPixmap tilePix;
                if (!tilePix.loadFromData(tileBytes)) {
                    QMessageBox::information(this, "Carte", "Image de tuile invalide.");
                    return;
                }
                double latRad2 = qDegreesToRadians(dLat);
                double n2 = (1 << zoom);
                double pixelX = ((dLon + 180.0) / 360.0) * 256.0 * n2;
                double pixelY = ((1.0 - log(tan(latRad2) + 1.0 / cos(latRad2)) / M_PI) / 2.0) * 256.0 * n2;
                double localX = pixelX - 256.0 * xTile;
                double localY = pixelY - 256.0 * yTile;
                QPixmap composed = tilePix.copy();
                QPainter p(&composed);
                p.setRenderHint(QPainter::Antialiasing, true);
                p.setPen(QPen(Qt::red, 2));
                p.setBrush(QBrush(Qt::red));
                p.drawEllipse(QPointF(localX, localY), 6, 6);
                p.end();
                sceneCarte->clear();
                sceneCarte->addPixmap(composed);
                viewCarte->show();
            });
        });
    });
}

void MainWindow::onRetourAlertes()
{
    ui->stackedWidget_5->setCurrentIndex(0);
}
