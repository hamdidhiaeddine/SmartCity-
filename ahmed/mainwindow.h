#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QSortFilterProxyModel>
#include <QNetworkAccessManager>



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
    void on_ajouter_3_clicked();
    void on_modifier_3_clicked();
    void on_supprimer_3_clicked();
    void on_exporter_3_clicked();
    void on_lineEdit_2_textChanged(const QString &text);
    void on_btnTriDate_clicked();
    void on_btnChatbot_clicked();
    void on_btnBackFromChat_clicked();
    void on_btnSendChat_clicked();
    void sendMessageToAzureAI(const QString &message);
    void on_btnRecom_clicked();
    void on_btnBackFromRecom_clicked();
    QString buildMaintenancePromptFromCurrentVehicle() const;
    void sendRecommendationToAzureAI(const QString &message);
    QChartView* createVehiculePieChart();
    void on_pushButton_3_clicked();
    void on_tribox_changed(const QString &mode);

private:
    Ui::MainWindow *ui;
    void connectButtons();
    QString selectedImmat;
    void loadVehicules();
    QNetworkAccessManager *networkManager;

    QSortFilterProxyModel *proxyModel;
    bool triCroissant = true;
    QString processChatMessage(const QString &msg);

};

#endif // MAINWINDOW_H
