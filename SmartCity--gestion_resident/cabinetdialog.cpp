#include "cabinetdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QDebug>
#include <QSqlQueryModel>
#include <QGroupBox>

CabinetDialog::CabinetDialog(QWidget *parent)
    : QDialog(parent), m_cabinet(nullptr), m_mediaPlayer(nullptr), m_videoWidget(nullptr), m_audioOutput(nullptr)
{
    setWindowTitle(tr("Gestion des Cabinets"));
    setGeometry(100, 100, 1200, 800);
    
    m_cabinet = new Cabinet();
    setupUI();
    setupVideoPlayer();
}

CabinetDialog::~CabinetDialog()
{
    if (m_cabinet) {
        delete m_cabinet;
    }
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
}

void CabinetDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Toolbar with buttons
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    
    QLabel* lblCritere = new QLabel(tr("Rechercher par:"));
    m_searchCritere = new QComboBox();
    m_searchCritere->addItems(QStringList()
                              << tr("Nom")
                              << tr("Adresse")
                              << tr("Email")
                              << tr("Spécialité")
                              << tr("Téléphone")
                              << tr("ID résident"));
    
    QLabel* lblSearch = new QLabel(tr("Valeur:"));
    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText(tr("Entrez la valeur de recherche..."));
    
    m_btnAfficher = new QPushButton(tr("Afficher"));
    m_btnRecherche = new QPushButton(tr("Recherche Cabinet"));
    m_btnStat = new QPushButton(tr("Statistiques"));
    m_btnExportExcel = new QPushButton(tr("Exporter Excel"));
    
    connect(m_btnAfficher, &QPushButton::clicked, this, &CabinetDialog::onAfficherClicked);
    connect(m_btnRecherche, &QPushButton::clicked, this, &CabinetDialog::onRechercheClicked);
    connect(m_btnStat, &QPushButton::clicked, this, &CabinetDialog::onStatClicked);
    connect(m_btnExportExcel, &QPushButton::clicked, this, &CabinetDialog::onExportExcelClicked);
    
    toolbarLayout->addWidget(lblCritere);
    toolbarLayout->addWidget(m_searchCritere);
    toolbarLayout->addWidget(lblSearch);
    toolbarLayout->addWidget(m_searchInput);
    toolbarLayout->addWidget(m_btnAfficher);
    toolbarLayout->addWidget(m_btnRecherche);
    toolbarLayout->addWidget(m_btnStat);
    toolbarLayout->addWidget(m_btnExportExcel);
    toolbarLayout->addStretch();
    
    mainLayout->addLayout(toolbarLayout);
    
    // Table and Chart area
    QHBoxLayout* contentLayout = new QHBoxLayout();
    
    m_tableView = new QTableView();
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    m_chartView = new QChartView();
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->hide();
    
    contentLayout->addWidget(m_tableView, 2);
    contentLayout->addWidget(m_chartView, 1);
    
    mainLayout->addLayout(contentLayout);
    
    // Video player section
    QGroupBox* videoGroup = new QGroupBox(tr("Lecteur Vidéo"));
    QVBoxLayout* videoLayout = new QVBoxLayout(videoGroup);
    
    m_videoWidget = new QVideoWidget();
    m_videoWidget->setMinimumHeight(200);
    
    QHBoxLayout* videoControlLayout = new QHBoxLayout();
    m_btnOpenVideo = new QPushButton(tr("Ouvrir Vidéo"));
    m_btnPlayVideo = new QPushButton(tr("Lecture"));
    m_btnStopVideo = new QPushButton(tr("Arrêt"));
    
    connect(m_btnOpenVideo, &QPushButton::clicked, this, &CabinetDialog::onOpenVideoClicked);
    connect(m_btnPlayVideo, &QPushButton::clicked, this, &CabinetDialog::onPlayVideoClicked);
    connect(m_btnStopVideo, &QPushButton::clicked, this, &CabinetDialog::onStopVideoClicked);
    
    videoControlLayout->addWidget(m_btnOpenVideo);
    videoControlLayout->addWidget(m_btnPlayVideo);
    videoControlLayout->addWidget(m_btnStopVideo);
    videoControlLayout->addStretch();
    
    videoLayout->addWidget(m_videoWidget);
    videoLayout->addLayout(videoControlLayout);
    
    mainLayout->addWidget(videoGroup);
    
    setLayout(mainLayout);
}

