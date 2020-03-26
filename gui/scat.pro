OBJECTS_DIR = obj
MOC_DIR = moc

RESOURCES += ../ChartGeany/resources/ChartGeany.qrc

CONFIG += qt debug

QT += widgets network webenginewidgets

DEFINES *= QT_USE_QSTRINGBUILDER	# For feed*.cpp

INCLUDEPATH += ../ChartGeany/include \
	../ChartGeany/ui \
	../ChartGeany/cgscript/include \
	../database \
	../3rdparty/simplecrypt \
	../3rdparty/ta-lib/include \
	../3rdparty/x86dis/include \
	../3rdparty/sqlite3/include

LIBS += -ldl -lxlsreader

HEADERS += MainWindow.h \
	../ChartGeany/include/appColorDialog.h \
	../ChartGeany/include/chartapp.h \
	../ChartGeany/include/debugdialog.h \
	../ChartGeany/include/databrowserdialog.h \
	../ChartGeany/include/datamanagerdialog.h \
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
	../ChartGeany/include/qtachart.h \
	../ChartGeany/include/qtachart_core.h \
	../ChartGeany/include/qtachart_eventfilters.h \
	../ChartGeany/include/qtachart_data.h \
	../ChartGeany/include/qtachart_draw.h \
	../ChartGeany/include/qtachart_functions.h \
	../ChartGeany/include/qtachart_help.h \
	../ChartGeany/include/qtachart_object.h \
	../ChartGeany/include/qtachart_objects.h \
	../ChartGeany/include/qtachart_properties.h \
	../ChartGeany/include/qtcgraphicsitem.h \
	../ChartGeany/include/qtcgraphicsscene.h \
	../database/feedav.h \
	../database/feediex.h \
	../database/feedyahoo.h

SOURCES += MainWindow.cpp \
	../ChartGeany/src/appColorDialog.cpp \
	../ChartGeany/src/chartapp.cpp \
	../ChartGeany/src/common.cpp \
	../ChartGeany/src/debugdialog.cpp \
	../ChartGeany/src/databrowserdialog.cpp \
	../ChartGeany/src/datamanagerdialog.cpp \
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
	../ChartGeany/src/qtachart_core_barchart.cpp \
	../ChartGeany/src/qtachart_core_candlechart.cpp \
	../ChartGeany/src/qtachart_core.cpp \
	../ChartGeany/src/qtachart_core_drawbar.cpp \
	../ChartGeany/src/qtachart_core_drawcandle.cpp \
	../ChartGeany/src/qtachart_core_draw.cpp \
	../ChartGeany/src/qtachart_core_drawpriceline.cpp \
	../ChartGeany/src/qtachart.cpp \
	../ChartGeany/src/qtachart_data.cpp \
	../ChartGeany/src/qtachart_draw.cpp \
	../ChartGeany/src/qtachart_eventfilters.cpp \
	../ChartGeany/src/qtachart_functions.cpp \
	../ChartGeany/src/qtachart_help.cpp \
	../ChartGeany/src/qtachart_methods.cpp \
	../ChartGeany/src/qtachart_object.cpp \
	../ChartGeany/src/qtachart_object_draw.cpp \
	../ChartGeany/src/qtachart_object_modules.cpp \
	../ChartGeany/src/qtachart_object_sanitizer.cpp \
	../ChartGeany/src/qtachart_objects.cpp \
	../ChartGeany/src/qtachart_properties.cpp \
	../ChartGeany/src/qtachart_utilities.cpp \
	../ChartGeany/src/qtcgraphicsitem.cpp \
	../database/idb.cpp \
	../database/csv.cpp \
	../database/dbman.cpp \
	../database/platformstring.cpp \
	../database/feedav.cpp \
	../database/feediex.cpp \
	../database/feedyahoo.cpp \
	../database/sqlcb_datafeeds.cpp \
    ../database/sqlcb_dbversion.cpp \
	../database/sqlcb_symbol_table.cpp \
	../database/sqlcb_toolchain.cpp \

# Third party modules.

LIBS += -L../3rdparty -lta-lib -lx86dis

SOURCES += ../3rdparty/sqlite3/sqlite3.c \
	../3rdparty/simplecrypt/simplecrypt.cpp
