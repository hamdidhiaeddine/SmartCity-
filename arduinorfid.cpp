#include "arduinorfid.h"
#include <QDebug>
#include <QTime>
#include <QTimer>

ArduinoRFID::ArduinoRFID(QObject *parent)
    : QObject(parent)
    , serialPort(nullptr)
{
}

ArduinoRFID::~ArduinoRFID()
{
    disconnectArduino();
}

bool ArduinoRFID::connectArduino(const QString &portName)
{
    qDebug() << "";
    qDebug() << "🔌 Tentative de connexion Arduino...";
    
    if (serialPort && serialPort->isOpen()) {
        qDebug() << "✓ Arduino déjà connecté sur" << serialPort->portName();
        return true;
    }
    
    // Déterminer le port à utiliser
    QString selectedPort = portName;
    if (selectedPort.isEmpty()) {
        qDebug() << "🔍 Recherche automatique d'Arduino...";
        
        // Chercher automatiquement un Arduino
        QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
        qDebug() << "📡 Nombre de ports série détectés:" << availablePorts.size();
        
        for (const QSerialPortInfo &info : availablePorts) {
            qDebug() << "   Port:" << info.portName()
                     << "| Description:" << info.description()
                     << "| Fabricant:" << info.manufacturer();
            
            if (info.description().contains("Arduino", Qt::CaseInsensitive) ||
                info.manufacturer().contains("Arduino", Qt::CaseInsensitive)) {
                selectedPort = info.portName();
                qDebug() << "✓ Arduino trouvé automatiquement sur" << selectedPort;
                break;
            }
        }
        
        // Si aucun Arduino trouvé, utiliser le premier port disponible
        if (selectedPort.isEmpty() && !availablePorts.isEmpty()) {
            selectedPort = availablePorts.first().portName();
            qDebug() << "⚠️ Aucun Arduino détecté, utilisation du premier port:" << selectedPort;
        }
    } else {
        qDebug() << "🎯 Port spécifié:" << portName;
    }
    
    if (selectedPort.isEmpty()) {
        QString error = "❌ Aucun port série disponible sur ce système";
        qWarning() << error;
        qWarning() << "💡 Vérifiez que:";
        qWarning() << "   1. L'Arduino est branché via USB";
        qWarning() << "   2. Les drivers USB sont installés";
        qWarning() << "   3. Le module Qt SerialPort est installé";
        emit errorOccurred(error);
        return false;
    }
    
    // Créer et configurer le port série
    serialPort = new QSerialPort(this);
    serialPort->setPortName(selectedPort);
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    
    // Connexion du signal de réception
    connect(serialPort, &QSerialPort::readyRead, this, &ArduinoRFID::readSerialData);
    
    // Ouvrir le port
    qDebug() << "🔓 Tentative d'ouverture du port" << selectedPort << "...";
    if (!serialPort->open(QIODevice::ReadWrite)) {
        QString error = QString("❌ Impossible d'ouvrir le port %1: %2")
                            .arg(selectedPort)
                            .arg(serialPort->errorString());
        qWarning() << error;
        qWarning() << "💡 Causes possibles:";
        qWarning() << "   - Port déjà utilisé par une autre application";
        qWarning() << "   - Permissions insuffisantes";
        qWarning() << "   - Câble USB défectueux";
        emit errorOccurred(error);
        delete serialPort;
        serialPort = nullptr;
        return false;
    }
    
    qDebug() << "========================================";
    qDebug() << "✅ ARDUINO CONNECTÉ AVEC SUCCÈS";
    qDebug() << "   Port:" << selectedPort;
    qDebug() << "   Vitesse: 9600 bauds";
    qDebug() << "   Bits de données: 8";
    qDebug() << "   Parité: Aucune";
    qDebug() << "   Bits d'arrêt: 1";
    qDebug() << "   Contrôle de flux: Aucun";
    qDebug() << "========================================";
    emit connectionStatusChanged(true);
    return true;
}

