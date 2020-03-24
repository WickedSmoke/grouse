TEMPLATE = lib
VERSION  = 0.9.27

CONFIG += staticlib release

OBJECTS_DIR = obj

INCLUDEPATH += libtcc

SOURCES += libtcc/tccasm.c \
	libtcc/tccelf.c \
	libtcc/tccgen.c \
	libtcc/tccpp.c \
	libtcc/tccrun.c \
	libtcc/libtcc.c

config_h.target = libtcc/config.h
config_h.commands = @echo -e $$escape_expand(\")\x23define TCC_VERSION \x220.9.27\x22$$escape_expand(\") >libtcc/config.h
QMAKE_EXTRA_TARGETS = config_h
