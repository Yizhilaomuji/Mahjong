/****************************************************************************
** Meta object code from reading C++ file 'gamengine.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gamengine.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gamengine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
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
struct qt_meta_tag_ZN10GameEngineE_t {};
} // unnamed namespace

template <> constexpr inline auto GameEngine::qt_create_metaobjectdata<qt_meta_tag_ZN10GameEngineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "GameEngine",
        "gameStarted",
        "",
        "phaseChanged",
        "GameEngine::Phase",
        "phase",
        "turnChanged",
        "player",
        "playerDrew",
        "playerDiscarded",
        "Tile",
        "tile",
        "playerPonged",
        "playerKonged",
        "GameEngine::KongType",
        "kongType",
        "playerWon",
        "selfDrawn",
        "fan",
        "dingqueSet",
        "TileSuit",
        "suit",
        "gameOver",
        "logMessage",
        "msg"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'gameStarted'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'phaseChanged'
        QtMocHelpers::SignalData<void(GameEngine::Phase)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'turnChanged'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Signal 'playerDrew'
        QtMocHelpers::SignalData<void(int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Signal 'playerDiscarded'
        QtMocHelpers::SignalData<void(int, Tile)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 }, { 0x80000000 | 10, 11 },
        }}),
        // Signal 'playerPonged'
        QtMocHelpers::SignalData<void(int, Tile)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 }, { 0x80000000 | 10, 11 },
        }}),
        // Signal 'playerKonged'
        QtMocHelpers::SignalData<void(int, Tile, GameEngine::KongType)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 }, { 0x80000000 | 10, 11 }, { 0x80000000 | 14, 15 },
        }}),
        // Signal 'playerWon'
        QtMocHelpers::SignalData<void(int, bool, int)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 }, { QMetaType::Bool, 17 }, { QMetaType::Int, 18 },
        }}),
        // Signal 'dingqueSet'
        QtMocHelpers::SignalData<void(int, TileSuit)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 }, { 0x80000000 | 20, 21 },
        }}),
        // Signal 'gameOver'
        QtMocHelpers::SignalData<void()>(22, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'logMessage'
        QtMocHelpers::SignalData<void(const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GameEngine, qt_meta_tag_ZN10GameEngineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject GameEngine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10GameEngineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10GameEngineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10GameEngineE_t>.metaTypes,
    nullptr
} };

void GameEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GameEngine *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->gameStarted(); break;
        case 1: _t->phaseChanged((*reinterpret_cast<std::add_pointer_t<GameEngine::Phase>>(_a[1]))); break;
        case 2: _t->turnChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->playerDrew((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->playerDiscarded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2]))); break;
        case 5: _t->playerPonged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2]))); break;
        case 6: _t->playerKonged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<GameEngine::KongType>>(_a[3]))); break;
        case 7: _t->playerWon((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 8: _t->dingqueSet((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<TileSuit>>(_a[2]))); break;
        case 9: _t->gameOver(); break;
        case 10: _t->logMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)()>(_a, &GameEngine::gameStarted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(GameEngine::Phase )>(_a, &GameEngine::phaseChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(int )>(_a, &GameEngine::turnChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(int )>(_a, &GameEngine::playerDrew, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(int , Tile )>(_a, &GameEngine::playerDiscarded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(int , Tile )>(_a, &GameEngine::playerPonged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(int , Tile , GameEngine::KongType )>(_a, &GameEngine::playerKonged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(int , bool , int )>(_a, &GameEngine::playerWon, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(int , TileSuit )>(_a, &GameEngine::dingqueSet, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)()>(_a, &GameEngine::gameOver, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameEngine::*)(const QString & )>(_a, &GameEngine::logMessage, 10))
            return;
    }
}

const QMetaObject *GameEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GameEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10GameEngineE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
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
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void GameEngine::turnChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void GameEngine::playerDrew(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void GameEngine::playerDiscarded(int _t1, Tile _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void GameEngine::playerPonged(int _t1, Tile _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void GameEngine::playerKonged(int _t1, Tile _t2, GameEngine::KongType _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2, _t3);
}

// SIGNAL 7
void GameEngine::playerWon(int _t1, bool _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2, _t3);
}

// SIGNAL 8
void GameEngine::dingqueSet(int _t1, TileSuit _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void GameEngine::gameOver()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void GameEngine::logMessage(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}
QT_WARNING_POP
