/****************************************************************************
** Meta object code from reading C++ file 'simulationwidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../simulationwidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'simulationwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
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
struct qt_meta_tag_ZN16SimulationWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto SimulationWidget::qt_create_metaobjectdata<qt_meta_tag_ZN16SimulationWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SimulationWidget",
        "onNewGame",
        "",
        "onDingqueClicked",
        "TileSuit",
        "suit",
        "onHandTileClicked",
        "index",
        "onPongClicked",
        "onKongClicked",
        "onSelfKongClicked",
        "onWinClicked",
        "onPassClicked",
        "onAutoPilotToggled",
        "player",
        "enabled",
        "onAITimerTick",
        "onGameStarted",
        "onTurnChanged",
        "onPlayerDiscarded",
        "Tile",
        "tile",
        "onPlayerPonged",
        "onPlayerKonged",
        "GameEngine::KongType",
        "kongType",
        "onPlayerWon",
        "selfDrawn",
        "fan",
        "onDingqueSet",
        "onGameOver",
        "onLogMessage",
        "msg"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onNewGame'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDingqueClicked'
        QtMocHelpers::SlotData<void(TileSuit)>(3, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Slot 'onHandTileClicked'
        QtMocHelpers::SlotData<void(int)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Slot 'onPongClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onKongClicked'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSelfKongClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWinClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPassClicked'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAutoPilotToggled'
        QtMocHelpers::SlotData<void(int, bool)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { QMetaType::Bool, 15 },
        }}),
        // Slot 'onAITimerTick'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGameStarted'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTurnChanged'
        QtMocHelpers::SlotData<void(int)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 },
        }}),
        // Slot 'onPlayerDiscarded'
        QtMocHelpers::SlotData<void(int, Tile)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { 0x80000000 | 20, 21 },
        }}),
        // Slot 'onPlayerPonged'
        QtMocHelpers::SlotData<void(int, Tile)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { 0x80000000 | 20, 21 },
        }}),
        // Slot 'onPlayerKonged'
        QtMocHelpers::SlotData<void(int, Tile, GameEngine::KongType)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { 0x80000000 | 20, 21 }, { 0x80000000 | 24, 25 },
        }}),
        // Slot 'onPlayerWon'
        QtMocHelpers::SlotData<void(int, bool, int)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { QMetaType::Bool, 27 }, { QMetaType::Int, 28 },
        }}),
        // Slot 'onDingqueSet'
        QtMocHelpers::SlotData<void(int, TileSuit)>(29, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { 0x80000000 | 4, 5 },
        }}),
        // Slot 'onGameOver'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLogMessage'
        QtMocHelpers::SlotData<void(const QString &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 32 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SimulationWidget, qt_meta_tag_ZN16SimulationWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SimulationWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16SimulationWidgetE_t>.metaTypes,
    nullptr
} };

void SimulationWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SimulationWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onNewGame(); break;
        case 1: _t->onDingqueClicked((*reinterpret_cast<std::add_pointer_t<TileSuit>>(_a[1]))); break;
        case 2: _t->onHandTileClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onPongClicked(); break;
        case 4: _t->onKongClicked(); break;
        case 5: _t->onSelfKongClicked(); break;
        case 6: _t->onWinClicked(); break;
        case 7: _t->onPassClicked(); break;
        case 8: _t->onAutoPilotToggled((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 9: _t->onAITimerTick(); break;
        case 10: _t->onGameStarted(); break;
        case 11: _t->onTurnChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->onPlayerDiscarded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2]))); break;
        case 13: _t->onPlayerPonged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2]))); break;
        case 14: _t->onPlayerKonged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<GameEngine::KongType>>(_a[3]))); break;
        case 15: _t->onPlayerWon((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 16: _t->onDingqueSet((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<TileSuit>>(_a[2]))); break;
        case 17: _t->onGameOver(); break;
        case 18: _t->onLogMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *SimulationWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SimulationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWidgetE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 19;
    }
    return _id;
}
QT_WARNING_POP
