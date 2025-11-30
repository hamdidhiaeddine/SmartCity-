#include "vehicule.h"
#include <QSqlError>
#include <QDebug>

Vehicule::Vehicule() {}

Vehicule::Vehicule(QString immat, QString marque, QString modele, QString type, QString etat, QString service, QDate date_maint)
{
    this->immat = immat;
    this->marque = marque;
    this->modele = modele;
    this->type = type;
    this->etat = etat;
    this->service = service;
    this->date_maint = date_maint;
}

// ✅ ADD
bool Vehicule::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO SYSTEM.VEHICULE "
                  "(ID_VEHI, IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT) "
                  "VALUES (SEQ_VEHI.NEXTVAL, :IMMAT, :MARQUE, :MODELE, :TYPE, :ETAT, :SERVICE, :DATE_MAINT)");

    query.bindValue(":IMMAT", immat);
    query.bindValue(":MARQUE", marque);
    query.bindValue(":MODELE", modele);
    query.bindValue(":TYPE", type);
    query.bindValue(":ETAT", etat);
    query.bindValue(":SERVICE", service);
    query.bindValue(":DATE_MAINT", date_maint);

    if (!query.exec()) {
        qDebug() << "❌ SQL Error (ajouter):" << query.lastError().text();
        return false;
    }
    return true;
}

// ✅ DISPLAY
QSqlQuery Vehicule::afficher()
{
    QSqlQuery query;
    query.prepare("SELECT IMMAT, MARQUE, MODELE, TYPE, ETAT, SERVICE, DATE_MAINT FROM SYSTEM.VEHICULE ORDER BY ID_VEHI DESC");
    query.exec();
    return query;
}

/*bool Vehicule::modifier(QString oldImmat)
{
    QSqlQuery query;
    query.prepare("UPDATE SYSTEM.VEHICULE SET "
                  "IMMAT = :IMMAT, MARQUE = :MARQUE, MODELE = :MODELE, TYPE = :TYPE, "
                  "ETAT = :ETAT, SERVICE = :SERVICE, DATE_MAINT = :DATE_MAINT "
                  "WHERE IMMAT = :OLD_IMMAT");

    query.bindValue(":IMMAT", immat);
    query.bindValue(":MARQUE", marque);
    query.bindValue(":MODELE", modele);
    query.bindValue(":TYPE", type);
    query.bindValue(":ETAT", etat);
    query.bindValue(":SERVICE", service);
    query.bindValue(":DATE_MAINT", date_maint);
    query.bindValue(":OLD_IMMAT", oldImmat);

    if (!query.exec()) {
        qDebug() << "❌ SQL Error (modifier):" << query.lastError().text();
        return false;
    }

    // ✅ Force commit in case autocommit is off
    QSqlQuery commitQuery;
    commitQuery.exec("COMMIT");

    qDebug() << "✅ Update done for oldImmat =" << oldImmat << "-> newImmat =" << immat;
    return true;
}
*/
bool Vehicule::modifier(QString oldImmat)
{
    QSqlQuery query;
    query.prepare("UPDATE SYSTEM.VEHICULE SET "
                  "IMMAT = :IMMAT, MARQUE = :MARQUE, MODELE = :MODELE, TYPE = :TYPE, "
                  "ETAT = :ETAT, SERVICE = :SERVICE, DATE_MAINT = :DATE_MAINT "
                  "WHERE IMMAT = :OLD_IMMAT");

    query.bindValue(":IMMAT", immat);
    query.bindValue(":MARQUE", marque);
    query.bindValue(":MODELE", modele);
    query.bindValue(":TYPE", type);
    query.bindValue(":ETAT", etat);
    query.bindValue(":SERVICE", service);
    query.bindValue(":DATE_MAINT", date_maint);
    query.bindValue(":OLD_IMMAT", oldImmat);

    if (!query.exec()) {
        qDebug() << "❌ SQL Error (modifier):" << query.lastError().text();
        return false;
    }

    // ✅ Force commit in case autocommit is off
    QSqlQuery commitQuery;
    commitQuery.exec("COMMIT");

    qDebug() << "✅ Update done for oldImmat =" << oldImmat << "-> newImmat =" << immat;
    return true;
}

/*bool Vehicule::supprimer(QString immat)
{
    QSqlQuery query;
    query.prepare("DELETE FROM SYSTEM.VEHICULE WHERE IMMAT = :IMMAT");
    query.bindValue(":IMMAT", immat);

    if (!query.exec()) {
        qDebug() << "❌ SQL Error (supprimer):" << query.lastError().text();
        return false;
    }
    return true;
}
*/
bool Vehicule::supprimer(QString immat)
{
    QSqlQuery query;
    query.prepare("DELETE FROM SYSTEM.VEHICULE WHERE IMMAT = :IMMAT");
    query.bindValue(":IMMAT", immat);

    if (!query.exec()) {
        qDebug() << "❌ SQL Error (supprimer):" << query.lastError().text();
        return false;
    }

    qDebug() << "✅ Vehicle deleted successfully from DB (immat):" << immat;
    return true;
}
