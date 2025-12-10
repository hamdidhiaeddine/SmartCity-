#ifndef JARDIN_H
#define JARDIN_H

#include <QString>
#include <QList>
#include <QSqlDatabase>

class Jardin
{
public:
    Jardin();
    Jardin(int id,
           const QString &emplacement,
           double superficie,
           const QString &typeSol,
           double temperatureMoyenneSol,
           const QString &typeChoix);

    int id() const;
    QString emplacement() const;
    double superficie() const;
    QString typeSol() const;
    double temperatureMoyenneSol() const;
    QString typeChoix() const;

    void setId(int value);
    void setEmplacement(const QString &value);
    void setSuperficie(double value);
    void setTypeSol(const QString &value);
    void setTemperatureMoyenneSol(double value);
    void setTypeChoix(const QString &value);

    bool ajouter(QString *errorText = nullptr) const;
    bool modifier(int oldId, QString *errorText = nullptr) const;
    static bool supprimer(int id, QString *errorText = nullptr);
    static bool idExists(int id);
    static QList<Jardin> fetchAll(QString *errorText = nullptr);
    static QList<Jardin> fetchAllSortedByType(QString *errorText = nullptr);
    static QList<Jardin> fetchAllSortedById(QString *errorText = nullptr);
    static QList<Jardin> fetchAllSortedBySuperficie(QString *errorText = nullptr);

private:
    static QSqlDatabase database();

    int m_id;
    QString m_emplacement;
    double m_superficie;
    QString m_typeSol;
    double m_temperatureMoyenneSol;
    QString m_typeChoix;
};

#endif // JARDIN_H
