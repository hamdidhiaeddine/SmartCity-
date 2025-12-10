// videodialog.cpp
#include "videodialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QCoreApplication>
#include <QDebug>

#include <QVideoWidget>
#include <QMediaPlayer>
#include <QAudioOutput>

static QString findPathTxt()
{
    // try several likely locations: application dir, current dir, and parents
    QStringList candidates;
    QString appDir = QCoreApplication::applicationDirPath();
    candidates << QDir::cleanPath(appDir + "/path.txt");
    candidates << QDir::cleanPath(QDir::current().filePath("path.txt"));

    QDir d(appDir);
    // walk up several levels
    for (int i = 0; i < 5; ++i) {
        candidates << QDir::cleanPath(d.filePath("path.txt"));
        d.cdUp();
    }

    for (const QString &c : candidates) {
        if (QFile::exists(c)) {
            // if found somewhere other than current working dir, copy to current dir for runtime convenience
            QString target = QDir::current().filePath("path.txt");
            if (QDir::cleanPath(c) != QDir::cleanPath(target)) {
                // overwrite existing in current dir
                if (QFile::exists(target))
                    QFile::remove(target);
                if (QFile::copy(c, target)) {
                    qDebug() << "Copied" << c << "to" << target;
                    return target;
                }
                // if copy failed, still return original path
            }
            return c;
        }
    }
    // try resource path
    if (QFile::exists(":/path.txt"))
        return QString(":/path.txt");
    return {};
}

VideoDialog::VideoDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Vidéos de Jardins - SmartCity");
    resize(1000, 700);

    // Find path.txt in likely places and init Metier with the containing folder
    QString pathsFile = findPathTxt();
    if (pathsFile.isEmpty()) {
        // No path.txt found — initialize an empty/default path file in the working directory
        m_metier.initializeFilePath("path.txt");
        if (!m_metier.loadPaths(m_metier.mediaListFilePath())) {
            QMessageBox::warning(this, "Erreur", "Impossible de charger/initialiser path.txt. Placez path.txt dans le dossier du projet ou dans le dossier de l'exécutable.");
        }
        // try to ensure vd/ content exists in working dir
        if (!m_metier.ensureVdInWorkingDir()) {
            qDebug() << "VideoDialog: ensureVdInWorkingDir failed or found no files";
        }
    } else {
        QString dir = QFileInfo(pathsFile).absolutePath();
        m_metier = Metier(dir);
        if (!m_metier.loadPaths(pathsFile)) {
            // attempt to create a working copy
            m_metier.initializeFilePath("path.txt");
            if (!m_metier.loadPaths(m_metier.mediaListFilePath())) {
                QMessageBox::warning(this, "Erreur", QString("Impossible de charger %1").arg(pathsFile));
            }
            if (!m_metier.ensureVdInWorkingDir()) {
                qDebug() << "VideoDialog: ensureVdInWorkingDir after copying path failed";
            }
        }
    }

        setupUi();

        // create media objects and use the video widget created in setupUi
        m_player = new QMediaPlayer(this);
        m_audio = new QAudioOutput(this);
        // ensure the video widget expands to fill the available space and keeps aspect ratio
        if (m_videoWidget) {
        m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        m_videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    #endif
        m_player->setVideoOutput(m_videoWidget);
        }
        m_player->setAudioOutput(m_audio);
}

VideoDialog::~VideoDialog()
{
    if (m_player)
        m_player->stop();
}

void VideoDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    // top: grid of jardin buttons
    QWidget *topWidget = new QWidget(this);
    auto *grid = new QGridLayout(topWidget);

    QStringList names = m_metier.cabinetNames();
    if (names.isEmpty()) {
        names = QStringList{"Jardin 1","Jardin 2","Jardin 3","Jardin 4"};
    }

    int cols = 4;
    for (int i = 0; i < names.size(); ++i) {
        QString label = names.at(i);
        QPushButton *btn = new QPushButton(label, this);
        btn->setProperty("index", i);
        btn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px; border-radius: 5px; font-weight: bold; }"
                          "QPushButton:hover { background-color: #45a049; }");
        connect(btn, &QPushButton::clicked, this, &VideoDialog::onCabinetButtonClicked);
        m_buttons.append(btn);
        int row = i / cols;
        int col = i % cols;
        grid->addWidget(btn, row, col);
    }

    // add 'Générer Carte de Visite' button
    QPushButton *addDoc = new QPushButton("Générer Carte de Visite Jardinier", this);
    addDoc->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 10px; border-radius: 5px; font-weight: bold; }"
                         "QPushButton:hover { background-color: #0b7dda; }");
    connect(addDoc, &QPushButton::clicked, this, &VideoDialog::onAddDoctorClicked);
    grid->addWidget(addDoc, (names.size()+cols-1)/cols, 0, 1, cols);

    mainLayout->addWidget(topWidget, 0);

    // center: video widget (fills central area)
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setMinimumHeight(420);
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
#endif
    m_videoWidget->setStyleSheet("background-color: black;");
    mainLayout->addWidget(m_videoWidget, 1);

    // bottom: basic controls
    QWidget *controls = new QWidget(this);
    auto *h = new QHBoxLayout(controls);
    QPushButton *play = new QPushButton("▶ Lecture", this);
    QPushButton *pause = new QPushButton("⏸ Pause", this);
    QPushButton *stop = new QPushButton("⏹ Arrêt", this);
    
    play->setStyleSheet("QPushButton { padding: 8px 15px; }");
    pause->setStyleSheet("QPushButton { padding: 8px 15px; }");
    stop->setStyleSheet("QPushButton { padding: 8px 15px; }");
    
    h->addWidget(play);
    h->addWidget(pause);
    h->addWidget(stop);
    h->addStretch();
    mainLayout->addWidget(controls, 0);

    connect(play, &QPushButton::clicked, [this]() { if (m_player) m_player->play(); });
    connect(pause, &QPushButton::clicked, [this]() { if (m_player) m_player->pause(); });
    connect(stop, &QPushButton::clicked, [this]() { if (m_player) m_player->stop(); });
}

