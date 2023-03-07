/****************************************************************************
** Meta object code from reading C++ file 'pm_gui.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../pm_gui/pm_gui.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pm_gui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Widget_t {
    QByteArrayData data[18];
    char stringdata0[265];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widget_t qt_meta_stringdata_Widget = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Widget"
QT_MOC_LITERAL(1, 7, 9), // "slotAbout"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 13), // "slotConnected"
QT_MOC_LITERAL(4, 32, 12), // "slotDataRead"
QT_MOC_LITERAL(5, 45, 16), // "slotDisconnected"
QT_MOC_LITERAL(6, 62, 20), // "slotReadLeakDetector"
QT_MOC_LITERAL(7, 83, 15), // "slotReadLTC2944"
QT_MOC_LITERAL(8, 99, 14), // "slotReadMS5637"
QT_MOC_LITERAL(9, 114, 14), // "slotReadMC3416"
QT_MOC_LITERAL(10, 129, 17), // "slotReadPowerBits"
QT_MOC_LITERAL(11, 147, 16), // "slotReadSettings"
QT_MOC_LITERAL(12, 164, 18), // "slotReadStatusBits"
QT_MOC_LITERAL(13, 183, 16), // "slotReinitialize"
QT_MOC_LITERAL(14, 200, 19), // "slotCalibrateMC3416"
QT_MOC_LITERAL(15, 220, 14), // "slotZeroMC3416"
QT_MOC_LITERAL(16, 235, 16), // "slotPingReceived"
QT_MOC_LITERAL(17, 252, 12) // "slotSetPower"

    },
    "Widget\0slotAbout\0\0slotConnected\0"
    "slotDataRead\0slotDisconnected\0"
    "slotReadLeakDetector\0slotReadLTC2944\0"
    "slotReadMS5637\0slotReadMC3416\0"
    "slotReadPowerBits\0slotReadSettings\0"
    "slotReadStatusBits\0slotReinitialize\0"
    "slotCalibrateMC3416\0slotZeroMC3416\0"
    "slotPingReceived\0slotSetPower"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    0,   95,    2, 0x08 /* Private */,
       4,    1,   96,    2, 0x08 /* Private */,
       5,    0,   99,    2, 0x08 /* Private */,
       6,    0,  100,    2, 0x08 /* Private */,
       7,    0,  101,    2, 0x08 /* Private */,
       8,    0,  102,    2, 0x08 /* Private */,
       9,    0,  103,    2, 0x08 /* Private */,
      10,    0,  104,    2, 0x08 /* Private */,
      11,    0,  105,    2, 0x08 /* Private */,
      12,    0,  106,    2, 0x08 /* Private */,
      13,    0,  107,    2, 0x08 /* Private */,
      14,    0,  108,    2, 0x08 /* Private */,
      15,    0,  109,    2, 0x08 /* Private */,
      16,    0,  110,    2, 0x08 /* Private */,
      17,    1,  111,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    2,
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
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotAbout(); break;
        case 1: _t->slotConnected(); break;
        case 2: _t->slotDataRead((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 3: _t->slotDisconnected(); break;
        case 4: _t->slotReadLeakDetector(); break;
        case 5: _t->slotReadLTC2944(); break;
        case 6: _t->slotReadMS5637(); break;
        case 7: _t->slotReadMC3416(); break;
        case 8: _t->slotReadPowerBits(); break;
        case 9: _t->slotReadSettings(); break;
        case 10: _t->slotReadStatusBits(); break;
        case 11: _t->slotReinitialize(); break;
        case 12: _t->slotCalibrateMC3416(); break;
        case 13: _t->slotZeroMC3416(); break;
        case 14: _t->slotPingReceived(); break;
        case 15: _t->slotSetPower((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Widget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Widget.data,
    qt_meta_data_Widget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Widget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
