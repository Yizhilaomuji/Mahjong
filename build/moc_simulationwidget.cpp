/****************************************************************************
** Meta object code from reading C++ file 'simulationwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../simulationwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'simulationwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SimulationWidget_t {
    QByteArrayData data[33];
    char stringdata0[387];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SimulationWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SimulationWidget_t qt_meta_stringdata_SimulationWidget = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SimulationWidget"
QT_MOC_LITERAL(1, 17, 9), // "onNewGame"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "onDingqueClicked"
QT_MOC_LITERAL(4, 45, 8), // "TileSuit"
QT_MOC_LITERAL(5, 54, 4), // "suit"
QT_MOC_LITERAL(6, 59, 17), // "onHandTileClicked"
QT_MOC_LITERAL(7, 77, 5), // "index"
QT_MOC_LITERAL(8, 83, 13), // "onPongClicked"
QT_MOC_LITERAL(9, 97, 13), // "onKongClicked"
QT_MOC_LITERAL(10, 111, 17), // "onSelfKongClicked"
QT_MOC_LITERAL(11, 129, 12), // "onWinClicked"
QT_MOC_LITERAL(12, 142, 13), // "onPassClicked"
QT_MOC_LITERAL(13, 156, 18), // "onAutoPilotToggled"
QT_MOC_LITERAL(14, 175, 6), // "player"
QT_MOC_LITERAL(15, 182, 7), // "enabled"
QT_MOC_LITERAL(16, 190, 13), // "onAITimerTick"
QT_MOC_LITERAL(17, 204, 13), // "onGameStarted"
QT_MOC_LITERAL(18, 218, 13), // "onTurnChanged"
QT_MOC_LITERAL(19, 232, 17), // "onPlayerDiscarded"
QT_MOC_LITERAL(20, 250, 4), // "Tile"
QT_MOC_LITERAL(21, 255, 4), // "tile"
QT_MOC_LITERAL(22, 260, 14), // "onPlayerPonged"
QT_MOC_LITERAL(23, 275, 14), // "onPlayerKonged"
QT_MOC_LITERAL(24, 290, 20), // "GameEngine::KongType"
QT_MOC_LITERAL(25, 311, 8), // "kongType"
QT_MOC_LITERAL(26, 320, 11), // "onPlayerWon"
QT_MOC_LITERAL(27, 332, 9), // "selfDrawn"
QT_MOC_LITERAL(28, 342, 3), // "fan"
QT_MOC_LITERAL(29, 346, 12), // "onDingqueSet"
QT_MOC_LITERAL(30, 359, 10), // "onGameOver"
QT_MOC_LITERAL(31, 370, 12), // "onLogMessage"
QT_MOC_LITERAL(32, 383, 3) // "msg"

    },
    "SimulationWidget\0onNewGame\0\0"
    "onDingqueClicked\0TileSuit\0suit\0"
    "onHandTileClicked\0index\0onPongClicked\0"
    "onKongClicked\0onSelfKongClicked\0"
    "onWinClicked\0onPassClicked\0"
    "onAutoPilotToggled\0player\0enabled\0"
    "onAITimerTick\0onGameStarted\0onTurnChanged\0"
    "onPlayerDiscarded\0Tile\0tile\0onPlayerPonged\0"
    "onPlayerKonged\0GameEngine::KongType\0"
    "kongType\0onPlayerWon\0selfDrawn\0fan\0"
    "onDingqueSet\0onGameOver\0onLogMessage\0"
    "msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SimulationWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x08 /* Private */,
       3,    1,  110,    2, 0x08 /* Private */,
       6,    1,  113,    2, 0x08 /* Private */,
       8,    0,  116,    2, 0x08 /* Private */,
       9,    0,  117,    2, 0x08 /* Private */,
      10,    0,  118,    2, 0x08 /* Private */,
      11,    0,  119,    2, 0x08 /* Private */,
      12,    0,  120,    2, 0x08 /* Private */,
      13,    2,  121,    2, 0x08 /* Private */,
      16,    0,  126,    2, 0x08 /* Private */,
      17,    0,  127,    2, 0x08 /* Private */,
      18,    1,  128,    2, 0x08 /* Private */,
      19,    2,  131,    2, 0x08 /* Private */,
      22,    2,  136,    2, 0x08 /* Private */,
      23,    3,  141,    2, 0x08 /* Private */,
      26,    3,  148,    2, 0x08 /* Private */,
      29,    2,  155,    2, 0x08 /* Private */,
      30,    0,  160,    2, 0x08 /* Private */,
      31,    1,  161,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 20,   14,   21,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 20,   14,   21,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 20, 0x80000000 | 24,   14,   21,   25,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::Int,   14,   27,   28,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4,   14,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   32,

       0        // eod
};

void SimulationWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SimulationWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onNewGame(); break;
        case 1: _t->onDingqueClicked((*reinterpret_cast< TileSuit(*)>(_a[1]))); break;
        case 2: _t->onHandTileClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onPongClicked(); break;
        case 4: _t->onKongClicked(); break;
        case 5: _t->onSelfKongClicked(); break;
        case 6: _t->onWinClicked(); break;
        case 7: _t->onPassClicked(); break;
        case 8: _t->onAutoPilotToggled((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->onAITimerTick(); break;
        case 10: _t->onGameStarted(); break;
        case 11: _t->onTurnChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->onPlayerDiscarded((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Tile(*)>(_a[2]))); break;
        case 13: _t->onPlayerPonged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Tile(*)>(_a[2]))); break;
        case 14: _t->onPlayerKonged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Tile(*)>(_a[2])),(*reinterpret_cast< GameEngine::KongType(*)>(_a[3]))); break;
        case 15: _t->onPlayerWon((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 16: _t->onDingqueSet((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< TileSuit(*)>(_a[2]))); break;
        case 17: _t->onGameOver(); break;
        case 18: _t->onLogMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SimulationWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SimulationWidget.data,
    qt_meta_data_SimulationWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SimulationWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SimulationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SimulationWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SimulationWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
