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
    
    // Extraire uniquement le numéro de port (ex: "COM8 (Arduino Uno)" -> "COM8")
    if (selectedPort.contains(" (")) {
        selectedPort = selectedPort.split(" (").first();
        qDebug() << "🔧 Port extrait:" << selectedPort << "(depuis" << portName << ")";
    }
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
            // Filtrer les messages servo (SERVO:OPENING, SERVO:OPENED, etc.)
            if (line.startsWith("SERVO:")) {
                qDebug() << "🔧" << line;
            }
            // Message RFID (commence par RFID: suivi d'un code hexadécimal)
            else if (line.startsWith("RFID:")) {
                QString rfidCode = line.mid(5).trimmed(); // Enlever "RFID:"
                
                // Vérifier que c'est un vrai code RFID (hexadécimal, 8 caractères généralement)
                if (rfidCode.length() >= 4 && rfidCode.length() <= 16) {
                    bool isHex = true;
                    for (QChar c : rfidCode) {
                        if (!c.isDigit() && !QString("ABCDEF").contains(c.toUpper())) {
                            isHex = false;
                            break;
                        }
                    }
                    
                    if (isHex) {
                        qDebug() << "";
                        qDebug() << "📡 ========================================";
                        qDebug() << "   CARTE RFID DÉTECTÉE";
                        qDebug() << "   Code:" << rfidCode;
                        qDebug() << "   Heure:" << QTime::currentTime().toString("HH:mm:ss");
                        qDebug() << "========================================";
                        emit rfidScanned(rfidCode);
                    } else {
                        qDebug() << "⚠️ Code RFID invalide ignoré:" << rfidCode;
                    }
                }
            }
            // Message système Arduino (ignorer)
            else if (line.contains("Systeme") || line.contains("attente") || line.contains("pret")) {
                qDebug() << "🤖 Arduino:" << line;
            }
            // Code RFID brut sans préfixe (pour compatibilité ancien code)
            else if (line.length() >= 4 && line.length() <= 16) {
                bool isHex = true;
                for (QChar c : line) {
                    if (!c.isDigit() && !QString("ABCDEF").contains(c.toUpper())) {
                        isHex = false;
                        break;
                    }
                }
                
                if (isHex) {
                    qDebug() << "";
                    qDebug() << "📡 ========================================";
                    qDebug() << "   CARTE RFID DÉTECTÉE";
                    qDebug() << "   Code:" << line;
                    qDebug() << "   Heure:" << QTime::currentTime().toString("HH:mm:ss");
                    qDebug() << "========================================";
                    emit rfidScanned(line);
                }
            }
            // Autres messages (debug uniquement)
            else {
                qDebug() << "📟 Arduino:" << line;
            }
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
    
    // Envoyer la commande OPEN (comme attendu par Arduino)
    QByteArray commande = "OPEN\n";
    qint64 bytesWritten = serialPort->write(commande);
    
    if (bytesWritten == -1) {
        qWarning() << "❌ Erreur d'envoi:" << serialPort->errorString();
        emit errorOccurred("Erreur d'envoi au servo");
    } else {
        serialPort->flush();
        serialPort->waitForBytesWritten(1000);
        qDebug() << "🚪 Commande OPEN envoyée au servo";
        qDebug() << "⏱️ Le servo ouvrira et fermera automatiquement (3 secondes)";
    }
}

void ArduinoRFID::fermerPorte()
{
    if (!serialPort || !serialPort->isOpen()) {
        return;
    }
    
    // Envoyer la commande CLOSE (comme attendu par Arduino)
    QByteArray commande = "CLOSE\n";
    qint64 bytesWritten = serialPort->write(commande);
    
    if (bytesWritten != -1) {
        serialPort->flush();
        serialPort->waitForBytesWritten(1000);
        qDebug() << "🚪 Commande CLOSE envoyée - Servo fermé";
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



