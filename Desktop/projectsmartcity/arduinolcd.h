#ifndef ARDUINOLCD_H
#define ARDUINOLCD_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QDebug>

/**
 * @brief Classe pour gérer l'Arduino LCD I2C + Keypad 4x4
 * 
 * Gère la communication série avec l'Arduino équipé d'un écran LCD I2C
 * et d'un clavier 4x4. Permet d'afficher du texte sur le LCD et de 
 * recevoir les saisies du clavier.
 */
class ArduinoLCD : public QObject
{
    Q_OBJECT

public:
    explicit ArduinoLCD(QObject *parent = nullptr);
    ~ArduinoLCD();

    /**
     * @brief Connecter à l'Arduino sur un port série
     * @param portName Nom du port (ex: "COM3")
     * @return true si connexion réussie
     */
    bool connecter(const QString &portName);
    
    /**
     * @brief Déconnecter l'Arduino
     */
    void deconnecter();
    
    /**
     * @brief Vérifier si l'Arduino est connecté
     * @return true si connecté
     */
    bool estConnecte() const;
    
    /**
     * @brief Afficher du texte sur le LCD (2 lignes max)
     * @param ligne1 Texte de la ligne 1 (max 16 caractères)
     * @param ligne2 Texte de la ligne 2 (max 16 caractères)
     */
    void afficherTexte(const QString &ligne1, const QString &ligne2);
    
    /**
     * @brief Effacer l'écran LCD
     */
    void effacer();
    
    /**
     * @brief Tester l'affichage LCD
     */
    void tester();

signals:
    /**
     * @brief Signal émis quand une touche est pressée sur le clavier
     * @param touche Caractère de la touche pressée
     */
    void touchePressee(const QString &touche);
    
    /**
     * @brief Signal émis quand # est pressé (envoi de données)
     * @param donnees Données saisies (ligne1 + ligne2)
     */
    void donneesEnvoyees(const QString &donnees);
    
    /**
     * @brief Signal émis en cas d'erreur
     * @param message Message d'erreur
     */
    void erreur(const QString &message);

private slots:
    /**
     * @brief Traiter les données reçues de l'Arduino
     */
    void lireDonneesDisponibles();

private:
    QSerialPort *m_serialPort;
    QString m_buffer;  // Buffer pour accumuler les données reçues
    
    /**
     * @brief Traiter une ligne complète reçue de l'Arduino
     * @param ligne Ligne à traiter
     */
    void traiterLigne(const QString &ligne);
};

#endif // ARDUINOLCD_H