void VideoDialog::onCabinetButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;
    bool ok;
    int idx = btn->property("index").toInt(&ok);
    if (!ok) return;

    playVideoAt(idx);
}

void VideoDialog::playVideoAt(int index)
{
    // Try to obtain an existing absolute path using Metier helper
    QStringList tried;
    QString resolved = m_metier.resolvedVideoPathForIndex(index, &tried);
    if (resolved.isEmpty()) {
        // give a friendlier message showing where we looked (first few candidates)
        QString details;
        int shown = qMin(8, tried.size());
        for (int i = 0; i < shown; ++i) {
            details += QString("%1\n").arg(tried.at(i));
        }
        if (tried.size() > shown)
            details += QString("... and %1 more candidates\n").arg(tried.size() - shown);

        QMessageBox::warning(this, "Fichier manquant",
                             QString("La vidéo pour l'index %1 n'a pas été trouvée. Chemins testés:\n%2").arg(index).arg(details));
        return;
    }

    qDebug() << "Resolved video path:" << resolved;
    QUrl url;
    // If it's a resource path (:/...), create a qrc: URL that QMediaPlayer understands
    if (resolved.startsWith(":/")) {
        // convert :/vd/vd1.mp4 -> qrc:/vd/vd1.mp4
        QString qrcUrl = QString("qrc:%1").arg(resolved.mid(1));
        url = QUrl(qrcUrl);
    } else {
        url = QUrl::fromLocalFile(resolved);
    }
    m_player->setSource(url);
    m_audio->setVolume(1.0);
    m_player->play();
}

void VideoDialog::onAddDoctorClicked()
{
    // Demander le nom du jardinier/responsable
    QString docName = QInputDialog::getText(this, "Carte de Visite", "Nom du Jardinier/Responsable :");
    if (docName.trimmed().isEmpty())
        return;

    // Demander la spécialité
    QString specialite = QInputDialog::getText(this, "Carte de Visite", "Spécialité (ex: Jardinage, Arrosage, Maintenance) :");
    if (specialite.trimmed().isEmpty())
        specialite = "Jardinier";

    // Choisir jardin
    QStringList names = m_metier.cabinetNames();
    QString cabinet = names.isEmpty() ? QString("Jardin Public") : names.first();
    if (names.size() > 1) {
        bool okCab;
        cabinet = QInputDialog::getItem(this, "Carte de Visite", "Jardin :", names, 0, false, &okCab);
        if (!okCab || cabinet.isEmpty())
            cabinet = names.first();
    }

    // Demander email
    QString email = QInputDialog::getText(this, "Carte de Visite", "Email :");
    if (email.trimmed().isEmpty())
        email = QString("%1@smartcity-jardins.tn").arg(docName.simplified().replace(' ', '.').toLower());

    // Demander téléphone
    QString telephone = QInputDialog::getText(this, "Carte de Visite", "Téléphone :");
    if (telephone.trimmed().isEmpty())
        telephone = "+216 XX XXX XXX";

    // Demander adresse
    QString adresse = QInputDialog::getText(this, "Carte de Visite", "Adresse :");
    if (adresse.trimmed().isEmpty())
        adresse = "SmartCity, Tunisie";

    // Choisir format de sortie
    QStringList formats{ "Image (PNG)", "PDF" };
    bool okf;
    QString choice = QInputDialog::getItem(this, "Format de sortie", "Choisir format:", formats, 0, false, &okf);
    if (!okf || choice.isEmpty())
        return;

    // Créer le dossier de sortie
    QDir outdir(QDir::current().filePath("cartes_visite_jardins"));
    if (!outdir.exists()) outdir.mkpath(".");

    QString sanitizedName = docName.simplified().replace(' ', '_');

    if (choice.startsWith("Image")) {
        QString out = outdir.filePath(QString("carte_visite_jardin_%1.png").arg(sanitizedName));
        if (m_metier.addDoctorCard(docName, specialite, cabinet, email, telephone, adresse, out)) {
            QMessageBox::information(this, "Carte de Visite créée", 
                QString("Carte de visite créée avec succès!\n\nFichier : %1").arg(out));
        } else {
            QMessageBox::warning(this, "Erreur", 
                "Impossible de créer l'image. Tentative de création en PDF...");
            // fallback to PDF
            QString outPdf = outdir.filePath(QString("carte_visite_jardin_%1.pdf").arg(sanitizedName));
            if (m_metier.addDoctorPdf(docName, specialite, cabinet, email, telephone, adresse, outPdf)) {
                QMessageBox::information(this, "Carte de Visite créée (PDF)", 
                    QString("PDF créé : %1").arg(outPdf));
            } else {
                QMessageBox::warning(this, "Erreur", 
                    "Impossible de créer la carte de visite.");
            }
        }
    } else {
        // PDF path
        QString outPdf = outdir.filePath(QString("carte_visite_jardin_%1.pdf").arg(sanitizedName));
        if (m_metier.addDoctorPdf(docName, specialite, cabinet, email, telephone, adresse, outPdf)) {
            QMessageBox::information(this, "Carte de Visite créée (PDF)", 
                QString("Carte de visite PDF créée!\n\nFichier : %1").arg(outPdf));
        } else {
            QMessageBox::warning(this, "Erreur", 
                "Impossible de créer la carte de visite PDF.");
        }
    }
}
