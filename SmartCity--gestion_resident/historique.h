#ifndef HISTORIQUE_H
#define HISTORIQUE_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QList>

class Historique
{
public:
    Historique();
    Historique(const QString &idHistorique, const QString &idResident, const QString &action, const QDateTime &dateAction);

    QString idHistorique() const { return m_idHistorique; }
    QString idResident() const { return m_idResident; }
    QString action() const { return m_action; }
    QDateTime dateAction() const { return m_dateAction; }

    static bool ajouter(const Historique &historique);
    static QList<Historique> recupererTout();
    static QList<Historique> filtrerParAction(const QString &action);
    static void vider();
    static int obtenirProchainIdHistorique();

private:
    QString m_idHistorique;
    QString m_idResident;
    QString m_action;
    QDateTime m_dateAction;
};

#endif // HISTORIQUE_H