void CabinetDialog::onAfficherClicked()
{
    QSqlQueryModel* model = m_cabinet->afficherCabinet();
    if (!model) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible de charger les cabinets"));
        return;
    }

    model->setParent(this);
    m_tableView->setModel(model);
    m_tableView->resizeColumnsToContents();
    m_chartView->hide();
    
    QMessageBox::information(this, tr("Succès"), 
        tr("Liste des cabinets chargée: %1 entrées").arg(model->rowCount()));
}

void CabinetDialog::onRechercheClicked()
{
    if (m_searchInput->text().isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez entrer une valeur de recherche"));
        return;
    }
    
    QString critere = m_searchCritere->currentText();
    QString valeur = m_searchInput->text();
    
    QSqlQueryModel* model = m_cabinet->rechercherCabinet(critere, valeur);
    if (!model) {
        QMessageBox::critical(this, tr("Erreur"), tr("Recherche impossible"));
        return;
    }

    model->setParent(this);
    m_tableView->setModel(model);
    m_tableView->resizeColumnsToContents();
    m_chartView->hide();
    
    QMessageBox::information(this, tr("Résultats de recherche"), 
        tr("Nombre de résultats: %1").arg(model->rowCount()));
}

void CabinetDialog::onStatClicked()
{
    QSqlQueryModel* model = m_cabinet->getStatSpecialite();
    if (!model) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible de récupérer les statistiques"));
        return;
    }

    model->setParent(this);
    m_tableView->setModel(model);
    m_tableView->resizeColumnsToContents();
    
    createStatChart(model);
    m_chartView->show();
}

void CabinetDialog::createStatChart(QSqlQueryModel* model)
{
    if (!model || model->rowCount() == 0) {
        QMessageBox::warning(this, tr("Erreur"), tr("Aucune donnée pour créer le graphique"));
        return;
    }
    
    QPieSeries* series = new QPieSeries();
    
    for (int row = 0; row < model->rowCount(); ++row) {
        QString specialite = model->data(model->index(row, 0)).toString();
        int nombre = model->data(model->index(row, 1)).toInt();
        
        QPieSlice* slice = new QPieSlice(specialite, nombre);
        slice->setLabelVisible(true);
        series->append(slice);
    }
    
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Distribution par Spécialité"));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    
    m_chartView->setChart(chart);
}

void CabinetDialog::onExportExcelClicked()
{
    if (!m_tableView->model()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Aucune donnée à exporter"));
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(this,
        tr("Enregistrer sous"), QString(), tr("CSV (*.csv)"));
    
    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath.append(".csv");
    }
    
    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(m_tableView->model());
    if (!model) {
        QMessageBox::warning(this, tr("Erreur"), tr("Aucune donnée à exporter"));
        return;
    }

    if (m_cabinet->exporterExcel(filePath, model)) {
        QMessageBox::information(this, tr("Succès"), 
            tr("Fichier exporté avec succès: %1").arg(filePath));
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors de l'export"));
    }
}

void CabinetDialog::setupVideoPlayer()
{
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    m_audioOutput->setVolume(0.5);
}

void CabinetDialog::onOpenVideoClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Ouvrir une vidéo"),
        QString(),
        tr("Fichiers vidéo (*.mp4 *.avi *.mkv *.mov *.wmv);;Tous les fichiers (*)"));
    
    if (filePath.isEmpty()) {
        return;
    }
    
    m_mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    QMessageBox::information(this, tr("Vidéo chargée"), 
        tr("Vidéo prête à être lue : %1").arg(QFileInfo(filePath).fileName()));
}

void CabinetDialog::onPlayVideoClicked()
{
    if (m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_mediaPlayer->pause();
        m_btnPlayVideo->setText(tr("Lecture"));
    } else {
        m_mediaPlayer->play();
        m_btnPlayVideo->setText(tr("Pause"));
    }
}

void CabinetDialog::onStopVideoClicked()
{
    m_mediaPlayer->stop();
    m_btnPlayVideo->setText(tr("Lecture"));
}
