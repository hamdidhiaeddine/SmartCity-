/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
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
        "onAjouterEmploye",
        "onModifierEmploye",
        "onSupprimerEmploye",
        "onChargerEmployes",
        "onTableSelectionChanged",
        "onExporterPDF",
        "onRechercheChanged",
        "onTriEmailChanged",
        "onTriSalaireChanged",
        "onStatistiqueEmploye",
        "onRechercheVehiculeChanged",
        "onTriTypeChanged",
        "onTriEtatChanged",
        "on_ajouter_3_clicked",
        "on_modifier_3_clicked",
        "on_supprimer_3_clicked",
        "on_exporter_3_clicked",
        "on_btnChatbot_clicked",
        "on_btnTriDate_clicked",
        "on_btnBackFromChat_clicked",
        "on_btnSendChat_clicked",
        "sendMessageToAzureAI",
        "message",
        "on_btnRecom_clicked",
        "on_btnBackFromRecom_clicked",
        "buildMaintenancePromptFromCurrentVehicle",
        "sendRecommendationToAzureAI",
        "createVehiculePieChart",
        "QChartView*",
        "on_pushButton_3_clicked",
        "onTriBoxChanged",
        "mode",
        "onAjouterMaison",
        "onModifierMaison",
        "onSupprimerMaison",
        "onExporterMaison",
        "onTableMaisonSelectionChanged",
        "refreshTableMaison",
        "clearMaisonInputs",
        "loadMaisonToInputs",
        "id",
        "onExporterMaisons",
        "exporterMaisonsPDF",
        "exporterMaisonsCSV",
        "onTriMaisonsChanged",
        "index",
        "onRechercheMaisonsChanged",
        "text",
        "onAjouterResident",
        "onModifierResident",
        "onSupprimerResident",
        "onExporterResident",
        "onAjouterJardin",
        "onModifierJardin",
        "onSupprimerJardin",
        "onExporterJardin",
        "on_Alertes_clicked",
        "refreshAlertes",
        "onAfficherCarte",
        "onRetourAlertes",
        "on_statistique_currentIndexChanged"
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
        // Slot 'onAjouterEmploye'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifierEmploye'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSupprimerEmploye'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onChargerEmployes'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTableSelectionChanged'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExporterPDF'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRechercheChanged'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriEmailChanged'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriSalaireChanged'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStatistiqueEmploye'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRechercheVehiculeChanged'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriTypeChanged'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriEtatChanged'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_ajouter_3_clicked'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_modifier_3_clicked'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_supprimer_3_clicked'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_exporter_3_clicked'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnChatbot_clicked'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnTriDate_clicked'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnBackFromChat_clicked'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnSendChat_clicked'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'sendMessageToAzureAI'
        QtMocHelpers::SlotData<void(const QString &)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 31 },
        }}),
        // Slot 'on_btnRecom_clicked'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnBackFromRecom_clicked'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'buildMaintenancePromptFromCurrentVehicle'
        QtMocHelpers::SlotData<QString() const>(34, 2, QMC::AccessPrivate, QMetaType::QString),
        // Slot 'sendRecommendationToAzureAI'
        QtMocHelpers::SlotData<void(const QString &)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 31 },
        }}),
        // Slot 'createVehiculePieChart'
        QtMocHelpers::SlotData<QChartView *()>(36, 2, QMC::AccessPrivate, 0x80000000 | 37),
        // Slot 'on_pushButton_3_clicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriBoxChanged'
        QtMocHelpers::SlotData<void(const QString &)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 40 },
        }}),
        // Slot 'onAjouterMaison'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifierMaison'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSupprimerMaison'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExporterMaison'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTableMaisonSelectionChanged'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshTableMaison'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'clearMaisonInputs'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'loadMaisonToInputs'
        QtMocHelpers::SlotData<void(int)>(48, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 49 },
        }}),
        // Slot 'onExporterMaisons'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'exporterMaisonsPDF'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'exporterMaisonsCSV'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriMaisonsChanged'
        QtMocHelpers::SlotData<void(int)>(53, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 54 },
        }}),
        // Slot 'onRechercheMaisonsChanged'
        QtMocHelpers::SlotData<void(const QString &)>(55, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 56 },
        }}),
        // Slot 'onAjouterResident'
        QtMocHelpers::SlotData<void()>(57, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifierResident'
        QtMocHelpers::SlotData<void()>(58, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSupprimerResident'
        QtMocHelpers::SlotData<void()>(59, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExporterResident'
        QtMocHelpers::SlotData<void()>(60, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAjouterJardin'
        QtMocHelpers::SlotData<void()>(61, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModifierJardin'
        QtMocHelpers::SlotData<void()>(62, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSupprimerJardin'
        QtMocHelpers::SlotData<void()>(63, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExporterJardin'
        QtMocHelpers::SlotData<void()>(64, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_Alertes_clicked'
        QtMocHelpers::SlotData<void()>(65, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshAlertes'
        QtMocHelpers::SlotData<void()>(66, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAfficherCarte'
        QtMocHelpers::SlotData<void()>(67, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRetourAlertes'
        QtMocHelpers::SlotData<void()>(68, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_statistique_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(69, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 54 },
        }}),
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
        case 7: _t->onAjouterEmploye(); break;
        case 8: _t->onModifierEmploye(); break;
        case 9: _t->onSupprimerEmploye(); break;
        case 10: _t->onChargerEmployes(); break;
        case 11: _t->onTableSelectionChanged(); break;
        case 12: _t->onExporterPDF(); break;
        case 13: _t->onRechercheChanged(); break;
        case 14: _t->onTriEmailChanged(); break;
        case 15: _t->onTriSalaireChanged(); break;
        case 16: _t->onStatistiqueEmploye(); break;
        case 17: _t->onRechercheVehiculeChanged(); break;
        case 18: _t->onTriTypeChanged(); break;
        case 19: _t->onTriEtatChanged(); break;
        case 20: _t->on_ajouter_3_clicked(); break;
        case 21: _t->on_modifier_3_clicked(); break;
        case 22: _t->on_supprimer_3_clicked(); break;
        case 23: _t->on_exporter_3_clicked(); break;
        case 24: _t->on_btnChatbot_clicked(); break;
        case 25: _t->on_btnTriDate_clicked(); break;
        case 26: _t->on_btnBackFromChat_clicked(); break;
        case 27: _t->on_btnSendChat_clicked(); break;
        case 28: _t->sendMessageToAzureAI((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 29: _t->on_btnRecom_clicked(); break;
        case 30: _t->on_btnBackFromRecom_clicked(); break;
        case 31: { QString _r = _t->buildMaintenancePromptFromCurrentVehicle();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 32: _t->sendRecommendationToAzureAI((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 33: { QChartView* _r = _t->createVehiculePieChart();
            if (_a[0]) *reinterpret_cast< QChartView**>(_a[0]) = std::move(_r); }  break;
        case 34: _t->on_pushButton_3_clicked(); break;
        case 35: _t->onTriBoxChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 36: _t->onAjouterMaison(); break;
        case 37: _t->onModifierMaison(); break;
        case 38: _t->onSupprimerMaison(); break;
        case 39: _t->onExporterMaison(); break;
        case 40: _t->onTableMaisonSelectionChanged(); break;
        case 41: _t->refreshTableMaison(); break;
        case 42: _t->clearMaisonInputs(); break;
        case 43: _t->loadMaisonToInputs((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 44: _t->onExporterMaisons(); break;
        case 45: _t->exporterMaisonsPDF(); break;
        case 46: _t->exporterMaisonsCSV(); break;
        case 47: _t->onTriMaisonsChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 48: _t->onRechercheMaisonsChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 49: _t->onAjouterResident(); break;
        case 50: _t->onModifierResident(); break;
        case 51: _t->onSupprimerResident(); break;
        case 52: _t->onExporterResident(); break;
        case 53: _t->onAjouterJardin(); break;
        case 54: _t->onModifierJardin(); break;
        case 55: _t->onSupprimerJardin(); break;
        case 56: _t->onExporterJardin(); break;
        case 57: _t->on_Alertes_clicked(); break;
        case 58: _t->refreshAlertes(); break;
        case 59: _t->onAfficherCarte(); break;
        case 60: _t->onRetourAlertes(); break;
        case 61: _t->on_statistique_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
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
        if (_id < 62)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 62;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 62)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 62;
    }
    return _id;
}
QT_WARNING_POP
