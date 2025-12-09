#ifndef CABINET_H
#define CABINET_H

#include <QString>
#include <QList>
#include <QSqlDatabase>

class QSqlQueryModel;

class Cabinet
{
public:
    Cabinet();
    Cabinet(int id,
            const QString &nom,
            const QString &adresse,
            const QString &specialite,
            const QString &telephone,
            const QString &email,
            int idResident = -1);

    int id() const;
    QString nom() const;
    QString adresse() const;
    QString specialite() const;
    QString telephone() const;
    QString email() const;
    int idResident() const;

    void setId(int value);
    void setNom(const QString &value);
    void setAdresse(const QString &value);
    void setSpecialite(const QString &value);
    void setTelephone(const QString &value);
    void setEmail(const QString &value);
    void setIdResident(int value);

    bool ajouter(QString *errorText = nullptr) const;
    bool modifier(int oldId, QString *errorText = nullptr) const;
    static bool supprimer(int id, QString *errorText = nullptr);
    static bool idExists(int id);
    static QList<Cabinet> fetchAll(QString *errorText = nullptr);

    // Utilitaires pour les vues avancées
    QSqlQueryModel *afficherCabinet() const;
    QSqlQueryModel *rechercherCabinet(const QString &critere, const QString &valeur) const;
    QSqlQueryModel *getStatSpecialite() const;
    bool exporterExcel(const QString &filePath, QSqlQueryModel *model) const;

private:
    static QSqlDatabase database();
    static QString resolveCritereColumn(const QString &critere);

    int m_id;
    QString m_nom;
    QString m_adresse;
    QString m_specialite;
    QString m_telephone;
    QString m_email;
    int m_idResident;
};

#endif // CABINET_H
