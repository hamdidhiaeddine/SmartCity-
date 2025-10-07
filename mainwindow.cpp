#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPixmap>
#include <QFile>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // === COMPREHENSIVE LOGO DEBUGGING ===
    qDebug() << "=== LOGO DEBUGGING START ===";

    // 1. Check if file exists physically
    QString physicalPath = "C:/Users/hamdi/OneDrive/Documents/menuges/Resources/images/Lg.png";
    QFile physicalFile(physicalPath);
    qDebug() << "Physical file exists:" << physicalFile.exists() << "at:" << physicalPath;

    // 2. Check all possible resource paths
    QStringList resourcePaths = {
        ":/Resources/image/Lg.png",
        ":/image/Lg.png",
        ":/Lg.png",
        "qrc:/Resources/image/Lg.png",
        "qrc:/image/Lg.png"
    };

    for(const QString &path : resourcePaths) {
        qDebug() << "Resource path" << path << "exists:" << QFile::exists(path);
    }

    // 3. Try to load the pixmap with different paths
    QPixmap logo;
    QString successfulPath;

    for(const QString &path : resourcePaths) {
        if(QFile::exists(path)) {
            logo.load(path);
            if(!logo.isNull()) {
                successfulPath = path;
                qDebug() << "Successfully loaded logo from:" << path;
                qDebug() << "Logo size:" << logo.size();
                break;
            }
        }
    }

    // 4. If resource loading failed, try physical path
    if(logo.isNull() && physicalFile.exists()) {
        logo.load(physicalPath);
        if(!logo.isNull()) {
            successfulPath = physicalPath;
            qDebug() << "Successfully loaded logo from physical path:" << physicalPath;
        }
    }

    // 5. Set the logo if loaded
    if(!logo.isNull()) {
        ui->label_6->setPixmap(logo);
        ui->label_6->setScaledContents(true);
        qDebug() << "Logo set on label_6 successfully";

        // Debug label properties
        qDebug() << "Label_6 size:" << ui->label_6->size();
        qDebug() << "Label_6 is visible:" << ui->label_6->isVisible();
        qDebug() << "Label_6 geometry:" << ui->label_6->geometry();
    } else {
        qDebug() << "FAILED: Could not load logo from any source!";

        // List files in resources directory for debugging
        qDebug() << "Listing files in :/Resources/images/";
        QDir resourceDir(":/Resources/images/");
        if(resourceDir.exists()) {
            QStringList files = resourceDir.entryList();
            qDebug() << "Files in resource dir:" << files;
        } else {
            qDebug() << "Resource directory doesn't exist!";
        }
    }

    qDebug() << "=== LOGO DEBUGGING END ===";

    // Rest of your existing code
    ui->main->setCurrentIndex(0);

    connect(ui->gestionemployes,  &QPushButton::clicked, this, &MainWindow::showEmployees);
    connect(ui->gestionvehicules, &QPushButton::clicked, this, &MainWindow::showVehicules);
    connect(ui->gestionresidents, &QPushButton::clicked, this, &MainWindow::showResidents);
    connect(ui->gestionmaisons,   &QPushButton::clicked, this, &MainWindow::showMaisons);
    connect(ui->gestionjardins,   &QPushButton::clicked, this, &MainWindow::showJardins);
    connect(ui->gestioncabinets,  &QPushButton::clicked, this, &MainWindow::showCabinets);
}

// ... rest of your functions remain the same
MainWindow::~MainWindow() { delete ui; }

void MainWindow::showEmployees() {
    qDebug() << "Employees clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->Employe_2));
}

void MainWindow::showVehicules() {
    qDebug() << "Vehicules clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->vehicule));
}

void MainWindow::showResidents() {
    qDebug() << "Residents clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->resident));
}

void MainWindow::showMaisons() {
    qDebug() << "Maisons clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->maison));
}

void MainWindow::showJardins() {
    qDebug() << "Jardins clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->jardin));
}

void MainWindow::showCabinets() {
    qDebug() << "Cabinets clicked";
    ui->main->setCurrentIndex(ui->main->indexOf(ui->cabinet));
}
