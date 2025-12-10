/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onGestionEmployes",
        "",
        "onGestionVehicules",
        "onGestionResidents",
        "onGestionMaisons",
        "onGestionJardins",
        "onGestionCabinets",
        "onDeconnexion",
        "onAjouterResident",
        "onModifierResident",
        "onSupprimerResident",
        "onExporterResidentsPdf",
        "onResidentSelectionChanged",
        "onRechercherResident",
        "onTriResident",
        "onAfficherStatistiques",
        "afficherStatistiquesCercle",
        "onAfficherHistorique",
        "onExporterHistoriquePdf",
        "onFermerHistorique",
        "onViderHistorique",
        "onAjouterReclamation",
        "onEnvoyerSms",
        "onSmsRecu",
        "from",
        "to",
        "message",
        "dateTime",
        "onAfficherSmsRecus",
        "chargerEmployes",
        "onAjouterEmploye",
        "onModifierEmploye",
        "onSupprimerEmploye",
        "onEmployeSelectionChanged",
        "chargerVehicules",
        "onAjouterVehicule",
        "onModifierVehicule",
        "onSupprimerVehicule",
        "onVehiculeSelectionChanged",
        "chargerMaisons",
        "onAjouterMaison",
        "onModifierMaison",
        "onSupprimerMaison",
        "onMaisonSelectionChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onGestionEmployes'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGestionVehicules'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGestionResidents'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGestionMaisons'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGestionJardins'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGestionCabinets'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeconnexion'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAjouterResident'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifierResident'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSupprimerResident'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExporterResidentsPdf'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onResidentSelectionChanged'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRechercherResident'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriResident'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAfficherStatistiques'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'afficherStatistiquesCercle'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAfficherHistorique'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExporterHistoriquePdf'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFermerHistorique'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onViderHistorique'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAjouterReclamation'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEnvoyerSms'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSmsRecu'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &, const QDateTime &)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 25 }, { QMetaType::QString, 26 }, { QMetaType::QString, 27 }, { QMetaType::QDateTime, 28 },
        }}),
        // Slot 'onAfficherSmsRecus'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'chargerEmployes'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAjouterEmploye'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifierEmploye'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSupprimerEmploye'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEmployeSelectionChanged'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'chargerVehicules'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAjouterVehicule'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifierVehicule'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSupprimerVehicule'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onVehiculeSelectionChanged'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'chargerMaisons'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAjouterMaison'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifierMaison'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSupprimerMaison'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMaisonSelectionChanged'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onGestionEmployes(); break;
        case 1: _t->onGestionVehicules(); break;
        case 2: _t->onGestionResidents(); break;
        case 3: _t->onGestionMaisons(); break;
        case 4: _t->onGestionJardins(); break;
        case 5: _t->onGestionCabinets(); break;
        case 6: _t->onDeconnexion(); break;
        case 7: _t->onAjouterResident(); break;
        case 8: _t->onModifierResident(); break;
        case 9: _t->onSupprimerResident(); break;
        case 10: _t->onExporterResidentsPdf(); break;
        case 11: _t->onResidentSelectionChanged(); break;
        case 12: _t->onRechercherResident(); break;
        case 13: _t->onTriResident(); break;
        case 14: _t->onAfficherStatistiques(); break;
        case 15: _t->afficherStatistiquesCercle(); break;
        case 16: _t->onAfficherHistorique(); break;
        case 17: _t->onExporterHistoriquePdf(); break;
        case 18: _t->onFermerHistorique(); break;
        case 19: _t->onViderHistorique(); break;
        case 20: _t->onAjouterReclamation(); break;
        case 21: _t->onEnvoyerSms(); break;
        case 22: _t->onSmsRecu((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[4]))); break;
        case 23: _t->onAfficherSmsRecus(); break;
        case 24: _t->chargerEmployes(); break;
        case 25: _t->onAjouterEmploye(); break;
        case 26: _t->onModifierEmploye(); break;
        case 27: _t->onSupprimerEmploye(); break;
        case 28: _t->onEmployeSelectionChanged(); break;
        case 29: _t->chargerVehicules(); break;
        case 30: _t->onAjouterVehicule(); break;
        case 31: _t->onModifierVehicule(); break;
        case 32: _t->onSupprimerVehicule(); break;
        case 33: _t->onVehiculeSelectionChanged(); break;
        case 34: _t->chargerMaisons(); break;
        case 35: _t->onAjouterMaison(); break;
        case 36: _t->onModifierMaison(); break;
        case 37: _t->onSupprimerMaison(); break;
        case 38: _t->onMaisonSelectionChanged(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 39)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 39;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 39)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 39;
    }
    return _id;
}
QT_WARNING_POP
