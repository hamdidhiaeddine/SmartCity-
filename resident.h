#ifndef RESIDENT_H
#define RESIDENT_H

#include <QString>
#include <QDate>
#include <QList>

class Resident
{
public:
    Resident() = default;
    Resident(const QString &id,
             const QString &nom,
             const QString &prenom,
             const QDate &dateNaissance,
             const QString &adresse,
             const QString &telephone,
             const QString &email,
             const QString &statut,
             const QString &situation);

    QString id() const;
    QString nom() const;
    QString prenom() const;
    QString nomComplet() const;
    QDate dateNaissance() const;
    QString adresse() const;
    QString telephone() const;
    QString email() const;
    QString statut() const;
    QString situation() const;

    static bool ajouter(const Resident &resident);
    static bool modifier(const Resident &resident);
    static bool supprimer(const QString &id);
    static bool existe(const QString &id);
    static QList<Resident> recupererTout();

private:
    QString m_id;
    QString m_nom;
    QString m_prenom;
    QDate m_dateNaissance;
    QString m_adresse;
    QString m_telephone;
    QString m_email;
    QString m_statut;
    QString m_situation;
};

#endif // RESIDENT_H

