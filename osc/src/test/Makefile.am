# osc tests

# programs to build
noinst_PROGRAMS = oftest

# bin sources, headers here because we dont want to install them
oftest_SOURCES = Main.cpp

# local libraries needed to build (builddir)
oftest_LDADD = -L../osc -losc

# include paths
oftest_CPPFLAGS = $(CXXFLAGS) \
        $(LO_CFLAGS) -I$(top_srcdir)/src

# libs to link
oftest_LDFLAGS = $(LO_LIBS)

