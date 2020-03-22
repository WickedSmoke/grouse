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
        3rdparty/sqlite3/include \
        3rdparty/segvcatch/include \
        3rdparty/ta-lib/include \
        3rdparty/simplecrypt/include

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
        src/chartapp.cpp \
        src/common.cpp \
        src/compile.cpp \
        src/csv.cpp \
        src/create_portfolio_views.cpp \
        src/databrowserdialog.cpp \
        src/datamanagerdialog.cpp \
        dbman/dbman.cpp \
        src/debugdialog.cpp \
        src/downloaddatadialog.cpp \
        src/dynparamsdialog.cpp \
        src/editorwidget.cpp \
        src/feedav.cpp \
        src/feediex.cpp \
        src/feedyahoo.cpp \
        src/function_dataset.cpp \
        src/highlighter.cpp \
        src/infodialog.cpp \
        src/licensedialog.cpp \
        src/lineobjectdialog.cpp \
        src/loadcsvdialog.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/manualdialog.cpp \
        src/modulemanagerdialog.cpp \
        src/netservice.cpp \
        src/native_segvfpe.cpp \
        src/optionsdialog.cpp \
        src/platformstring.cpp \
        src/priceupdater.cpp \
        src/priceworker.cpp \
        src/progressdialog.cpp \
        src/portfoliomanagerdialog.cpp \
        src/portfolio.cpp \
        src/qtachart_core_barchart.cpp \
        src/qtachart_core_candlechart.cpp \
        src/qtachart_core.cpp \
        src/qtachart_core_drawbar.cpp \
        src/qtachart_core_drawcandle.cpp \
        src/qtachart_core_draw.cpp \
        src/qtachart_core_drawpriceline.cpp \
        src/qtachart.cpp \
        src/qtachart_data.cpp \
        src/qtachart_draw.cpp \
        src/qtachart_eventfilters.cpp \
        src/qtachart_functions.cpp \
        src/qtachart_help.cpp \
        src/qtachart_methods.cpp \
        src/qtachart_object.cpp \
        src/qtachart_object_sanitizer.cpp \
        src/qtachart_object_draw.cpp \
        src/qtachart_object_modules.cpp \
        src/qtachart_objects.cpp \
        src/qtachart_properties.cpp \
        src/qtachart_utilities.cpp \
        src/qtcgraphicsitem.cpp \
        src/splashdialog.cpp \
        src/sqlcb_dbversion.cpp \
        src/sqlcb_datafeeds.cpp \
        src/sqlcb_modules.cpp \
        src/sqlcb_symbol_table.cpp \
        src/sqlcb_toolchain.cpp \
        src/stockticker.cpp \
        src/selectfromdb.cpp \
        src/symbollistdialog.cpp \
        src/templatemanagerdialog.cpp \
        src/textobjectdialog.cpp \
        src/unix_signals.cpp \
        src/waitdialog.cpp

# application headers, common for all Qt versions, platforms and compilers
HEADERS  += \
        include/appColorDialog.h \
        include/addportfoliodialog.h \
        include/addpricedialog.h \
        include/addtransactiondialog.h \
        include/chartapp.h \
        include/compile.h \
        include/databrowserdialog.h \
        include/datamanagerdialog.h \
        include/debug.h \
        include/debugdialog.h \
        include/defs.h \
        include/downloaddatadialog.h \
        include/dynparamsdialog.h \
        include/editorwidget.h \
        include/feedav.h \
        include/feediex.h \
        include/feedyahoo.h \
        include/highlighter.h \
        include/infodialog.h \
        include/licensedialog.h \
        include/lineobjectdialog.h \
        include/loadcsvdialog.h \
        include/mainwindow.h \
        include/manualdialog.h \
        include/modulemanagerdialog.h \
        include/netservice.h \
        include/optionsdialog.h \
        include/priceupdater.h \
        include/priceworker.h \
        include/progressdialog.h \
        include/portfoliomanagerdialog.h \
        include/portfolio.h \
        include/qtachart_core.h \
        include/qtachart_data.h \
        include/qtachart_draw.h \
        include/qtachart_eventfilters.h \
        include/qtachart_functions.h \
        include/qtachart.h \
        include/qtachart_help.h \
        include/qtachart_object.h \
        include/qtachart_objects.h \
        include/qtachart_properties.h \
        include/qtcgraphicsitem.h \
        include/qtcgraphicsscene.h \
        include/splashdialog.h \
        include/stockticker.h \
        include/symbollistdialog.h \
        include/templatemanagerdialog.h \
        include/textobjectdialog.h \
        include/unix_signals.h \
        include/waitdialog.h


