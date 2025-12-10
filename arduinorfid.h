#ifndef ARDUINORFID_H
#define ARDUINORFID_H

#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QSerialPortInfo>

class ArduinoRFID : public QObject
{
    Q_OBJECT

public:
    explicit ArduinoRFID(QObject *parent = nullptr);
    ~ArduinoRFID();
    
    // Connexion et déconnexion
    bool connectArduino(const QString &portName = QString());
    void disconnectArduino();
    bool isConnected() const;
    
    // Obtenir la liste des ports disponibles
    static QStringList getAvailablePorts();
    
    // Simuler un scan RFID (pour tests sans Arduino)
    void simulateScan(const QString &rfidCode);
    
    // Contrôle direct du servo-moteur
    void ouvrirPorte();
    void fermerPorte();
    void setAngleServo(int angle);
    
    // Accès au port série pour commandes manuelles
    QSerialPort* getSerialPort() const { return serialPort; }
    
signals:
    void rfidScanned(const QString &rfidCode);
    void connectionStatusChanged(bool connected);
    void errorOccurred(const QString &error);

private slots:
    void readSerialData();

private:
    QSerialPort *serialPort;
    QString serialBuffer;
};

#endif // ARDUINORFID_H
