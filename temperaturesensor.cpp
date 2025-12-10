#include "temperaturesensor.h"
#include "jardin.h"
#include <QDebug>
#include <QMessageBox>

TemperatureSensor::TemperatureSensor(QObject *parent)
    : QObject(parent)
    , m_serialPort(new QSerialPort(this))
    , m_timer(new QTimer(this))
    , m_temperature(0.0f)
    , m_humidite(0.0f)
{
    // Timer pour vérifier périodiquement les jardins (toutes les 5 secondes)
    m_timer->setInterval(5000);
    connect(m_timer, &QTimer::timeout, this, &TemperatureSensor::verifierJardins);
}

TemperatureSensor::~TemperatureSensor()
{
    deconnecter();
}

bool TemperatureSensor::connecter(const QString &portName)
{
    // Extraire uniquement le numéro de port (ex: "COM9 (Arduino Uno)" -> "COM9")
    QString cleanPort = portName;
    if (cleanPort.contains(" (")) {
        cleanPort = cleanPort.split(" (").first();
    }
    
    m_serialPort->setPortName(cleanPort);
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadOnly)) {
        connect(m_serialPort, &QSerialPort::readyRead, this, &TemperatureSensor::lireDonnees);
        m_timer->start();
        qDebug() << "🌡️ Capteur DHT11 connecté sur" << cleanPort;
        return true;
    } else {
        emit erreurConnexion(tr("Impossible d'ouvrir le port %1: %2")
                            .arg(cleanPort)
                            .arg(m_serialPort->errorString()));
        return false;
    }
}

void TemperatureSensor::deconnecter()
{
    m_timer->stop();
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        qDebug() << "🌡️ Capteur DHT11 déconnecté";
    }
}

bool TemperatureSensor::isConnected() const
{
    return m_serialPort->isOpen();
}

void TemperatureSensor::lireDonnees()
{
    m_buffer += QString::fromUtf8(m_serialPort->readAll());
    
    // Traiter les lignes complètes
    QStringList lignes = m_buffer.split('\n');
    m_buffer = lignes.takeLast(); // Garder la dernière ligne incomplète
    
    for (const QString &ligne : lignes) {
        QString ligneTrimmed = ligne.trimmed();
        
        // Format attendu: "25.30 °C" ou "65.00 %"
        if (ligneTrimmed.contains("°C")) {
            // C'est la température
            bool ok = false;
            float temp = ligneTrimmed.split(' ').first().toFloat(&ok);
            if (ok) {
                m_temperature = temp;
                qDebug() << "🌡️ Température:" << m_temperature << "°C";
            }
        } else if (ligneTrimmed.contains("%")) {
            // C'est l'humidité
            bool ok = false;
            float hum = ligneTrimmed.split(' ').first().toFloat(&ok);
            if (ok) {
                m_humidite = hum;
                qDebug() << "💧 Humidité:" << m_humidite << "%";
                
                // Émettre le signal avec les nouvelles valeurs
                emit temperatureRecue(m_temperature, m_humidite);
            }
        }
    }
}

void TemperatureSensor::verifierJardins()
{
    qDebug() << "🔍 Vérification jardins - Temp capteur:" << m_temperature << "°C";
    
    if (m_temperature <= 0) {
        qDebug() << "⚠️ Pas de données de température valides";
        return; // Pas encore de données
    }
    
    QStringList jardinsAArroser = obtenirJardinsAArroser();
    
    qDebug() << "🌿" << jardinsAArroser.size() << "jardin(s) nécessitent un arrosage";
    
    if (!jardinsAArroser.isEmpty()) {
        emit alerteArrosage(jardinsAArroser);
        qDebug() << "💦 ÉMISSION ALERTE ARROSAGE:" << jardinsAArroser.size() << "jardin(s) à arroser";
        for (const QString &jardin : jardinsAArroser) {
            qDebug() << "  •" << jardin;
        }
    }
}

QStringList TemperatureSensor::obtenirJardinsAArroser()
{
    QStringList jardinsAArroser;
    
    QString error;
    QList<Jardin> jardins = Jardin::fetchAll(&error);
    
    if (!error.isEmpty()) {
        qWarning() << "❌ Erreur lors de la récupération des jardins:" << error;
        return jardinsAArroser;
    }
    
    qDebug() << "🌿 Nombre de jardins en BD:" << jardins.size();
    
    // Comparer la température du capteur avec chaque jardin
    for (const Jardin &j : jardins) {
        qDebug() << QString("  Jardin #%1: %2 - Sol: %3°C vs Capteur: %4°C")
                    .arg(j.id())
                    .arg(j.emplacement())
                    .arg(j.temperatureMoyenneSol(), 0, 'f', 1)
                    .arg(m_temperature, 0, 'f', 1);
        
        // Si la température ambiante (capteur) est SUPÉRIEURE à la température du sol du jardin
        // cela signifie que le jardin est plus froid et nécessite un arrosage
        if (m_temperature > j.temperatureMoyenneSol()) {
            QString info = QString("Jardin #%1 (%2) - Sol: %3°C < Ambiant: %4°C")
                          .arg(j.id())
                          .arg(j.emplacement())
                          .arg(j.temperatureMoyenneSol(), 0, 'f', 1)
                          .arg(m_temperature, 0, 'f', 1);
            jardinsAArroser.append(info);
            qDebug() << "    ✅ NÉCESSITE ARROSAGE";
        } else {
            qDebug() << "    ❌ Pas besoin d'arrosage";
        }
    }
    
    return jardinsAArroser;
}
