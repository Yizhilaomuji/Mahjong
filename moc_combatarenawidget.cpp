/****************************************************************************
** Meta object code from reading C++ file 'combatarenawidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "combatarenawidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'combatarenawidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17CombatArenaWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto CombatArenaWidget::qt_create_metaobjectdata<qt_meta_tag_ZN17CombatArenaWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CombatArenaWidget",
        "backRequested",
        "",
        "onNewGame",
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
        "onPeekOpponent",
        "player",
        "onAITimerTick",
        "onManualCapture",
        "onUpdateGameState",
        "onStartVideoCapture",
        "onStartVoiceCapture",
        "onVoiceCommandSubmit",
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
        // Signal 'backRequested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onNewGame'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDingqueClicked'
        QtMocHelpers::SlotData<void(TileSuit)>(4, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Slot 'onHandTileClicked'
        QtMocHelpers::SlotData<void(int)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'onPongClicked'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onKongClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSelfKongClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWinClicked'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPassClicked'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPeekOpponent'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onAITimerTick'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onManualCapture'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUpdateGameState'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartVideoCapture'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartVoiceCapture'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onVoiceCommandSubmit'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGameStarted'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTurnChanged'
        QtMocHelpers::SlotData<void(int)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onPlayerDiscarded'
        QtMocHelpers::SlotData<void(int, Tile)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { 0x80000000 | 25, 26 },
        }}),
        // Slot 'onPlayerPonged'
        QtMocHelpers::SlotData<void(int, Tile)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { 0x80000000 | 25, 26 },
        }}),
        // Slot 'onPlayerKonged'
        QtMocHelpers::SlotData<void(int, Tile, GameEngine::KongType)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { 0x80000000 | 25, 26 }, { 0x80000000 | 29, 30 },
        }}),
        // Slot 'onPlayerWon'
        QtMocHelpers::SlotData<void(int, bool, int)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { QMetaType::Bool, 32 }, { QMetaType::Int, 33 },
        }}),
        // Slot 'onDingqueSet'
        QtMocHelpers::SlotData<void(int, TileSuit)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { 0x80000000 | 5, 6 },
        }}),
        // Slot 'onGameOver'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLogMessage'
        QtMocHelpers::SlotData<void(const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 37 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CombatArenaWidget, qt_meta_tag_ZN17CombatArenaWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CombatArenaWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CombatArenaWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CombatArenaWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17CombatArenaWidgetE_t>.metaTypes,
    nullptr
} };

void CombatArenaWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CombatArenaWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->backRequested(); break;
        case 1: _t->onNewGame(); break;
        case 2: _t->onDingqueClicked((*reinterpret_cast<std::add_pointer_t<TileSuit>>(_a[1]))); break;
        case 3: _t->onHandTileClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->onPongClicked(); break;
        case 5: _t->onKongClicked(); break;
        case 6: _t->onSelfKongClicked(); break;
        case 7: _t->onWinClicked(); break;
        case 8: _t->onPassClicked(); break;
        case 9: _t->onPeekOpponent((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->onAITimerTick(); break;
        case 11: _t->onManualCapture(); break;
        case 12: _t->onUpdateGameState(); break;
        case 13: _t->onStartVideoCapture(); break;
        case 14: _t->onStartVoiceCapture(); break;
        case 15: _t->onVoiceCommandSubmit(); break;
        case 16: _t->onGameStarted(); break;
        case 17: _t->onTurnChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->onPlayerDiscarded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2]))); break;
        case 19: _t->onPlayerPonged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2]))); break;
        case 20: _t->onPlayerKonged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Tile>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<GameEngine::KongType>>(_a[3]))); break;
        case 21: _t->onPlayerWon((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 22: _t->onDingqueSet((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<TileSuit>>(_a[2]))); break;
        case 23: _t->onGameOver(); break;
        case 24: _t->onLogMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CombatArenaWidget::*)()>(_a, &CombatArenaWidget::backRequested, 0))
            return;
    }
}

const QMetaObject *CombatArenaWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CombatArenaWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CombatArenaWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CombatArenaWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void CombatArenaWidget::backRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
