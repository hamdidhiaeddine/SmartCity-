// videodialog.h
#ifndef VIDEODIALOG_H
#define VIDEODIALOG_H

#include <QDialog>
#include <QStringList>
#include "metier.h"

QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QVideoWidget)
QT_FORWARD_DECLARE_CLASS(QMediaPlayer)
QT_FORWARD_DECLARE_CLASS(QAudioOutput)

class VideoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VideoDialog(QWidget *parent = nullptr);
    ~VideoDialog() override;

private slots:
    void onCabinetButtonClicked();
    void onAddDoctorClicked();

private:
    void setupUi();
    void playVideoAt(int index);

    Metier m_metier;
    QList<QPushButton*> m_buttons;
    QVideoWidget *m_videoWidget = nullptr;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audio = nullptr;
};

#endif // VIDEODIALOG_H
