#ifndef CABINETDIALOG_H
#define CABINETDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QChart>
#include <QChartView>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include "cabinet.h"

class QSqlQueryModel;

class CabinetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CabinetDialog(QWidget *parent = nullptr);
    ~CabinetDialog();

private slots:
    void onAfficherClicked();
    void onRechercheClicked();
    void onStatClicked();
    void onExportExcelClicked();
    void onPlayVideoClicked();
    void onStopVideoClicked();
    void onOpenVideoClicked();

private:
    void setupUI();
    void createStatChart(QSqlQueryModel* model);
    void setupVideoPlayer();
    
    Cabinet* m_cabinet;
    
    QTableView* m_tableView;
    QLineEdit* m_searchInput;
    QComboBox* m_searchCritere;
    QPushButton* m_btnAfficher;
    QPushButton* m_btnRecherche;
    QPushButton* m_btnStat;
    QPushButton* m_btnExportExcel;
    QChartView* m_chartView;
    
    // Video player
    QMediaPlayer* m_mediaPlayer;
    QVideoWidget* m_videoWidget;
    QAudioOutput* m_audioOutput;
    QPushButton* m_btnPlayVideo;
    QPushButton* m_btnStopVideo;
    QPushButton* m_btnOpenVideo;
};

#endif // CABINETDIALOG_H
