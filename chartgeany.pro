TEMPLATE = subdirs
SUBDIRS  = talib cgtool ChartGeany

talib.file = 3rdparty/ta-lib.pro
ChartGeany.depends = talib
