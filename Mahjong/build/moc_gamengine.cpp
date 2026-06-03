/****************************************************************************
** Meta object code from reading C++ file 'gamengine.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../gamengine.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gamengine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GameEngine_t {
    QByteArrayData data[25];
    char stringdata0[246];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GameEngine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GameEngine_t qt_meta_stringdata_GameEngine = {
    {
QT_MOC_LITERAL(0, 0, 10), // "GameEngine"
QT_MOC_LITERAL(1, 11, 11), // "gameStarted"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 12), // "phaseChanged"
QT_MOC_LITERAL(4, 37, 17), // "GameEngine::Phase"
QT_MOC_LITERAL(5, 55, 5), // "phase"
QT_MOC_LITERAL(6, 61, 11), // "turnChanged"
QT_MOC_LITERAL(7, 73, 6), // "player"
QT_MOC_LITERAL(8, 80, 10), // "playerDrew"
QT_MOC_LITERAL(9, 91, 15), // "playerDiscarded"
QT_MOC_LITERAL(10, 107, 4), // "Tile"
QT_MOC_LITERAL(11, 112, 4), // "tile"
QT_MOC_LITERAL(12, 117, 12), // "playerPonged"
QT_MOC_LITERAL(13, 130, 12), // "playerKonged"
QT_MOC_LITERAL(14, 143, 20), // "GameEngine::KongType"
QT_MOC_LITERAL(15, 164, 8), // "kongType"
QT_MOC_LITERAL(16, 173, 9), // "playerWon"
QT_MOC_LITERAL(17, 183, 9), // "selfDrawn"
QT_MOC_LITERAL(18, 193, 3), // "fan"
QT_MOC_LITERAL(19, 197, 10), // "dingqueSet"
QT_MOC_LITERAL(20, 208, 8), // "TileSuit"
QT_MOC_LITERAL(21, 217, 4), // "suit"
QT_MOC_LITERAL(22, 222, 8), // "gameOver"
QT_MOC_LITERAL(23, 231, 10), // "logMessage"
QT_MOC_LITERAL(24, 242, 3) // "msg"

    },
    "GameEngine\0gameStarted\0\0phaseChanged\0"
    "GameEngine::Phase\0phase\0turnChanged\0"
    "player\0playerDrew\0playerDiscarded\0"
    "Tile\0tile\0playerPonged\0playerKonged\0"
    "GameEngine::KongType\0kongType\0playerWon\0"
    "selfDrawn\0fan\0dingqueSet\0TileSuit\0"
    "suit\0gameOver\0logMessage\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GameEngine[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    1,   70,    2, 0x06 /* Public */,
       6,    1,   73,    2, 0x06 /* Public */,
       8,    1,   76,    2, 0x06 /* Public */,
       9,    2,   79,    2, 0x06 /* Public */,
      12,    2,   84,    2, 0x06 /* Public */,
      13,    3,   89,    2, 0x06 /* Public */,
      16,    3,   96,    2, 0x06 /* Public */,
      19,    2,  103,    2, 0x06 /* Public */,
      22,    0,  108,    2, 0x06 /* Public */,
      23,    1,  109,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 10,    7,   11,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 10,    7,   11,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 10, 0x80000000 | 14,    7,   11,   15,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::Int,    7,   17,   18,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 20,    7,   21,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   24,

       0        // eod
};

void GameEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GameEngine *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->gameStarted(); break;
        case 1: _t->phaseChanged((*reinterpret_cast< GameEngine::Phase(*)>(_a[1]))); break;
        case 2: _t->turnChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->playerDrew((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->playerDiscarded((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Tile(*)>(_a[2]))); break;
        case 5: _t->playerPonged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Tile(*)>(_a[2]))); break;
        case 6: _t->playerKonged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Tile(*)>(_a[2])),(*reinterpret_cast< GameEngine::KongType(*)>(_a[3]))); break;
        case 7: _t->playerWon((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 8: _t->dingqueSet((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< TileSuit(*)>(_a[2]))); break;
        case 9: _t->gameOver(); break;
        case 10: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GameEngine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::gameStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(GameEngine::Phase );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::phaseChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::turnChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::playerDrew)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(int , Tile );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::playerDiscarded)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(int , Tile );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::playerPonged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(int , Tile , GameEngine::KongType );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::playerKonged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(int , bool , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::playerWon)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(int , TileSuit );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::dingqueSet)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::gameOver)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (GameEngine::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GameEngine::logMessage)) {
                *result = 10;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GameEngine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_GameEngine.data,
    qt_meta_data_GameEngine,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GameEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GameEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GameEngine.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GameEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void GameEngine::gameStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void GameEngine::phaseChanged(GameEngine::Phase _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GameEngine::turnChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GameEngine::playerDrew(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GameEngine::playerDiscarded(int _t1, Tile _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GameEngine::playerPonged(int _t1, Tile _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void GameEngine::playerKonged(int _t1, Tile _t2, GameEngine::KongType _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void GameEngine::playerWon(int _t1, bool _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void GameEngine::dingqueSet(int _t1, TileSuit _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void GameEngine::gameOver()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void GameEngine::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
