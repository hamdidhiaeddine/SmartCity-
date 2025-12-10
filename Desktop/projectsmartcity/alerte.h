#ifndef ALERTE_H
#define ALERTE_H

#include <QObject>
#include <QString>
#include <QSqlQueryModel>
#include <QDate>

class Alerte : public QObject
{
    Q_OBJECT

public:
    explicit Alerte(QObject *parent = nullptr);
    
    // Constructeur complet
    Alerte(int idMaison, const QString &zone, int niveau, const QString &statut);
    
    // Constructeur avec date
    Alerte(int idMaison, const QString &zone, int niveau, const QString &statut, const QDate &dateAlerte);

    // Getters
    int getId() const;
    int getIdMaison() const;
    QString getZone() const;
    int getNiveau() const;
    QString getStatut() const;
    QDate getDateAlerte() const;

    // Setters
    void setId(int id);
    void setIdMaison(int idMaison);
    void setZone(const QString &zone);
    void setNiveau(int niveau);
    void setStatut(const QString &statut);
    void setDateAlerte(const QDate &dateAlerte);

    // CRUD
    bool ajouter();
    bool modifier(int id);
    bool supprimer(int id);
    bool rechercher(int id);
    static QSqlQueryModel* afficher();
    
    // Méthodes utilitaires
    bool marquerCommeTraitee(int id);
    static bool existeAlertePourMaison(int idMaison);

private:
    int id;
    int idMaison;
    QString zone;
    int niveau;
    QString statut;
    QDate dateAlerte;
};

#endif // ALERTE_H

