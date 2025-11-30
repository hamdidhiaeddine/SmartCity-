/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtNetwork/QSslError>
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
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
    "onAjouterResident",
    "onModifierResident",
    "onSupprimerResident",
    "onExporterResident",
    "onAjouterJardin",
    "onModifierJardin",
    "onSupprimerJardin",
    "onExporterJardin"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      48,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  302,    2, 0x08,    1 /* Private */,
       3,    0,  303,    2, 0x08,    2 /* Private */,
       4,    0,  304,    2, 0x08,    3 /* Private */,
       5,    0,  305,    2, 0x08,    4 /* Private */,
       6,    0,  306,    2, 0x08,    5 /* Private */,
       7,    0,  307,    2, 0x08,    6 /* Private */,
       8,    0,  308,    2, 0x08,    7 /* Private */,
       9,    0,  309,    2, 0x08,    8 /* Private */,
      10,    0,  310,    2, 0x08,    9 /* Private */,
      11,    0,  311,    2, 0x08,   10 /* Private */,
      12,    0,  312,    2, 0x08,   11 /* Private */,
      13,    0,  313,    2, 0x08,   12 /* Private */,
      14,    0,  314,    2, 0x08,   13 /* Private */,
      15,    0,  315,    2, 0x08,   14 /* Private */,
      16,    0,  316,    2, 0x08,   15 /* Private */,
      17,    0,  317,    2, 0x08,   16 /* Private */,
      18,    0,  318,    2, 0x08,   17 /* Private */,
      19,    0,  319,    2, 0x08,   18 /* Private */,
      20,    0,  320,    2, 0x08,   19 /* Private */,
      21,    0,  321,    2, 0x08,   20 /* Private */,
      22,    0,  322,    2, 0x08,   21 /* Private */,
      23,    0,  323,    2, 0x08,   22 /* Private */,
      24,    0,  324,    2, 0x08,   23 /* Private */,
      25,    0,  325,    2, 0x08,   24 /* Private */,
      26,    0,  326,    2, 0x08,   25 /* Private */,
      27,    0,  327,    2, 0x08,   26 /* Private */,
      28,    0,  328,    2, 0x08,   27 /* Private */,
      29,    0,  329,    2, 0x08,   28 /* Private */,
      30,    1,  330,    2, 0x08,   29 /* Private */,
      32,    0,  333,    2, 0x08,   31 /* Private */,
      33,    0,  334,    2, 0x08,   32 /* Private */,
      34,    0,  335,    2, 0x108,   33 /* Private | MethodIsConst  */,
      35,    1,  336,    2, 0x08,   34 /* Private */,
      36,    0,  339,    2, 0x08,   36 /* Private */,
      38,    0,  340,    2, 0x08,   37 /* Private */,
      39,    1,  341,    2, 0x08,   38 /* Private */,
      41,    0,  344,    2, 0x08,   40 /* Private */,
      42,    0,  345,    2, 0x08,   41 /* Private */,
      43,    0,  346,    2, 0x08,   42 /* Private */,
      44,    0,  347,    2, 0x08,   43 /* Private */,
      45,    0,  348,    2, 0x08,   44 /* Private */,
      46,    0,  349,    2, 0x08,   45 /* Private */,
      47,    0,  350,    2, 0x08,   46 /* Private */,
      48,    0,  351,    2, 0x08,   47 /* Private */,
      49,    0,  352,    2, 0x08,   48 /* Private */,
      50,    0,  353,    2, 0x08,   49 /* Private */,
      51,    0,  354,    2, 0x08,   50 /* Private */,
      52,    0,  355,    2, 0x08,   51 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::QString,
    QMetaType::Void, QMetaType::QString,   31,
    0x80000000 | 37,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   40,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'onGestionEmployes'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGestionVehicules'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGestionResidents'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGestionMaisons'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGestionJardins'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGestionCabinets'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeconnexion'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAjouterEmploye'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onModifierEmploye'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSupprimerEmploye'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onChargerEmployes'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTableSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExporterPDF'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRechercheChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTriEmailChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTriSalaireChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStatistiqueEmploye'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRechercheVehiculeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTriTypeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTriEtatChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ajouter_3_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_modifier_3_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_supprimer_3_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_exporter_3_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnChatbot_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnTriDate_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnBackFromChat_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSendChat_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendMessageToAzureAI'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_btnRecom_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnBackFromRecom_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'buildMaintenancePromptFromCurrentVehicle'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sendRecommendationToAzureAI'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'createVehiculePieChart'
        QtPrivate::TypeAndForceComplete<QChartView *, std::false_type>,
        // method 'on_pushButton_3_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTriBoxChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onAjouterMaison'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onModifierMaison'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSupprimerMaison'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExporterMaison'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAjouterResident'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onModifierResident'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSupprimerResident'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExporterResident'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAjouterJardin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onModifierJardin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSupprimerJardin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExporterJardin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
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
        case 20: _t->on_ajouter_3_clicked(); break;
        case 21: _t->on_modifier_3_clicked(); break;
        case 22: _t->on_supprimer_3_clicked(); break;
        case 23: _t->on_exporter_3_clicked(); break;
        case 24: _t->on_btnChatbot_clicked(); break;
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
        case 40: _t->onAjouterResident(); break;
        case 41: _t->onModifierResident(); break;
        case 42: _t->onSupprimerResident(); break;
        case 43: _t->onExporterResident(); break;
        case 44: _t->onAjouterJardin(); break;
        case 45: _t->onModifierJardin(); break;
        case 46: _t->onSupprimerJardin(); break;
        case 47: _t->onExporterJardin(); break;
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
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 48)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 48;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 48)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 48;
    }
    return _id;
}
QT_WARNING_POP
