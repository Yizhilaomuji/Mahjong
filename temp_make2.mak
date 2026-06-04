SHELL = cmd.exe
all:
	C:/Qt/Tools/mingw1310_64/bin/g++.exe -c -fno-keep-inline-dllexport -O2 -std=gnu++1z -Wall -Wextra -Wextra -fexceptions -mthreads -DUNICODE -D_UNICODE -DWIN32 -DMINGW_HAS_SECURE_API=1 -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_NEEDS_QMAIN -I. -IC:/Qt/6.11.0/mingw_64/include -IC:/Qt/6.11.0/mingw_64/include/QtWidgets -IC:/Qt/6.11.0/mingw_64/include/QtGui -IC:/Qt/6.11.0/mingw_64/include/QtCore -Irelease -I. -I/include -IC:/Qt/6.11.0/mingw_64/mkspecs/win32-g++ -o release\main.o main.cpp
