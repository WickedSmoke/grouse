TEMPLATE = lib

CONFIG += staticlib release

OBJECTS_DIR = obj

INCLUDEPATH += x86dis/include

SOURCES += x86dis/src/ia32_implicit.c \
	x86dis/src/ia32_insn.c \
	x86dis/src/ia32_invariant.c \
	x86dis/src/ia32_modrm.c \
	x86dis/src/ia32_opcode_tables.c \
	x86dis/src/ia32_operand.c \
	x86dis/src/ia32_reg.c \
	x86dis/src/ia32_settings.c \
	x86dis/src/x86_disasm.c \
	x86dis/src/x86_format.c \
	x86dis/src/x86_imm.c \
	x86dis/src/x86_insn.c \
	x86dis/src/x86_misc.c \
	x86dis/src/x86_operand_list.c
