TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    bond005_lvcsr_lib.c \
    backtrack_pointers_lib.c \
    command_prompt_lib.c

HEADERS += \
    bond005_lvcsr_lib.h \
    backtrack_pointers_lib.h \
    command_prompt_lib.h

win32:QMAKE_CFLAGS_RELEASE += /fp:fast /Ox /arch:SSE2
unix:QMAKE_CFLAGS_RELEASE += -O3 -march=native -mfpmath=sse -msse2 -funroll-loops -ffast-math
