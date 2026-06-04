QT += widgets svg

# Force qmake to use Windows cmd.exe instead of MSYS sh.exe for make recipes.
QMAKE_SH = C:/Windows/System32/cmd.exe

# Use the installed MinGW toolchain from Qt Tools and fix GCC search paths.
QMAKE_CC = C:/Qt/Tools/mingw1310_64/bin/gcc.exe
QMAKE_CXX = C:/Qt/Tools/mingw1310_64/bin/g++.exe
QMAKE_LINK = C:/Qt/Tools/mingw1310_64/bin/g++.exe
QMAKE_CXXFLAGS += -B C:/Qt/Tools/mingw1310_64/lib/gcc/x86_64-w64-mingw32/13.1.0/
QMAKE_LFLAGS += -B C:/Qt/Tools/mingw1310_64/lib/gcc/x86_64-w64-mingw32/13.1.0/

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mahjonglogic.cpp \
    trainingwidget.cpp \
    realtimeassistant.cpp \
    opponentanalysis.cpp \
    gamengine.cpp \
    aiplayer.cpp \
    simulationwidget.cpp \
    combatarenawidget.cpp \
    battletrainingwidget.cpp \
    videocapture.cpp

HEADERS += \
    mainwindow.h \
    mahjonglogic.h \
    trainingwidget.h \
    realtimeassistant.h \
    opponentanalysis.h \
    gamengine.h \
    aiplayer.h \
    simulationwidget.h \
    combatarenawidget.h \
    battletrainingwidget.h \
    videocapture.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
