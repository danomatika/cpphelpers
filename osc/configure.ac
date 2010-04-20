#########################################
##### Prelude #####

AC_INIT([oscframework], [0.1.0], [danomatika@gmail.com])
AC_CONFIG_SRCDIR([src/oscframework/oscframework.h])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

#########################################
##### Build Options #####

DEBUG_CXXFLAGS="-g -Wall -Werror -Wno-uninitialized -O0 -fvisibility=hidden"
RELEASE_CXXFLAGS="-g -O2"

#########################################
##### Check for programs/libs #####

# check progs
AC_PROG_CC
AC_PROG_RANLIB

# using c++ compiler and linker
AC_LANG_CPLUSPLUS

# check for headers
AC_HEADER_STDC
	
# check for headers & libs
PKG_CHECK_MODULES(LO, liblo >= 0.23,[],
	AC_MSG_ERROR([lo library >= 0.23 not found]))

#########################################
##### Options #####

# debug build switch
# from http://www.bioinf.uni-freiburg.de/~mmann/HowTo/automake.html#debug
AC_MSG_CHECKING([whether to enable the debug build])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug build (def=no)])],
    [enable_debug="$enableval"],
    [enable_debug=no])
AC_MSG_RESULT([$enable_debug])

if test x"$enable_debug" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    CXXFLAGS="$DEBUG_CXXFLAGS"
else
    AC_DEFINE([NDEBUG],[],[Release Mode])
    CXXFLAGS="$RELEASE_CXXFLAGS"
fi

#########################################
##### Externals #####

# externals with configure scripts
AC_CONFIG_SUBDIRS([])

#########################################
##### Output files #####

AC_CONFIG_HEADERS([src/oscframework/config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/oscframework/Makefile
	src/tests/Makefile
])
AC_OUTPUT

#########################################
##### Print a summary for the user #####

#AC_MSG_RESULT([
#	oscframework $VERSION is now configured
#
#	Source directory:     $srcdir
#	Installation prefix:  $prefix
#	C++ compiler:         $CXX $CXXFLAGS
#	Debug build:          $enable_debug
#])