# Qt5 specific sources and headers
equals(QT_MAJOR_VERSION, 5) {
SOURCES += \
        src/natevents.cpp

HEADERS  += \
        include/natevents.h
}

# 3rdparty sources common for all Qt versions, platforms and compilers
SOURCES += \
        3rdparty/sqlite3/sqlite3_wrapper.c \
        3rdparty/segvcatch/src/segvcatch.cpp \
        3rdparty/simplecrypt/simplecrypt.cpp \
        3rdparty/ta-lib/ta_abstract/frames/ta_frame.c \
        3rdparty/ta-lib/ta_abstract/ta_abstract.c \
        3rdparty/ta-lib/ta_abstract/tables/table_a.c \
        3rdparty/ta-lib/ta_abstract/tables/table_b.c \
        3rdparty/ta-lib/ta_abstract/tables/table_c.c \
        3rdparty/ta-lib/ta_abstract/tables/table_d.c \
        3rdparty/ta-lib/ta_abstract/tables/table_e.c \
        3rdparty/ta-lib/ta_abstract/tables/table_f.c \
        3rdparty/ta-lib/ta_abstract/tables/table_g.c \
        3rdparty/ta-lib/ta_abstract/tables/table_h.c \
        3rdparty/ta-lib/ta_abstract/tables/table_i.c \
        3rdparty/ta-lib/ta_abstract/tables/table_j.c \
        3rdparty/ta-lib/ta_abstract/tables/table_k.c \
        3rdparty/ta-lib/ta_abstract/tables/table_l.c \
        3rdparty/ta-lib/ta_abstract/tables/table_m.c \
        3rdparty/ta-lib/ta_abstract/tables/table_n.c \
        3rdparty/ta-lib/ta_abstract/tables/table_o.c \
        3rdparty/ta-lib/ta_abstract/tables/table_p.c \
        3rdparty/ta-lib/ta_abstract/tables/table_q.c \
        3rdparty/ta-lib/ta_abstract/tables/table_r.c \
        3rdparty/ta-lib/ta_abstract/tables/table_s.c \
        3rdparty/ta-lib/ta_abstract/tables/table_t.c \
        3rdparty/ta-lib/ta_abstract/tables/table_u.c \
        3rdparty/ta-lib/ta_abstract/tables/table_v.c \
        3rdparty/ta-lib/ta_abstract/tables/table_w.c \
        3rdparty/ta-lib/ta_abstract/tables/table_x.c \
        3rdparty/ta-lib/ta_abstract/tables/table_y.c \
        3rdparty/ta-lib/ta_abstract/tables/table_z.c \
        3rdparty/ta-lib/ta_abstract/ta_def_ui.c \
        3rdparty/ta-lib/ta_abstract/ta_func_api.c \
        3rdparty/ta-lib/ta_abstract/ta_group_idx.c \
        3rdparty/ta-lib/ta_common/ta_global.c \
        3rdparty/ta-lib/ta_common/ta_retcode.c \
        3rdparty/ta-lib/ta_common/ta_version.c \
        3rdparty/ta-lib/ta_func/ta_ACOS.c \
        3rdparty/ta-lib/ta_func/ta_AD.c \
        3rdparty/ta-lib/ta_func/ta_ADD.c \
        3rdparty/ta-lib/ta_func/ta_ADOSC.c \
        3rdparty/ta-lib/ta_func/ta_ADX.c \
        3rdparty/ta-lib/ta_func/ta_ADXR.c \
        3rdparty/ta-lib/ta_func/ta_APO.c \
        3rdparty/ta-lib/ta_func/ta_AROON.c \
        3rdparty/ta-lib/ta_func/ta_AROONOSC.c \
        3rdparty/ta-lib/ta_func/ta_ASIN.c \
        3rdparty/ta-lib/ta_func/ta_ATAN.c \
        3rdparty/ta-lib/ta_func/ta_ATR.c \
        3rdparty/ta-lib/ta_func/ta_AVGPRICE.c \
        3rdparty/ta-lib/ta_func/ta_BBANDS.c \
        3rdparty/ta-lib/ta_func/ta_BETA.c \
        3rdparty/ta-lib/ta_func/ta_BOP.c \
        3rdparty/ta-lib/ta_func/ta_CCI.c \
        3rdparty/ta-lib/ta_func/ta_CDL2CROWS.c \
        3rdparty/ta-lib/ta_func/ta_CDL3BLACKCROWS.c \
        3rdparty/ta-lib/ta_func/ta_CDL3INSIDE.c \
        3rdparty/ta-lib/ta_func/ta_CDL3LINESTRIKE.c \
        3rdparty/ta-lib/ta_func/ta_CDL3OUTSIDE.c \
        3rdparty/ta-lib/ta_func/ta_CDL3STARSINSOUTH.c \
        3rdparty/ta-lib/ta_func/ta_CDL3WHITESOLDIERS.c \
        3rdparty/ta-lib/ta_func/ta_CDLABANDONEDBABY.c \
        3rdparty/ta-lib/ta_func/ta_CDLADVANCEBLOCK.c \
        3rdparty/ta-lib/ta_func/ta_CDLBELTHOLD.c \
        3rdparty/ta-lib/ta_func/ta_CDLBREAKAWAY.c \
        3rdparty/ta-lib/ta_func/ta_CDLCLOSINGMARUBOZU.c \
        3rdparty/ta-lib/ta_func/ta_CDLCONCEALBABYSWALL.c \
        3rdparty/ta-lib/ta_func/ta_CDLCOUNTERATTACK.c \
        3rdparty/ta-lib/ta_func/ta_CDLDARKCLOUDCOVER.c \
        3rdparty/ta-lib/ta_func/ta_CDLDOJI.c \
        3rdparty/ta-lib/ta_func/ta_CDLDOJISTAR.c \
        3rdparty/ta-lib/ta_func/ta_CDLDRAGONFLYDOJI.c \
        3rdparty/ta-lib/ta_func/ta_CDLENGULFING.c \
        3rdparty/ta-lib/ta_func/ta_CDLEVENINGDOJISTAR.c \
        3rdparty/ta-lib/ta_func/ta_CDLEVENINGSTAR.c \
        3rdparty/ta-lib/ta_func/ta_CDLGAPSIDESIDEWHITE.c \
        3rdparty/ta-lib/ta_func/ta_CDLGRAVESTONEDOJI.c \
        3rdparty/ta-lib/ta_func/ta_CDLHAMMER.c \
        3rdparty/ta-lib/ta_func/ta_CDLHANGINGMAN.c \
        3rdparty/ta-lib/ta_func/ta_CDLHARAMI.c \
        3rdparty/ta-lib/ta_func/ta_CDLHARAMICROSS.c \
        3rdparty/ta-lib/ta_func/ta_CDLHIGHWAVE.c \
        3rdparty/ta-lib/ta_func/ta_CDLHIKKAKE.c \
        3rdparty/ta-lib/ta_func/ta_CDLHIKKAKEMOD.c \
        3rdparty/ta-lib/ta_func/ta_CDLHOMINGPIGEON.c \
        3rdparty/ta-lib/ta_func/ta_CDLIDENTICAL3CROWS.c \
        3rdparty/ta-lib/ta_func/ta_CDLINNECK.c \
        3rdparty/ta-lib/ta_func/ta_CDLINVERTEDHAMMER.c \
        3rdparty/ta-lib/ta_func/ta_CDLKICKINGBYLENGTH.c \
        3rdparty/ta-lib/ta_func/ta_CDLKICKING.c \
        3rdparty/ta-lib/ta_func/ta_CDLLADDERBOTTOM.c \
        3rdparty/ta-lib/ta_func/ta_CDLLONGLEGGEDDOJI.c \
        3rdparty/ta-lib/ta_func/ta_CDLLONGLINE.c \
        3rdparty/ta-lib/ta_func/ta_CDLMARUBOZU.c \
        3rdparty/ta-lib/ta_func/ta_CDLMATCHINGLOW.c \
        3rdparty/ta-lib/ta_func/ta_CDLMATHOLD.c \
        3rdparty/ta-lib/ta_func/ta_CDLMORNINGDOJISTAR.c \
        3rdparty/ta-lib/ta_func/ta_CDLMORNINGSTAR.c \
        3rdparty/ta-lib/ta_func/ta_CDLONNECK.c \
        3rdparty/ta-lib/ta_func/ta_CDLPIERCING.c \
        3rdparty/ta-lib/ta_func/ta_CDLRICKSHAWMAN.c \
        3rdparty/ta-lib/ta_func/ta_CDLRISEFALL3METHODS.c \
        3rdparty/ta-lib/ta_func/ta_CDLSEPARATINGLINES.c \
        3rdparty/ta-lib/ta_func/ta_CDLSHOOTINGSTAR.c \
        3rdparty/ta-lib/ta_func/ta_CDLSHORTLINE.c \
        3rdparty/ta-lib/ta_func/ta_CDLSPINNINGTOP.c \
        3rdparty/ta-lib/ta_func/ta_CDLSTALLEDPATTERN.c \
        3rdparty/ta-lib/ta_func/ta_CDLSTICKSANDWICH.c \
        3rdparty/ta-lib/ta_func/ta_CDLTAKURI.c \
        3rdparty/ta-lib/ta_func/ta_CDLTASUKIGAP.c \
        3rdparty/ta-lib/ta_func/ta_CDLTHRUSTING.c \
        3rdparty/ta-lib/ta_func/ta_CDLTRISTAR.c \
        3rdparty/ta-lib/ta_func/ta_CDLUNIQUE3RIVER.c \
        3rdparty/ta-lib/ta_func/ta_CDLUPSIDEGAP2CROWS.c \
        3rdparty/ta-lib/ta_func/ta_CDLXSIDEGAP3METHODS.c \
        3rdparty/ta-lib/ta_func/ta_CEIL.c \
        3rdparty/ta-lib/ta_func/ta_CMO.c \
        3rdparty/ta-lib/ta_func/ta_CORREL.c \
        3rdparty/ta-lib/ta_func/ta_COS.c \
        3rdparty/ta-lib/ta_func/ta_COSH.c \
        3rdparty/ta-lib/ta_func/ta_DEMA.c \
        3rdparty/ta-lib/ta_func/ta_DIV.c \
        3rdparty/ta-lib/ta_func/ta_DX.c \
        3rdparty/ta-lib/ta_func/ta_EMA.c \
        3rdparty/ta-lib/ta_func/ta_EXP.c \
        3rdparty/ta-lib/ta_func/ta_FLOOR.c \
        3rdparty/ta-lib/ta_func/ta_HT_DCPERIOD.c \
        3rdparty/ta-lib/ta_func/ta_HT_DCPHASE.c \
        3rdparty/ta-lib/ta_func/ta_HT_PHASOR.c \
        3rdparty/ta-lib/ta_func/ta_HT_SINE.c \
        3rdparty/ta-lib/ta_func/ta_HT_TRENDLINE.c \
        3rdparty/ta-lib/ta_func/ta_HT_TRENDMODE.c \
        3rdparty/ta-lib/ta_func/ta_KAMA.c \
        3rdparty/ta-lib/ta_func/ta_LINEARREG_ANGLE.c \
        3rdparty/ta-lib/ta_func/ta_LINEARREG.c \
        3rdparty/ta-lib/ta_func/ta_LINEARREG_INTERCEPT.c \
        3rdparty/ta-lib/ta_func/ta_LINEARREG_SLOPE.c \
        3rdparty/ta-lib/ta_func/ta_LN.c \
        3rdparty/ta-lib/ta_func/ta_LOG10.c \
        3rdparty/ta-lib/ta_func/ta_MA.c \
        3rdparty/ta-lib/ta_func/ta_MACD.c \
        3rdparty/ta-lib/ta_func/ta_MACDEXT.c \
        3rdparty/ta-lib/ta_func/ta_MACDFIX.c \
        3rdparty/ta-lib/ta_func/ta_MAMA.c \
        3rdparty/ta-lib/ta_func/ta_MAVP.c \
        3rdparty/ta-lib/ta_func/ta_MAX.c \
        3rdparty/ta-lib/ta_func/ta_MAXINDEX.c \
        3rdparty/ta-lib/ta_func/ta_MEDPRICE.c \
        3rdparty/ta-lib/ta_func/ta_MFI.c \
        3rdparty/ta-lib/ta_func/ta_MIDPOINT.c \
        3rdparty/ta-lib/ta_func/ta_MIDPRICE.c \
        3rdparty/ta-lib/ta_func/ta_MIN.c \
        3rdparty/ta-lib/ta_func/ta_MININDEX.c \
        3rdparty/ta-lib/ta_func/ta_MINMAX.c \
        3rdparty/ta-lib/ta_func/ta_MINMAXINDEX.c \
        3rdparty/ta-lib/ta_func/ta_MINUS_DI.c \
        3rdparty/ta-lib/ta_func/ta_MINUS_DM.c \
        3rdparty/ta-lib/ta_func/ta_MOM.c \
        3rdparty/ta-lib/ta_func/ta_MULT.c \
        3rdparty/ta-lib/ta_func/ta_NATR.c \
        3rdparty/ta-lib/ta_func/ta_NVI.c \
        3rdparty/ta-lib/ta_func/ta_OBV.c \
        3rdparty/ta-lib/ta_func/ta_PLUS_DI.c \
        3rdparty/ta-lib/ta_func/ta_PLUS_DM.c \
        3rdparty/ta-lib/ta_func/ta_PPO.c \
        3rdparty/ta-lib/ta_func/ta_PVI.c \
        3rdparty/ta-lib/ta_func/ta_ROC.c \
        3rdparty/ta-lib/ta_func/ta_ROCP.c \
        3rdparty/ta-lib/ta_func/ta_ROCR100.c \
        3rdparty/ta-lib/ta_func/ta_ROCR.c \
        3rdparty/ta-lib/ta_func/ta_RSI.c \
        3rdparty/ta-lib/ta_func/ta_SAR.c \
        3rdparty/ta-lib/ta_func/ta_SAREXT.c \
        3rdparty/ta-lib/ta_func/ta_SIN.c \
        3rdparty/ta-lib/ta_func/ta_SINH.c \
        3rdparty/ta-lib/ta_func/ta_SMA.c \
        3rdparty/ta-lib/ta_func/ta_SQRT.c \
        3rdparty/ta-lib/ta_func/ta_STDDEV.c \
        3rdparty/ta-lib/ta_func/ta_STOCH.c \
        3rdparty/ta-lib/ta_func/ta_STOCHF.c \
        3rdparty/ta-lib/ta_func/ta_STOCHRSI.c \
        3rdparty/ta-lib/ta_func/ta_SUB.c \
        3rdparty/ta-lib/ta_func/ta_SUM.c \
        3rdparty/ta-lib/ta_func/ta_T3.c \
        3rdparty/ta-lib/ta_func/ta_TAN.c \
        3rdparty/ta-lib/ta_func/ta_TANH.c \
        3rdparty/ta-lib/ta_func/ta_TEMA.c \
        3rdparty/ta-lib/ta_func/ta_TRANGE.c \
        3rdparty/ta-lib/ta_func/ta_TRIMA.c \
        3rdparty/ta-lib/ta_func/ta_TRIX.c \
        3rdparty/ta-lib/ta_func/ta_TSF.c \
        3rdparty/ta-lib/ta_func/ta_TYPPRICE.c \
        3rdparty/ta-lib/ta_func/ta_ULTOSC.c \
        3rdparty/ta-lib/ta_func/ta_utility.c \
        3rdparty/ta-lib/ta_func/ta_VAR.c \
        3rdparty/ta-lib/ta_func/ta_WCLPRICE.c \
        3rdparty/ta-lib/ta_func/ta_WILLR.c \
        3rdparty/ta-lib/ta_func/ta_WMA.c

