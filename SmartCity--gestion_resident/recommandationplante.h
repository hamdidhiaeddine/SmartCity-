#ifndef RECOMMANDATIONPLANTE_H
#define RECOMMANDATIONPLANTE_H

#include <QString>
#include <QList>
#include <QSqlDatabase>

/**
 * @brief Modèle pour la table RECOMMANDATION_PLANTE (recommandations de plantes par type de sol)
 * Correspond au schéma SQL complet
 */
class RecommandationPlante
{
public:
    RecommandationPlante();
    RecommandationPlante(int idPlante,
                         const QString &typeSol,
                         const QString &recommandation,
                         const QString &nomPlante = QString(),
                         const QString &compatibilite = "Moyenne",
                         const QString &saison = QString());

    // Getters
    int idPlante() const;
    QString typeSol() const;
    QString recommandation() const;
    QString nomPlante() const;
    QString compatibilite() const;
    QString saison() const;

    // Setters
    void setIdPlante(int value);
    void setTypeSol(const QString &value);
    void setRecommandation(const QString &value);
    void setNomPlante(const QString &value);
    void setCompatibilite(const QString &value);
    void setSaison(const QString &value);

    // CRUD
    bool ajouter(QString *errorText = nullptr) const;
    bool modifier(int oldId, QString *errorText = nullptr) const;
    static bool supprimer(int id, QString *errorText = nullptr);
    static bool idExists(int id);
    static QList<RecommandationPlante> fetchAll(QString *errorText = nullptr);
    static QList<RecommandationPlante> fetchByTypeSol(const QString &typeSol, QString *errorText = nullptr);
    
    // Génération automatique de recommandations aléatoires
    static RecommandationPlante genererRecommandationAleatoire(const QString &typeSol);
    static QList<RecommandationPlante> genererRecommandationsAleatoires(const QString &typeSol, int count = 3);
    
    // Types de sol disponibles
    static QStringList typesDesolDisponibles();
    // Compatibilités disponibles
    static QStringList compatibilitesDisponibles();
    // Saisons disponibles
    static QStringList saisonsDisponibles();
    // Obtenir le prochain ID disponible
    static int getNextId();

private:
    static QSqlDatabase database();

    int m_idPlante;
    QString m_typeSol;
    QString m_recommandation;
    QString m_nomPlante;
    QString m_compatibilite;
    QString m_saison;
};

#endif // RECOMMANDATIONPLANTE_H
