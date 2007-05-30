
INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../shared
INCLUDES += -I$(srcdir)/../game

bin_PROGRAMS = antrix

antrix_SOURCES = \
    Main.cpp \
    Master.cpp \
    Master.h \
    CConsole.cpp \
    CConsole.h

antrix_LDADD = -lgame -ldatabase -lconfig -lauth -lshared -lzlib -lnetwork

antrix_LDFLAGS = -L$(srcdir)/../shared \
               -L$(srcdir)/../game \
               -L$(srcdir)/../../dep/src/zlib \
               -Wl,-export-dynamic
#               @DLL_EXPORT@