# 3rdparty headers common for all Qt versions, platforms and compilers
HEADERS  += \
        include/appColorDialog.h \
        include/addportfoliodialog.h \
        include/addpricedialog.h \
        include/addtransactiondialog.h \
        include/appdata.h \
        include/chartapp.h \
        include/cgscript.h \
        cgscript/include/top.h \
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
        include/feedav.h \
        include/feediex.h \
        include/feedyahoo.h \
        include/function_dataset.h \
        include/highlighter.h \
        include/infodialog.h \
        include/licensedialog.h \
        include/lineobjectdialog.h \
        include/loadcsvdialog.h \
        include/mainwindow.h \
        include/manualdialog.h \
        include/modulemanagerdialog.h \
        include/netservice.h \
        include/optionsdialog.h \
        include/priceupdater.h \
        include/priceworker.h \
        include/progressdialog.h \
        include/portfoliomanagerdialog.h \
        include/portfolio.h \
        include/qtachart_core.h \
        include/qtachart_data.h \
        include/qtachart_draw.h \
        include/qtachart_eventfilters.h \
        include/qtachart_functions.h \
        include/qtachart.h \
        include/qtachart_help.h \
        include/qtachart_object.h \
        include/qtachart_objects.h \
        include/qtachart_properties.h \
        include/qtcgraphicsitem.h \
        include/qtcgraphicsscene.h \
        include/splashdialog.h \
        include/stockticker.h \
        include/symbollistdialog.h \
        include/templatemanagerdialog.h \
        include/textobjectdialog.h \
        include/unix_signals.h \
        include/waitdialog.h

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
        3rdparty/qcodeeditor/include \
        3rdparty/qcodeeditor/include/internal

