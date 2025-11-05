#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGestionEmployes();
    void onGestionVehicules();
    void onGestionResidents();
    void onGestionMaisons();
    void onGestionJardins();
    void onGestionCabinets();
    void onDeconnexion();

    // Employés CRUD only
    void onAjouterEmploye();
    void onModifierEmploye();
    void onSupprimerEmploye();
    void onChargerEmployes();
    void onTableSelectionChanged();

private:
    Ui::MainWindow *ui;
    void connectButtons();
    int m_lastInsertedId = -1;
};

#endif // MAINWINDOW_H
