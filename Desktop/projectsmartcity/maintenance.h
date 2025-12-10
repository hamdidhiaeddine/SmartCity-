#ifndef MAINTENANCE_H
#define MAINTENANCE_H

#include <QString>
#include <QDate>
#include <QList>
#include <QSqlDatabase>

/**
 * @brief Modèle pour la table MAINTENANCE (maintenance prédictive des jardins)
 */
class Maintenance
{
public:
    Maintenance();
    Maintenance(int id,
                int idJardin,
                const QString &typeMaintenance,
                const QString &description,
                const QDate &dateEffectuee,
                const QDate &dateRecommandee,
                const QString &statut,
                const QString &frequence);

    // Getters
    int id() const;
    int idJardin() const;
    QString typeMaintenance() const;
    QString description() const;
    QDate dateEffectuee() const;
    QDate dateRecommandee() const;
    QString statut() const;
    QString frequence() const;

    // Setters
    void setId(int value);
    void setIdJardin(int value);
    void setTypeMaintenance(const QString &value);
    void setDescription(const QString &value);
    void setDateEffectuee(const QDate &value);
    void setDateRecommandee(const QDate &value);
    void setStatut(const QString &value);
    void setFrequence(const QString &value);

    // CRUD
    bool ajouter(QString *errorText = nullptr) const;
    bool modifier(int oldId, QString *errorText = nullptr) const;
    static bool supprimer(int id, QString *errorText = nullptr);
    static bool idExists(int id);
    static QList<Maintenance> fetchAll(QString *errorText = nullptr);
    static QList<Maintenance> fetchByJardin(int idJardin, QString *errorText = nullptr);
    
    // Maintenance prédictive
    static QList<Maintenance> fetchMaintenancePredictive(QString *errorText = nullptr);
    static QList<Maintenance> fetchMaintenancesAVenir(int joursFutur = 30, QString *errorText = nullptr);
    
    // Utilitaires
    static int getNextId();
    static QStringList typesMaintenanceDisponibles();
    static QStringList statutsDisponibles();
    static QStringList frequencesDisponibles();

private:
    static QSqlDatabase database();

    int m_id;
    int m_idJardin;
    QString m_typeMaintenance;
    QString m_description;
    QDate m_dateEffectuee;
    QDate m_dateRecommandee;
    QString m_statut;
    QString m_frequence;
};

#endif // MAINTENANCE_H
