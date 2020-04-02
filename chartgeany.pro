TEMPLATE = subdirs
SUBDIRS  = talib tcc x86dis cgtool ChartGeany grouse

talib.file = 3rdparty/ta-lib.pro
tcc.file = 3rdparty/tcc.pro
x86dis.file = 3rdparty/x86dis.pro

cgtool.depends = tcc

grouse.file = gui/grouse.pro
grouse.depends = talib tcc x86dis

ChartGeany.depends = talib tcc x86dis
