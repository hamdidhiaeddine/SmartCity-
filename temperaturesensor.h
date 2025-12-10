#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class TemperatureSensor : public QObject
{
    Q_OBJECT

public:
    explicit TemperatureSensor(QObject *parent = nullptr);
    ~TemperatureSensor();

    bool connecter(const QString &portName);
    void deconnecter();
    bool isConnected() const;
    
    float getTemperature() const { return m_temperature; }
    float getHumidite() const { return m_humidite; }

signals:
    void temperatureRecue(float temperature, float humidite);
    void alerteArrosage(const QStringList &jardinsAArroser);
    void erreurConnexion(const QString &message);

private slots:
    void lireDonnees();
    void verifierJardins();

private:
    QSerialPort *m_serialPort;
    QTimer *m_timer;
    float m_temperature;
    float m_humidite;
    QString m_buffer;
    
    QStringList obtenirJardinsAArroser();
};

#endif // TEMPERATURESENSOR_H
