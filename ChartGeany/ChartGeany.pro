#-------------------------------------------------
#
# Project created by QtCreator 2012-02-24T20:18:08
#
#-------------------------------------------------

QT         += core gui network
CONFIG     += qt release
DEFINES    *= QT_USE_QSTRINGBUILDER
TEMPLATE    = app
TARGET      = ChartGeany
PKG_NAME    = chartgeany
UI_DIR      = ui
MOC_DIR     = moc
OBJECTS_DIR = obj
RESOURCES  += resources/ChartGeany.qrc
MAKE_RESOURCE_FLAGS += -threshold 10 -compress 9

#
# global variabes
TCC_ARCH    = TCC_TARGET_I386

#
# distribution
win32:DISTRIBUTION = $$system(systeminfo | findstr /B /C:"OS Name")
unix:DISTRIBUTION = $$system(cat /etc/issue | cut -d\' \' -f1)

#
# Qt4 specific definitions
equals(QT_MAJOR_VERSION, 4) {
QT          += webkit
*g++:QMAKE_CXXFLAGS +=-Wno-deprecated-copy
}

#
# Qt5 specific definitions
equals(QT_MAJOR_VERSION, 5) {
CONFIG += c99 strict_c c++11 strict_c++
QT         += concurrent

lessThan(QT_MINOR_VERSION, 7) {
QT         += webkit widgets webkitwidgets
}

greaterThan(QT_MINOR_VERSION, 6) {
QT          += webenginewidgets
}
}

#
# common QMAKE FLAGS
QMAKE_CXXFLAGS += -DCGSCRIPT_SANITIZER

#
# common gcc flags
*g++:QMAKE_CFLAGS += -fmax-errors=2 -fstrict-aliasing -fexceptions \
                     -fstack-protector

*g++:QMAKE_CXXFLAGS += -fmax-errors=2 -fstrict-aliasing -fexceptions \
                       -fstack-protector -Wall -Wextra

equals(QT_MAJOR_VERSION, 4) {
*g++:QMAKE_CFLAGS   += -std=c99
*g++:QMAKE_CXXFLAGS += -std=gnu++11
}

#
# common clang flags
*clang:QMAKE_CFLAGS += -fstrict-aliasing -fexceptions -fstack-protector

*clang:QMAKE_CXXFLAGS += -fstrict-aliasing -fexceptions -fstack-protector \
                         -Wall -Wextra

equals(QT_MAJOR_VERSION, 4) {
*clang:QMAKE_CFLAGS += -std=c99
*clang:QMAKE_CXXFLAGS += -std=gnu++11
}

#
# platform and compiler flags for win32
win32 {
msvc:CONFIG -= embed_manifest_dll
msvc:QMAKE_CFLAGS_RELEASE += /UDEBUG -D_CRT_SECURE_NO_WARNINGS -DTCC_TARGET_PE
msvc:QMAKE_CXXFLAGS_RELEASE += /UDEBUG -D_CRT_SECURE_NO_WARNINGS

!contains(QMAKE_TARGET.arch, x86_64) {
      LIBS += -L../binaries/msvc32/lib -llibeay32
      INCLUDEPATH += ../binaries/msvc32/include
    } else {
      LIBS += -L../binaries/msvc64/lib -llibeay32
      INCLUDEPATH += ../binaries/msvc64/include
    }

RC_FILE = chartgeany.rc
}

#
# platform and compiler flags for linux
linux* {
contains(QMAKE_HOST.arch, x86_64) {
    TCC_ARCH = TCC_TARGET_X86_64
}

OSDISTRO = $$system(cat /proc/version)

LIBS += -ldl -lxlsreader

QMAKE_CFLAGS += -fomit-frame-pointer -w -D$$TCC_ARCH

QMAKE_CXXFLAGS += -fomit-frame-pointer

QMAKE_STRIP = echo
QMAKE_POST_LINK += strip $(TARGET)

target.path = /opt/$$PKG_NAME
images.path = /opt/$$PKG_NAME
desktop.path = /usr/share/applications/
images.files = images/$${PKG_NAME}.png
desktop.files = $${PKG_NAME}.desktop
INSTALLS += target desktop images
}

#
# platform and compiler flags for macx
macx* {
QMAKE_CFLAGS     -= -fmax-errors=2 -Wall -Wextra
QMAKE_CFLAGS     += -w -fomit-frame-pointer
QMAKE_CXXFLAGS   += -D_XOPEN_SOURCE -fomit-frame-pointer
QMAKE_INFO_PLIST += default.plist
QMAKE_POST_LINK  += strip $(TARGET) &&
QMAKE_POST_LINK  += /bin/cp ../cgtool/cgtool.app/Contents/MacOS/cgtool ./$${TARGET}.app/Contents/MacOS &&
QMAKE_POST_LINK  += /bin/cp -R licenses ./$${TARGET}.app &&
QMAKE_POST_LINK  += '/usr/bin/sed -i -e "s/@VERSION@/$$VERSION/g" "./$${TARGET}.app/Contents/Info.plist"'
ICON = images/chartgeany.icns
BUNDLEID = com.$${TARGET}
LIBS += -framework CoreServices

equals(QT_MAJOR_VERSION, 4) {
*clang:QMAKE_CXXFLAGS += -stdlib=libc++ -mmacosx-version-min=10.7
*clang:QMAKE_LFLAGS   += -mmacosx-version-min=10.7
*clang:LIBS           += -stdlib=libc++
}
}

#
# debug
DEFINES    *= QT_NO_DEBUG_OUTPUT
DEFINES    *= QT_NO_WARNING_OUTPUT
# QMAKE_CFLAGS -= -fomit-frame-pointer -O2
# QMAKE_CFLAGS += -fno-omit-frame-pointer -g3 -O0 -DDEBUG
# QMAKE_CXXFLAGS -= -fomit-frame-pointer -O2
# QMAKE_CXXFLAGS += -fno-omit-frame-pointer -g3 -O0 -DDEBUG

#
# sanitize address
# QMAKE_LFLAGS += -fsanitize=address -O0
# QMAKE_CFLAGS_RELEASE += -ggdb -fsanitize=address -O0 -g -fno-omit-frame-pointer
# QMAKE_CXXFLAGS_RELEASE += -ggdb -fsanitize=address -O0 -g -fno-omit-frame-pointer

#
# sanitize leak
# QMAKE_LFLAGS += -fsanitize=leak -O0
# QMAKE_CFLAGS_RELEASE += -ggdb -fsanitize=leak -O0
# QMAKE_CXXFLAGS_RELEASE += -ggdb -fsanitize=leak -O0 -g

#
# sanitize undefined
# QMAKE_LFLAGS += -fsanitize=leak -O0
# QMAKE_CFLAGS_RELEASE += -ggdb -fsanitize=undefined -O0
# QMAKE_CXXFLAGS_RELEASE += -ggdb -fsanitize=undefined -O0 -g

# include paths common for all Qt versions, platforms and compilers
INCLUDEPATH += \
        include \
        cgscript/include \
        ../gui \
        ../chart \
        ../database \
        ../3rdparty/sqlite3/include \
        ../3rdparty/segvcatch/include \
        ../3rdparty/ta-lib/include \
        ../3rdparty/simplecrypt

# application sources, common for all Qt versions, platforms and compilers
SOURCES += \
        src/appColorDialog.cpp \
        src/addportfoliodialog.cpp \
        src/addpricedialog.cpp \
        src/addtransactiondialog.cpp \
        src/cgscript.c \
        src/cgscript_imp_arrays.cpp \
        src/cgscript_imp_bars.cpp \
        src/cgscript_imp_chart.cpp \
        src/cgscript_imp_common.cpp \
        src/cgscript_imp_errors.cpp \
        src/cgscript_imp_hooks.cpp \
        src/cgscript_imp_object.cpp \
        src/cgscript_imp_strings.cpp \
        src/cgscript_imp_tafunc.cpp \
        src/common.cpp \
        src/compile.cpp \
        src/create_portfolio_views.cpp \
        src/databrowserdialog.cpp \
        src/datamanagerdialog.cpp \
        src/debugdialog.cpp \
        src/downloaddatadialog.cpp \
        src/dynparamsdialog.cpp \
        src/editorwidget.cpp \
        src/highlighter.cpp \
        src/infodialog.cpp \
        src/licensedialog.cpp \
        src/lineobjectdialog.cpp \
        src/loadcsvdialog.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/manualdialog.cpp \
        src/modulemanagerdialog.cpp \
        src/native_segvfpe.cpp \
        src/optionsdialog.cpp \
        src/progressdialog.cpp \
        src/portfoliomanagerdialog.cpp \
        src/portfolio.cpp \
        src/splashdialog.cpp \
        src/symbollistdialog.cpp \
        src/templatemanagerdialog.cpp \
        src/textobjectdialog.cpp \
        src/unix_signals.cpp \
        src/waitdialog.cpp \
        ../gui/StockTicker.cpp \
        ../chart/chartapp.cpp \
        ../chart/function_dataset.cpp \
        ../chart/qtachart_core_barchart.cpp \
        ../chart/qtachart_core_candlechart.cpp \
        ../chart/qtachart_core.cpp \
        ../chart/qtachart_core_drawbar.cpp \
        ../chart/qtachart_core_drawcandle.cpp \
        ../chart/qtachart_core_draw.cpp \
        ../chart/qtachart_core_drawpriceline.cpp \
        ../chart/qtachart.cpp \
        ../chart/qtachart_data.cpp \
        ../chart/qtachart_draw.cpp \
        ../chart/qtachart_eventfilters.cpp \
        ../chart/qtachart_functions.cpp \
        ../chart/qtachart_help.cpp \
        ../chart/qtachart_object.cpp \
        ../chart/qtachart_object_sanitizer.cpp \
        ../chart/qtachart_object_draw.cpp \
        ../chart/qtachart_object_modules.cpp \
        ../chart/qtachart_objects.cpp \
        ../chart/qtachart_properties.cpp \
        ../chart/qtcgraphicsitem.cpp \
        ../database/csv.cpp \
        ../database/dbman.cpp \
        ../database/feedav.cpp \
        ../database/feediex.cpp \
        ../database/feedyahoo.cpp \
        ../database/netservice.cpp \
        ../database/platformstring.cpp \
        ../database/priceupdater.cpp \
        ../database/sqlcb_modules.cpp \
        ../database/sqlcb_symbol_table.cpp \
        ../database/sqlcb_toolchain.cpp \
        ../database/idb.cpp

# application headers, common for all Qt versions, platforms and compilers
HEADERS  += \
        include/appColorDialog.h \
        include/appdata.h \
        include/addportfoliodialog.h \
        include/addpricedialog.h \
        include/addtransactiondialog.h \
        include/common.h \
        include/compile.h \
        include/databrowserdialog.h \
        include/datamanagerdialog.h \
        include/debug.h \
        include/debugdialog.h \
        include/defs.h \
        include/downloaddatadialog.h \
        include/dynparamsdialog.h \
        include/editorwidget.h \
        include/highlighter.h \
        include/infodialog.h \
        include/licensedialog.h \
        include/lineobjectdialog.h \
        include/loadcsvdialog.h \
        include/mainwindow.h \
        include/manualdialog.h \
        include/modulemanagerdialog.h \
        include/optionsdialog.h \
        include/progressdialog.h \
        include/portfoliomanagerdialog.h \
        include/portfolio.h \
        include/splashdialog.h \
        include/symbollistdialog.h \
        include/templatemanagerdialog.h \
        include/textobjectdialog.h \
        include/unix_signals.h \
        include/waitdialog.h \
        ../gui/StockTicker.h \
        ../chart/chartapp.h \
        ../chart/qtachart_core.h \
        ../chart/qtachart_data.h \
        ../chart/qtachart_draw.h \
        ../chart/qtachart_eventfilters.h \
        ../chart/qtachart_functions.h \
        ../chart/qtachart.h \
        ../chart/qtachart_help.h \
        ../chart/qtachart_object.h \
        ../chart/qtachart_objects.h \
        ../chart/qtachart_properties.h \
        ../chart/qtcgraphicsitem.h \
        ../chart/qtcgraphicsscene.h \
        ../database/feedav.h \
        ../database/feediex.h \
        ../database/feedyahoo.h \
        ../database/netservice.h \
        ../database/priceupdater.h


# Qt5 specific sources and headers
equals(QT_MAJOR_VERSION, 5) {
SOURCES += \
        src/natevents.cpp

HEADERS  += \
        include/natevents.h
}

LIBS += -L../3rdparty -lta-lib

# 3rdparty sources common for all Qt versions, platforms and compilers
SOURCES += \
        ../3rdparty/sqlite3/sqlite3_wrapper.c \
        ../3rdparty/segvcatch/src/segvcatch.cpp \
        ../3rdparty/simplecrypt/simplecrypt.cpp

# 3rdparty headers common for all Qt versions, platforms and compilers
HEADERS  += \
        include/cgscript.h \
        cgscript/include/top.h

#
# standard codeeditor
equals(QT_MAJOR_VERSION, 5) {
lessThan(QT_MINOR_VERSION, 7) {
SOURCES += \
        src/codeeditor.cpp

HEADERS  += \
        include/codeeditor.h
}
}

equals(QT_MAJOR_VERSION, 4) {
SOURCES += \
        src/codeeditor.cpp

HEADERS  += \
        include/codeeditor.h
}

#
# QCodeEditor requires Qt >= 5.7
equals(QT_MAJOR_VERSION, 5) {
greaterThan(QT_MINOR_VERSION, 6) {
INCLUDEPATH += \
        ../3rdparty/qcodeeditor/include \
        ../3rdparty/qcodeeditor/include/internal

SOURCES += \
        ../3rdparty/qcodeeditor/src/QCodeEditor.cpp \
        ../3rdparty/qcodeeditor/src/QCXXHighlighter.cpp \
        ../3rdparty/qcodeeditor/src/QFramedTextAttribute.cpp \
        ../3rdparty/qcodeeditor/src/QGLSLCompleter.cpp \
        ../3rdparty/qcodeeditor/src/QGLSLHighlighter.cpp \
        ../3rdparty/qcodeeditor/src/QJSONHighlighter.cpp \
        ../3rdparty/qcodeeditor/src/QLanguage.cpp \
        ../3rdparty/qcodeeditor/src/QLineNumberArea.cpp \
        ../3rdparty/qcodeeditor/src/QLuaCompleter.cpp \
        ../3rdparty/qcodeeditor/src/QLuaHighlighter.cpp \
        ../3rdparty/qcodeeditor/src/QPythonCompleter.cpp \
        ../3rdparty/qcodeeditor/src/QPythonHighlighter.cpp \
        ../3rdparty/qcodeeditor/src/QStyleSyntaxHighlighter.cpp \
        ../3rdparty/qcodeeditor/src/QSyntaxStyle.cpp \
        ../3rdparty/qcodeeditor/src/QXMLHighlighter.cpp

HEADERS += \
        ../3rdparty/qcodeeditor/include/QCodeEditor \
        ../3rdparty/qcodeeditor/include/QCXXHighlighter \
        ../3rdparty/qcodeeditor/include/QFramedTextAttribute \
        ../3rdparty/qcodeeditor/include/QGLSLCompleter \
        ../3rdparty/qcodeeditor/include/QGLSLHighlighter \
        ../3rdparty/qcodeeditor/include/QHighlightBlockRule  \
        ../3rdparty/qcodeeditor/include/QHighlightRule \
        ../3rdparty/qcodeeditor/include/QJSONHighlighter \
        ../3rdparty/qcodeeditor/include/QLanguage \
        ../3rdparty/qcodeeditor/include/QLineNumberArea \
        ../3rdparty/qcodeeditor/include/QLuaCompleter \
        ../3rdparty/qcodeeditor/include/QLuaHighlighter \
        ../3rdparty/qcodeeditor/include/QPythonCompleter \
        ../3rdparty/qcodeeditor/include/QPythonHighlighter \
        ../3rdparty/qcodeeditor/include/QStyleSyntaxHighlighter \
        ../3rdparty/qcodeeditor/include/QSyntaxStyle \
        ../3rdparty/qcodeeditor/include/QXMLHighlighter \
        ../3rdparty/qcodeeditor/include/internal/QCodeEditor.hpp \
        ../3rdparty/qcodeeditor/include/internal/QCXXHighlighter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QFramedTextAttribute.hpp \
        ../3rdparty/qcodeeditor/include/internal/QGLSLCompleter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QGLSLHighlighter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QHighlightBlockRule.hpp \
        ../3rdparty/qcodeeditor/include/internal/QHighlightRule.hpp \
        ../3rdparty/qcodeeditor/include/internal/QJSONHighlighter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QLanguage.hpp \
        ../3rdparty/qcodeeditor/include/internal/QLineNumberArea.hpp \
        ../3rdparty/qcodeeditor/include/internal/QLuaCompleter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QLuaHighlighter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QPythonCompleter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QPythonHighlighter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QStyleSyntaxHighlighter.hpp \
        ../3rdparty/qcodeeditor/include/internal/QSyntaxStyle.hpp \
        ../3rdparty/qcodeeditor/include/internal/QXMLHighlighter.hpp
}
}

#
# QWebEngineView: available after Qt 5.6
equals(QT_MAJOR_VERSION, 5) {
greaterThan(QT_MINOR_VERSION, 5) {
HEADERS += \
        include/qtcwebengine.h
}
}

#
# x86disasm
linux*|macx* {
INCLUDEPATH += ../3rdparty/x86dis/include
LIBS += -lx86dis
}

#
# libtcc
linux*|win32* {
INCLUDEPATH += ../3rdparty/libtcc
LIBS += -ltcc
}

#
# libxls
macx*|win32*|contains(OSDISTRO, .*Debian.*)|contains(OSDISTRO, .*centos.*)|contains(OSDISTRO, .*Ubuntu.*) {
INCLUDEPATH += \
        ../3rdparty/libxls/include

SOURCES += \
        ../3rdparty/libxls/src/endian.c \
        ../3rdparty/libxls/src/ole.c \
        ../3rdparty/libxls/src/xls.c \
        ../3rdparty/libxls/src/xlstool.c

LIBS    -= -lxlsreader
}

#
# forms
FORMS += \
        forms/addportfoliodialog.ui \
        forms/addtransactiondialog.ui \
        forms/addpricedialog.ui \
        forms/databrowserdialog.ui \
        forms/datamanagerdialog.ui \
        forms/debugdialog.ui \
        forms/downloaddatadialog.ui \
        forms/dynparamsdialog.ui \
        forms/editorwidget.ui \
        forms/infodialog.ui \
        forms/licensedialog.ui \
        forms/lineobjectdialog.ui \
        forms/loadcsvdialog.ui \
        forms/mainwindow.ui \
        forms/manualdialog.ui \
        forms/modulemanagerdialog.ui \
        forms/optionsdialog.ui \
        forms/progressdialog.ui \
        forms/portfoliomanagerdialog.ui \
        forms/portfolio.ui \
        forms/qtacdata.ui \
        forms/qtacdraw.ui \
        forms/qtacfunctions.ui \
        forms/qtachelp.ui \
        forms/qtacobjects.ui \
        forms/qtacproperties.ui \
        forms/splashdialog.ui \
        forms/symbollistdialog.ui \
        forms/templatemanagerdialog.ui \
        forms/textobjectdialog.ui \
        forms/waitdialog.ui
