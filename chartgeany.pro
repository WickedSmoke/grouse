TEMPLATE = subdirs
SUBDIRS  = talib tcc x86dis cgtool ChartGeany scat

talib.file = 3rdparty/ta-lib.pro
tcc.file = 3rdparty/tcc.pro
x86dis.file = 3rdparty/x86dis.pro

cgtool.depends = tcc

scat.file = gui/scat.pro
scat.depends = talib tcc x86dis

ChartGeany.depends = talib tcc x86dis
