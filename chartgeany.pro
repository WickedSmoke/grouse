TEMPLATE = subdirs
SUBDIRS  = talib tcc qcodeeditor x86dis cgtool ChartGeany grouse

talib.file = 3rdparty/ta-lib.pro
tcc.file = 3rdparty/tcc.pro
qcodeeditor.file = 3rdparty/qcodeeditor.pro
x86dis.file = 3rdparty/x86dis.pro

cgtool.depends = tcc

grouse.file = gui/grouse.pro
grouse.depends = talib tcc x86dis

ChartGeany.depends = talib tcc qcodeeditor x86dis
