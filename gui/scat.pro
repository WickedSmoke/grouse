OBJECTS_DIR = obj
MOC_DIR = moc

RESOURCES += ../ChartGeany/resources/ChartGeany.qrc

CONFIG += qt debug

QT += widgets network webenginewidgets

DEFINES *= QT_USE_QSTRINGBUILDER	# For feed*.cpp

INCLUDEPATH += ../ChartGeany/include \
	../ChartGeany/ui \
	../ChartGeany/cgscript/include \
	../chart \
	../database \
	../3rdparty/simplecrypt \
	../3rdparty/ta-lib/include \
	../3rdparty/x86dis/include \
	../3rdparty/sqlite3/include

LIBS += -ldl -lxlsreader

HEADERS += MainWindow.h \
	DataManager.h \
	../ChartGeany/include/appColorDialog.h \
	../ChartGeany/include/chartapp.h \
	../ChartGeany/include/debugdialog.h \
	../ChartGeany/include/databrowserdialog.h \
	../ChartGeany/include/downloaddatadialog.h \
	../ChartGeany/include/dynparamsdialog.h \
	../ChartGeany/include/symbollistdialog.h \
	../ChartGeany/include/textobjectdialog.h \
	../ChartGeany/include/templatemanagerdialog.h \
	../ChartGeany/include/lineobjectdialog.h \
	../ChartGeany/include/priceupdater.h \
	../ChartGeany/include/priceworker.h \
	../ChartGeany/include/progressdialog.h \
	../ChartGeany/include/netservice.h \
	../ChartGeany/include/stockticker.h \
	../ChartGeany/include/waitdialog.h \
	../chart/qtachart.h \
	../chart/qtachart_core.h \
	../chart/qtachart_eventfilters.h \
	../chart/qtachart_data.h \
	../chart/qtachart_draw.h \
	../chart/qtachart_functions.h \
	../chart/qtachart_help.h \
	../chart/qtachart_object.h \
	../chart/qtachart_objects.h \
	../chart/qtachart_properties.h \
	../chart/qtcgraphicsitem.h \
	../chart/qtcgraphicsscene.h \
	../database/feedav.h \
	../database/feediex.h \
	../database/feedyahoo.h


SOURCES += MainWindow.cpp \
	DataManager.cpp \
	../ChartGeany/src/appColorDialog.cpp \
	../ChartGeany/src/chartapp.cpp \
	../ChartGeany/src/common.cpp \
	../ChartGeany/src/debugdialog.cpp \
	../ChartGeany/src/databrowserdialog.cpp \
	../ChartGeany/src/downloaddatadialog.cpp \
	../ChartGeany/src/dynparamsdialog.cpp \
	../ChartGeany/src/symbollistdialog.cpp \
	../ChartGeany/src/textobjectdialog.cpp \
	../ChartGeany/src/templatemanagerdialog.cpp \
	../ChartGeany/src/function_dataset.cpp \
	../ChartGeany/src/lineobjectdialog.cpp \
	../ChartGeany/src/priceupdater.cpp \
	../ChartGeany/src/priceworker.cpp \
	../ChartGeany/src/progressdialog.cpp \
	../ChartGeany/src/native_segvfpe.cpp \
	../ChartGeany/src/netservice.cpp \
	../ChartGeany/src/stockticker.cpp \
	../ChartGeany/src/waitdialog.cpp \
	../ChartGeany/src/cgscript.c \
	../ChartGeany/src/cgscript_imp_arrays.cpp \
	../ChartGeany/src/cgscript_imp_bars.cpp \
	../ChartGeany/src/cgscript_imp_chart.cpp \
	../ChartGeany/src/cgscript_imp_common.cpp \
	../ChartGeany/src/cgscript_imp_errors.cpp \
	../ChartGeany/src/cgscript_imp_hooks.cpp \
	../ChartGeany/src/cgscript_imp_object.cpp \
	../ChartGeany/src/cgscript_imp_strings.cpp \
	../ChartGeany/src/cgscript_imp_tafunc.cpp \
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
	../chart/qtachart_methods.cpp \
	../chart/qtachart_object.cpp \
	../chart/qtachart_object_draw.cpp \
	../chart/qtachart_object_modules.cpp \
	../chart/qtachart_object_sanitizer.cpp \
	../chart/qtachart_objects.cpp \
	../chart/qtachart_properties.cpp \
	../chart/qtcgraphicsitem.cpp \
	../database/idb.cpp \
	../database/csv.cpp \
	../database/dbman.cpp \
	../database/platformstring.cpp \
	../database/feedav.cpp \
	../database/feediex.cpp \
	../database/feedyahoo.cpp \
	../database/sqlcb_symbol_table.cpp \
	../database/sqlcb_toolchain.cpp \

# Third party modules.

LIBS += -L../3rdparty -lta-lib -lx86dis

SOURCES += ../3rdparty/sqlite3/sqlite3.c \
	../3rdparty/simplecrypt/simplecrypt.cpp
