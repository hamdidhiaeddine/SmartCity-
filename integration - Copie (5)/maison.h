#ifndef MAISON_H
#define MAISON_H

#include <QObject>
#include <QString>
#include <QSqlQueryModel>
#include <QMap>
#include <QTableWidget>

class Maison : public QObject
{
    Q_OBJECT

public:
    explicit Maison(QObject *parent = nullptr);

    Maison(int id, const QString &adresse, int securite, const QString &statut);
    Maison(int id, const QString &adresse, int securite, const QString &statut,
           const QString &type, int nbrPieces);
    Maison(const QString &adresse, const QString &statut, const QString &securiteText,
           int nbrPieces, const QString &type);

    // Getters
    int getId() const;
    QString getAdresse() const;
    int getSecurite() const;
    QString getStatut() const;
    QString getType() const;
    int getNbrPieces() const;

    // Setters
    void setId(int id);
    void setAdresse(const QString &adresse);
    void setSecurite(int securite);
    void setStatut(const QString &statut);
    void setType(const QString &type);
    void setNbrPieces(int nbrPieces);

    // CRUD
    bool ajouter();
    bool ajouter(int idOverride, QString *errorText);
    bool modifier(int id);
    bool modifier(int oldId, int newId, QString *errorText);
    bool supprimer(int id);
    bool supprimer(int id, QString *errorText);
    bool rechercher(int id);
    static QSqlQueryModel* afficher();
    static void afficher(QTableWidget *table);
    static QSqlQueryModel* afficher_selon_niv_sec_2();

    // Statistiques
    QMap<QString,int> getStatistiquesStatus();
    QMap<QString,int> getStatistiquesNiveauSecurite();

    // Validation & helpers
    static bool validateAdresse(const QString &adresse, QString &error);
    static bool validateSecurite(const QString &securiteText, QString &error);
    static bool validateStatut(const QString &statut, QString &error);
    static bool validateType(const QString &type, QString &error);
    static bool validateNbrPieces(int nbrPieces, QString &error);
    static bool idExists(int id);
    static QStringList getResidentsParMaison(int idMaison);
    static bool assignerResidentAMaison(const QString &idResident, int idMaison);

private:
    int id;
    QString adresse;
    int securite;
    QString statut;
    QString type;
    int nbrPieces;
};

#endif // MAISON_H
