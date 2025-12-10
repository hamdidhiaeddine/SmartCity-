/****************************************************************************
** Meta object code from reading C++ file 'authentification.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../authentification.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'authentification.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSAuthentificationENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSAuthentificationENDCLASS = QtMocHelpers::stringData(
    "Authentification",
    "onConnexionClicked",
    "",
    "onInscriptionClicked",
    "onMotDePasseOublieClicked",
    "onConnexionVisage"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSAuthentificationENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x08,    1 /* Private */,
       3,    0,   39,    2, 0x08,    2 /* Private */,
       4,    0,   40,    2, 0x08,    3 /* Private */,
       5,    0,   41,    2, 0x08,    4 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Authentification::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSAuthentificationENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSAuthentificationENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSAuthentificationENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Authentification, std::true_type>,
        // method 'onConnexionClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onInscriptionClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMotDePasseOublieClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConnexionVisage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Authentification::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Authentification *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onConnexionClicked(); break;
        case 1: _t->onInscriptionClicked(); break;
        case 2: _t->onMotDePasseOublieClicked(); break;
        case 3: _t->onConnexionVisage(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *Authentification::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Authentification::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSAuthentificationENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Authentification::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
