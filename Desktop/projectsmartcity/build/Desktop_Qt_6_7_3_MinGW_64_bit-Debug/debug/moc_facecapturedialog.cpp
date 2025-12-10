/****************************************************************************
** Meta object code from reading C++ file 'facecapturedialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../facecapturedialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'facecapturedialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSFaceCaptureDialogENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSFaceCaptureDialogENDCLASS = QtMocHelpers::stringData(
    "FaceCaptureDialog",
    "onSelectImage",
    "",
    "onCaptureFromCamera",
    "onImageCaptured",
    "id",
    "preview",
    "onAccept",
    "onCancel"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSFaceCaptureDialogENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x08,    1 /* Private */,
       3,    0,   45,    2, 0x08,    2 /* Private */,
       4,    2,   46,    2, 0x08,    3 /* Private */,
       7,    0,   51,    2, 0x08,    6 /* Private */,
       8,    0,   52,    2, 0x08,    7 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QImage,    5,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject FaceCaptureDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSFaceCaptureDialogENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSFaceCaptureDialogENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSFaceCaptureDialogENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FaceCaptureDialog, std::true_type>,
        // method 'onSelectImage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCaptureFromCamera'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImageCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QImage &, std::false_type>,
        // method 'onAccept'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void FaceCaptureDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FaceCaptureDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onSelectImage(); break;
        case 1: _t->onCaptureFromCamera(); break;
        case 2: _t->onImageCaptured((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QImage>>(_a[2]))); break;
        case 3: _t->onAccept(); break;
        case 4: _t->onCancel(); break;
        default: ;
        }
    }
}

const QMetaObject *FaceCaptureDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FaceCaptureDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSFaceCaptureDialogENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FaceCaptureDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
