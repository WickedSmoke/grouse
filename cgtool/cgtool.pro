#--------------------------------------------------------
#
# ChartGeany tool: a command line tool for ChartGeany
#
# ChartGeany: chartgeany@gmail.com
#
#--------------------------------------------------------

QT       += core
QT       -= gui
DEFINES  *= QT_NO_DEBUG_OUTPUT
DEFINES  *= QT_NO_WARNING_OUTPUT
DEFINES  *= QT_USE_QSTRINGBUILDER
TEMPLATE  = app
TARGET    = cgtool
PKG_NAME  = chartgeany
OBJECTS_DIR = obj
RESOURCES += cgtool.qrc

CONFIG += release cmdline console

QMAKE_CXXFLAGS += -DCGTOOL

#
# Qt4
equals(QT_MAJOR_VERSION, 4) {
CONFIG += qt release c99 strict_c c++11 strict_c++
unix {
QMAKE_CFLAGS   += -std=c99
QMAKE_CXXFLAGS += -std=c++11
}
*g++:QMAKE_CXXFLAGS +=-Wno-deprecated-copy
}

INCLUDEPATH = include \
              ../database \
              ../3rdparty/sqlite3/include \
              ../3rdparty/simplecrypt \
              ../ChartGeany/include \
              ../ChartGeany/cgscript/include

*g++|*clang {
QMAKE_LFLAGS += -fstack-protector
QMAKE_CFLAGS += -w -fomit-frame-pointer -fstack-protector -fexceptions
QMAKE_CXXFLAGS += -fomit-frame-pointer -fstrict-aliasing
QMAKE_CXXFLAGS += -fstrict-enums -pedantic
QMAKE_CXXFLAGS += -Wall -Wextra -fomit-frame-pointer -fstack-protector
#QMAKE_CXXFLAGS += -g -O0
}

linux* {
LIBS += -ldl
}

macx* {
LIBS += -framework CoreServices
equals(QT_MAJOR_VERSION, 4) {
clang:QMAKE_CXXFLAGS += -stdlib=libc++ -mmacosx-version-min=10.7 -std=c++11
clang:QMAKE_LFLAGS += -mmacosx-version-min=10.7
clang:LIBS += -stdlib=libc++
}
}

unix {
QMAKE_STRIP = echo
QMAKE_POST_LINK += strip $(TARGET)
target.path = /opt/$$PKG_NAME
INSTALLS += target
}

SOURCES += src/cgscript_toolchain.cpp \
           src/compile_module.cpp \
           src/decompile_module.cpp \
           src/control.cpp \
           src/dbfile_info.cpp \
           src/delete_module.cpp \
           src/export_data.cpp \
           src/export_module.cpp \
           src/help.cpp \
           src/import_module.cpp \
           src/list_modules.cpp \
           src/list_symbols.cpp \
           src/main.cpp \
           src/updatedb.cpp \
           ../database/idb.cpp \
           ../database/platformstring.cpp \
           ../database/sqlcb_datafeeds.cpp \
           ../database/sqlcb_dbversion.cpp \
           ../database/sqlcb_modules.cpp \
           ../database/sqlcb_symbol_table.cpp \
           ../database/sqlcb_toolchain.cpp \
           ../3rdparty/simplecrypt/simplecrypt.cpp \
           ../3rdparty/sqlite3/sqlite3_wrapper.c

HEADERS += ../ChartGeany/include/appdata.h \
           ../3rdparty/simplecrypt/simplecrypt.h

#
# libtcc
linux*|win32* {
INCLUDEPATH += ../3rdparty/libtcc
LIBS += -L../3rdparty -ltcc
}
