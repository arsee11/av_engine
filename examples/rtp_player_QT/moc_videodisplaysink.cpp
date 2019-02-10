/****************************************************************************
** Meta object code from reading C++ file 'videodisplaysink.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "videodisplaysink.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videodisplaysink.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VideoDisplaySink_t {
    QByteArrayData data[8];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VideoDisplaySink_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VideoDisplaySink_t qt_meta_stringdata_VideoDisplaySink = {
    {
QT_MOC_LITERAL(0, 0, 16), // "VideoDisplaySink"
QT_MOC_LITERAL(1, 17, 14), // "recvVideoFrame"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 6), // "uchar*"
QT_MOC_LITERAL(4, 40, 4), // "data"
QT_MOC_LITERAL(5, 45, 3), // "len"
QT_MOC_LITERAL(6, 49, 1), // "w"
QT_MOC_LITERAL(7, 51, 1) // "h"

    },
    "VideoDisplaySink\0recvVideoFrame\0\0"
    "uchar*\0data\0len\0w\0h"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VideoDisplaySink[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int, QMetaType::Int,    4,    5,    6,    7,

       0        // eod
};

void VideoDisplaySink::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VideoDisplaySink *_t = static_cast<VideoDisplaySink *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->recvVideoFrame((*reinterpret_cast< uchar*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (VideoDisplaySink::*_t)(uchar * , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoDisplaySink::recvVideoFrame)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject VideoDisplaySink::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VideoDisplaySink.data,
      qt_meta_data_VideoDisplaySink,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *VideoDisplaySink::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VideoDisplaySink::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VideoDisplaySink.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Sink<AVParam>"))
        return static_cast< Sink<AVParam>*>(this);
    return QObject::qt_metacast(_clname);
}

int VideoDisplaySink::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void VideoDisplaySink::recvVideoFrame(uchar * _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
