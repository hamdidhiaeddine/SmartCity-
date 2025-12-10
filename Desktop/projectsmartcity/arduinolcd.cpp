#include "arduinolcd.h"
#include <QRegularExpression>

ArduinoLCD::ArduinoLCD(QObject *parent)
    : QObject(parent)
    , m_serialPort(new QSerialPort(this))
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &ArduinoLCD::lireDonneesDisponibles);
}

ArduinoLCD::~ArduinoLCD()
{
    deconnecter();
}

bool ArduinoLCD::connecter(const QString &portName)
{
    if (m_serialPort->isOpen()) {
        deconnecter();
    }

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "✅ Arduino LCD connecté sur" << portName;
        m_buffer.clear();
        return true;
    } else {
        QString error = QString("Erreur de connexion Arduino LCD sur %1: %2")
                        .arg(portName, m_serialPort->errorString());
        qWarning() << error;
        emit erreur(error);
        return false;
    }
}

void ArduinoLCD::deconnecter()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        qDebug() << "Arduino LCD déconnecté";
    }
}

bool ArduinoLCD::estConnecte() const
{
    return m_serialPort->isOpen();
}

void ArduinoLCD::afficherTexte(const QString &ligne1, const QString &ligne2)
{
    if (!m_serialPort->isOpen()) {
        emit erreur("Arduino LCD non connecté");
        return;
    }

    // Limiter à 16 caractères par ligne
    QString l1 = ligne1.left(16);
    QString l2 = ligne2.left(16);

    // Format: LCD|ligne1|ligne2
    QString commande = QString("LCD|%1|%2\n").arg(l1, l2);
    
    qint64 written = m_serialPort->write(commande.toUtf8());
    if (written == -1) {
        emit erreur("Erreur d'écriture sur le port série");
    } else {
        qDebug() << "📟 Envoyé au LCD:" << l1 << "|" << l2;
    }
}

void ArduinoLCD::effacer()
{
    if (!m_serialPort->isOpen()) {
        emit erreur("Arduino LCD non connecté");
        return;
    }

    m_serialPort->write("CLEAR\n");
    qDebug() << "🧹 LCD effacé";
}

void ArduinoLCD::tester()
{
    if (!m_serialPort->isOpen()) {
        emit erreur("Arduino LCD non connecté");
        return;
    }

    m_serialPort->write("TEST\n");
    qDebug() << "🔧 Test LCD envoyé";
}

void ArduinoLCD::lireDonneesDisponibles()
{
    QByteArray data = m_serialPort->readAll();
    
    qDebug() << "🔵 Données brutes reçues (" << data.size() << "bytes):" << data.toHex() << "=" << data;
    
    // Convertir en QString et nettoyer
    QString texte = QString::fromLatin1(data);
    
    // Supprimer tous les caractères nuls et non-imprimables
    QString texteNettoye;
    for (int i = 0; i < texte.length(); i++) {
        QChar c = texte.at(i);
        if (c.isPrint() || c == '\n' || c == '\r') {
            texteNettoye.append(c);
        }
    }
    
    qDebug() << "🔵 Texte nettoyé:" << texteNettoye;
    
    m_buffer.append(texteNettoye);
    qDebug() << "🔵 Buffer actuel:" << m_buffer;

    // Traiter toutes les lignes complètes
    while (m_buffer.contains('\n')) {
        int pos = m_buffer.indexOf('\n');
        QString ligne = m_buffer.left(pos);
        ligne = ligne.replace('\r', "").trimmed();
        m_buffer.remove(0, pos + 1);
        
        qDebug() << "🔵 Ligne extraite:" << ligne;
        
        if (!ligne.isEmpty()) {
            traiterLigne(ligne);
        }
    }
}

void ArduinoLCD::traiterLigne(const QString &ligne)
{
    qDebug() << "📥 Arduino LCD reçu:" << ligne;

    // Format: "Touche : X"
    QRegularExpression rxTouche("Touche\\s*:\\s*(\\S)");
    QRegularExpressionMatch matchTouche = rxTouche.match(ligne);
    if (matchTouche.hasMatch()) {
        QString touche = matchTouche.captured(1);
        qDebug() << "   ➡️ Signal touchePressee émis avec:" << touche;
        emit touchePressee(touche);
        return;
    }
    
    // Format: "ENVOI: XXXXX"
    QRegularExpression rxEnvoi("ENVOI\\s*:\\s*(\\S+)");
    QRegularExpressionMatch matchEnvoi = rxEnvoi.match(ligne);
    if (matchEnvoi.hasMatch()) {
        QString donnees = matchEnvoi.captured(1);
        qDebug() << "   ➡️ Signal donneesEnvoyees émis avec:" << donnees;
        emit donneesEnvoyees(donnees);
        return;
    }
    
    // Messages système
    if (ligne.contains("Pret") || ligne.contains("Ready")) {
        qDebug() << "✅ Système LCD + Keypad prêt";
        return;
    }
    
    qDebug() << "   ℹ️ Message info:" << ligne;
}