SOURCES += \
        3rdparty/qcodeeditor/src/QCodeEditor.cpp \
        3rdparty/qcodeeditor/src/QCXXHighlighter.cpp \
        3rdparty/qcodeeditor/src/QFramedTextAttribute.cpp \
        3rdparty/qcodeeditor/src/QGLSLCompleter.cpp \
        3rdparty/qcodeeditor/src/QGLSLHighlighter.cpp \
        3rdparty/qcodeeditor/src/QJSONHighlighter.cpp \
        3rdparty/qcodeeditor/src/QLanguage.cpp \
        3rdparty/qcodeeditor/src/QLineNumberArea.cpp \
        3rdparty/qcodeeditor/src/QLuaCompleter.cpp \
        3rdparty/qcodeeditor/src/QLuaHighlighter.cpp \
        3rdparty/qcodeeditor/src/QPythonCompleter.cpp \
        3rdparty/qcodeeditor/src/QPythonHighlighter.cpp \
        3rdparty/qcodeeditor/src/QStyleSyntaxHighlighter.cpp \
        3rdparty/qcodeeditor/src/QSyntaxStyle.cpp \
        3rdparty/qcodeeditor/src/QXMLHighlighter.cpp

HEADERS += \
        3rdparty/qcodeeditor/include/QCodeEditor \
        3rdparty/qcodeeditor/include/QCXXHighlighter \
        3rdparty/qcodeeditor/include/QFramedTextAttribute \
        3rdparty/qcodeeditor/include/QGLSLCompleter \
        3rdparty/qcodeeditor/include/QGLSLHighlighter \
        3rdparty/qcodeeditor/include/QHighlightBlockRule  \
        3rdparty/qcodeeditor/include/QHighlightRule \
        3rdparty/qcodeeditor/include/QJSONHighlighter \
        3rdparty/qcodeeditor/include/QLanguage \
        3rdparty/qcodeeditor/include/QLineNumberArea \
        3rdparty/qcodeeditor/include/QLuaCompleter \
        3rdparty/qcodeeditor/include/QLuaHighlighter \
        3rdparty/qcodeeditor/include/QPythonCompleter \
        3rdparty/qcodeeditor/include/QPythonHighlighter \
        3rdparty/qcodeeditor/include/QStyleSyntaxHighlighter \
        3rdparty/qcodeeditor/include/QSyntaxStyle \
        3rdparty/qcodeeditor/include/QXMLHighlighter \
        3rdparty/qcodeeditor/include/internal/QCodeEditor.hpp \
        3rdparty/qcodeeditor/include/internal/QCXXHighlighter.hpp \
        3rdparty/qcodeeditor/include/internal/QFramedTextAttribute.hpp \
        3rdparty/qcodeeditor/include/internal/QGLSLCompleter.hpp \
        3rdparty/qcodeeditor/include/internal/QGLSLHighlighter.hpp \
        3rdparty/qcodeeditor/include/internal/QHighlightBlockRule.hpp \
        3rdparty/qcodeeditor/include/internal/QHighlightRule.hpp \
        3rdparty/qcodeeditor/include/internal/QJSONHighlighter.hpp \
        3rdparty/qcodeeditor/include/internal/QLanguage.hpp \
        3rdparty/qcodeeditor/include/internal/QLineNumberArea.hpp \
        3rdparty/qcodeeditor/include/internal/QLuaCompleter.hpp \
        3rdparty/qcodeeditor/include/internal/QLuaHighlighter.hpp \
        3rdparty/qcodeeditor/include/internal/QPythonCompleter.hpp \
        3rdparty/qcodeeditor/include/internal/QPythonHighlighter.hpp \
        3rdparty/qcodeeditor/include/internal/QStyleSyntaxHighlighter.hpp \
        3rdparty/qcodeeditor/include/internal/QSyntaxStyle.hpp \
        3rdparty/qcodeeditor/include/internal/QXMLHighlighter.hpp
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
INCLUDEPATH += \
        3rdparty/x86dis/include

