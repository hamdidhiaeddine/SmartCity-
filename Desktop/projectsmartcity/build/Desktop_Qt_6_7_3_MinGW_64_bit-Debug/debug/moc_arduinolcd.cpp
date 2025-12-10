/****************************************************************************
** Meta object code from reading C++ file 'arduinolcd.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../arduinolcd.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'arduinolcd.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSArduinoLCDENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSArduinoLCDENDCLASS = QtMocHelpers::stringData(
    "ArduinoLCD",
    "touchePressee",
    "",
    "touche",
    "donneesEnvoyees",
    "donnees",
    "erreur",
    "message",
    "lireDonneesDisponibles"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSArduinoLCDENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x06,    1 /* Public */,
       4,    1,   41,    2, 0x06,    3 /* Public */,
       6,    1,   44,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,   47,    2, 0x08,    7 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ArduinoLCD::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSArduinoLCDENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSArduinoLCDENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSArduinoLCDENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ArduinoLCD, std::true_type>,
        // method 'touchePressee'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'donneesEnvoyees'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'erreur'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'lireDonneesDisponibles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ArduinoLCD::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ArduinoLCD *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->touchePressee((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->donneesEnvoyees((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->erreur((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->lireDonneesDisponibles(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ArduinoLCD::*)(const QString & );
            if (_t _q_method = &ArduinoLCD::touchePressee; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ArduinoLCD::*)(const QString & );
            if (_t _q_method = &ArduinoLCD::donneesEnvoyees; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ArduinoLCD::*)(const QString & );
            if (_t _q_method = &ArduinoLCD::erreur; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *ArduinoLCD::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ArduinoLCD::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSArduinoLCDENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ArduinoLCD::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void ArduinoLCD::touchePressee(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ArduinoLCD::donneesEnvoyees(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ArduinoLCD::erreur(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
