#ifndef FACECAPTUREDIALOG_H
#define FACECAPTUREDIALOG_H

#include <QDialog>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QVideoWidget>

class FaceCaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FaceCaptureDialog(QWidget *parent = nullptr);
    ~FaceCaptureDialog();
    
    QImage getCapturedImage() const { return capturedImage; }

private slots:
    void onSelectImage();
    void onCaptureFromCamera();
    void onImageCaptured(int id, const QImage &preview);
    void onAccept();
    void onCancel();

private:
    QLabel *imageLabel;
    QPushButton *selectButton;
    QPushButton *cameraButton;
    QPushButton *captureButton;
    QPushButton *acceptButton;
    QPushButton *cancelButton;
    QVideoWidget *videoWidget;
    QImage capturedImage;
    QCamera *camera;
    QImageCapture *imageCapture;
    QMediaCaptureSession *captureSession;
    bool cameraMode;
    
    void setupUI();
    void startCamera();
    void stopCamera();
};

#endif // FACECAPTUREDIALOG_H