SOURCES     += \
        3rdparty/x86dis/src/ia32_implicit.c \
        3rdparty/x86dis/src/ia32_insn.c \
        3rdparty/x86dis/src/ia32_invariant.c \
        3rdparty/x86dis/src/ia32_modrm.c \
        3rdparty/x86dis/src/ia32_opcode_tables.c \
        3rdparty/x86dis/src/ia32_operand.c \
        3rdparty/x86dis/src/ia32_reg.c \
        3rdparty/x86dis/src/ia32_settings.c \
        3rdparty/x86dis/src/x86_disasm.c \
        3rdparty/x86dis/src/x86_format.c \
        3rdparty/x86dis/src/x86_imm.c \
        3rdparty/x86dis/src/x86_insn.c \
        3rdparty/x86dis/src/x86_misc.c \
        3rdparty/x86dis/src/x86_operand_list.c
}

#
# libtcc
linux*|win32* {
INCLUDEPATH += \
        ../3rdparty/libtcc

SOURCES += \
        ../3rdparty/libtcc/tccasm.c \
        ../3rdparty/libtcc/tccelf.c \
        ../3rdparty/libtcc/tccgen.c \
        ../3rdparty/libtcc/tccpp.c \
        ../3rdparty/libtcc/tccrun.c \
        ../3rdparty/libtcc/libtcc.c
}

#
# libxls
macx*|win32*|contains(OSDISTRO, .*Debian.*)|contains(OSDISTRO, .*centos.*)|contains(OSDISTRO, .*Ubuntu.*) {
INCLUDEPATH += \
        3rdparty/libxls/include

SOURCES += \
        3rdparty/libxls/src/endian.c \
        3rdparty/libxls/src/ole.c \
        3rdparty/libxls/src/xls.c \
        3rdparty/libxls/src/xlstool.c

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
        forms/qtachart.ui \
        forms/qtachelp.ui \
        forms/qtacobjects.ui \
        forms/qtacproperties.ui \
        forms/splashdialog.ui \
        forms/stockticker.ui \
        forms/symbollistdialog.ui \
        forms/templatemanagerdialog.ui \
        forms/textobjectdialog.ui \
        forms/waitdialog.ui
