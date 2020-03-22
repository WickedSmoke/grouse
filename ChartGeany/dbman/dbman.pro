TEMPLATE = lib
CONFIG   += release
DEFINES  *= QT_USE_QSTRINGBUILDER 
DEFINES  *= QT_NO_DEBUG_OUTPUT
DEFINES += cgdbman

INCLUDEPATH += \
	../3rdparty/sqlite3/include \
	../include
 
SOURCES += dbman.cpp

HEADERS += \
	../include/defs.h

unix {
    QMAKE_POST_LINK += strip $(TARGET)
    QMAKE_CXXFLAGS_RELEASE -= -g -Wall -O2
    QMAKE_CXXFLAGS_RELEASE += -Os -fomit-frame-pointer
}
