#ifndef MAISON_H
#define MAISON_H

#include <QObject>
#include <QString>
#include <QSqlQueryModel>
#include <QMap>

class Maison : public QObject
{
    Q_OBJECT

public:
    explicit Maison(QObject *parent = nullptr);

    Maison(int id, const QString &adresse, int securite, const QString &statut);
    Maison(int id, const QString &adresse, int securite, const QString &statut,
           const QString &type, int nbrPieces);

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
    bool modifier(int id);
    bool supprimer(int id);
    bool rechercher(int id);
    static QSqlQueryModel* afficher();
    static QSqlQueryModel* afficher_selon_niv_sec_2();

    // Statistiques
    QMap<QString,int> getStatistiquesStatus();
    QMap<QString,int> getStatistiquesNiveauSecurite();

private:
    int id;
    QString adresse;
    int securite;
    QString statut;
    QString type;
    int nbrPieces;
};

#endif // MAISON_H
