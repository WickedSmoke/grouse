TEMPLATE = subdirs
SUBDIRS  = talib tcc cgtool ChartGeany

talib.file = 3rdparty/ta-lib.pro
tcc.file = 3rdparty/tcc.pro
cgtool.depends = tcc
ChartGeany.depends = talib tcc
