#ifndef MAISON_H
#define MAISON_H

#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>

class Maison
{
private:
    int id_maison;
    QString adresse;
    QString statut;
    QString securite;
    int nbr_pieces;
    QString type;

public:
    Maison();
    Maison(QString adresse, QString statut, QString securite, int nbr_pieces, QString type);

    // CRUD operations
    bool ajouter(int id_maison = -1, QString *errorMessage = nullptr); // -1 = auto-generate ID
    bool modifier(int oldId, int newId = -1, QString *errorMessage = nullptr); // oldId = ID à modifier, newId = nouvel ID (si -1, garde l'ancien)
    bool supprimer(int id, QString *errorMessage = nullptr);
    static void afficher(QTableWidget *table);
    
    // Getters
    QString getAdresse() const { return adresse; }
    QString getStatut() const { return statut; }
    QString getSecurite() const { return securite; }
    int getNbrPieces() const { return nbr_pieces; }
    QString getType() const { return type; }
    
    // Validation
    static bool validateAdresse(const QString &adresse, QString &error);
    static bool validateStatut(const QString &statut, QString &error);
    static bool validateSecurite(const QString &securite, QString &error);
    static bool validateNbrPieces(int nbr_pieces, QString &error);
    static bool validateType(const QString &type, QString &error);
    static bool idExists(int id); // Vérifie si un ID existe déjà
};

#endif // MAISON_H