void ArduinoRFID::disconnectArduino()
{
    if (serialPort) {
        if (serialPort->isOpen()) {
            QString portName = serialPort->portName();
            serialPort->close();
            qDebug() << "🔌 Arduino déconnecté du port" << portName;
        }
        delete serialPort;
        serialPort = nullptr;
        emit connectionStatusChanged(false);
        qDebug() << "✓ Ressources libérées";
    }
}

bool ArduinoRFID::isConnected() const
{
    return serialPort && serialPort->isOpen();
}

QStringList ArduinoRFID::getAvailablePorts()
{
    QStringList ports;
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    
    for (const QSerialPortInfo &info : availablePorts) {
        QString portInfo = info.portName();
        if (!info.description().isEmpty()) {
            portInfo += QString(" (%1)").arg(info.description());
        }
        ports.append(portInfo);
    }
    
    return ports;
}

void ArduinoRFID::readSerialData()
{
    if (!serialPort) return;
    
    // Lire les données disponibles
    QByteArray data = serialPort->readAll();
    serialBuffer.append(QString::fromLatin1(data));
    
    // Traiter les lignes complètes (terminées par \n)
    while (serialBuffer.contains('\n')) {
        int newlinePos = serialBuffer.indexOf('\n');
        QString line = serialBuffer.left(newlinePos).trimmed();
        serialBuffer = serialBuffer.mid(newlinePos + 1);
        
        if (!line.isEmpty()) {
            qDebug() << "";
            qDebug() << "📡 ========================================";
            qDebug() << "   CARTE RFID DÉTECTÉE";
            qDebug() << "   Code:" << line;
            qDebug() << "   Heure:" << QTime::currentTime().toString("HH:mm:ss");
            qDebug() << "========================================";
            emit rfidScanned(line);
        }
    }
}

void ArduinoRFID::simulateScan(const QString &rfidCode)
{
    if (!isConnected()) {
        qWarning() << "Arduino non connecté";
        emit errorOccurred("Arduino non connecté");
        return;
    }
    
    qDebug() << "📡 Simulation scan RFID:" << rfidCode;
    emit rfidScanned(rfidCode);
}

void ArduinoRFID::ouvrirPorte()
{
    if (!serialPort || !serialPort->isOpen()) {
        qWarning() << "❌ Impossible d'ouvrir la porte: Arduino non connecté";
        emit errorOccurred("Arduino non connecté");
        return;
    }
    
    // Envoyer la commande simple: 1 = ouvrir
    QByteArray commande = "1\n";
    qint64 bytesWritten = serialPort->write(commande);
    
    if (bytesWritten == -1) {
        qWarning() << "❌ Erreur d'envoi:" << serialPort->errorString();
        emit errorOccurred("Erreur d'envoi au servo");
    } else {
        serialPort->flush();
        serialPort->waitForBytesWritten(1000);
        qDebug() << "🚪 Porte ouverte - Servo à 90°";
        
        // Attendre 5 secondes puis fermer
        QTimer::singleShot(5000, this, &ArduinoRFID::fermerPorte);
    }
}

void ArduinoRFID::fermerPorte()
{
    if (!serialPort || !serialPort->isOpen()) {
        return;
    }
    
    // Envoyer la commande simple: 0 = fermer
    QByteArray commande = "0\n";
    qint64 bytesWritten = serialPort->write(commande);
    
    if (bytesWritten != -1) {
        serialPort->flush();
        serialPort->waitForBytesWritten(1000);
        qDebug() << "🚪 Porte fermée - Servo à 0°";
    }
}

void ArduinoRFID::setAngleServo(int angle)
{
    if (!serialPort || !serialPort->isOpen()) {
        qWarning() << "❌ Arduino non connecté";
        return;
    }
    
    // Limiter l'angle entre 0 et 180
    angle = qBound(0, angle, 180);
    
    QString commande = QString("S%1\n").arg(angle);
    serialPort->write(commande.toLatin1());
    serialPort->flush();
    
    qDebug() << "🔧 Servo ajusté à" << angle << "°";
}



