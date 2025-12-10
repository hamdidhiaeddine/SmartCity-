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
    
    // Contrôle de la lecture
    void demarrerLecture();
    void arreterLecture();
    bool isReading() const { return m_isReading; }
    
    float getTemperature() const { return m_temperature; }
    float getHumidite() const { return m_humidite; }
    
    // Commandes pompe
    void activerPompe();
    void arreterPompe();
    bool isPompeActive() const { return m_pompeActive; }

signals:
    void temperatureRecue(float temperature, float humidite);
    void alerteArrosage(const QStringList &jardinsAArroser);
    void erreurConnexion(const QString &message);
    void pompeStateChanged(bool active);
    void lectureStateChanged(bool reading);  // Signal pour état lecture

private slots:
    void lireDonnees();
    void verifierJardins();
    void arreterPompeApresDelai();

private:
    QSerialPort *m_serialPort;
    QTimer *m_timer;
    QTimer *m_pompeTimer;  // Timer pour arrêter la pompe après 3s
    float m_temperature;
    float m_humidite;
    QString m_buffer;
    bool m_pompeActive;
    bool m_isReading;  // État de la lecture
    
    QStringList obtenirJardinsAArroser();
    void envoyerCommande(const QString &commande);
};

#endif // TEMPERATURESENSOR_H
