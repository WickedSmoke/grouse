TEMPLATE = subdirs
SUBDIRS  = talib tcc x86dis cgtool ChartGeany

talib.file = 3rdparty/ta-lib.pro
tcc.file = 3rdparty/tcc.pro
x86dis.file = 3rdparty/x86dis.pro
cgtool.depends = tcc
ChartGeany.depends = talib tcc x86dis
