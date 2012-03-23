#
#
AC_PREREQ(2.53)
AC_INIT(livecast_monitor, 0.0.1)
AC_CONFIG_SRCDIR(src/livecast_monitor.cc)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

#
#
AC_PROG_CC([gcc46 gcc cc])
AC_PROG_CXX([g++46 g++ cpp])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_STDC_HEADERS
AC_SYS_LARGEFILE
AC_LANG_CPLUSPLUS
AC_C_BIGENDIAN

#
#
CFLAGS=-ggdb
CXXFLAGS=-ggdb

#
#        
AC_ARG_WITH([version], 
            AC_HELP_STRING([--with-version], [version number]), 
            [CXXFLAGS="$CXXFLAGS -DSTREAMDUP_VERSION=\"\\\"$withval\\\"\""], 
            [])
AC_ARG_WITH([verbose], 
            AC_HELP_STRING([--with-verbose], [verbose level 0-9 (default: 0)]), 
            [CXXFLAGS="$CXXFLAGS -D__VERBOSE__=$withval -DDEBUGOUTPUT"], 
            [])
AC_ARG_WITH([memory-check], 
            AC_HELP_STRING(
            [--with-memory-check], 
            [compile with memory check]), 
            [CXXFLAGS="$CXXFLAGS -Wstack-protector -fstack-protector -fstack-protector-all"], 
            [])
AC_ARG_ENABLE([debug], 
              AC_HELP_STRING([--enable-debug], [compile with debug option]), 
              [CXXFLAGS="$CXXFLAGS -W -Wall -Werror -ggdb -O0 -D__DEBUG__ -DGLIBCXX_FORCE_NEW"], 
              [CXXFLAGS="$CXXFLAGS -O2 -DNDEBUG"])

#
# boost first
AX_CHECK_BOOST()
CXXFLAGS="$CXXFLAGS $BOOST_CFLAGS"
CFLAGS="$CFLAGS $BOOST_CFLAGS"
LIBS="$LIBS $BOOST_LIBS"

#
# wx widget
# CXXFGLAGS="wxgtk2-2.6-config --cflags"
CXXFLAGS="$CXXFLAGS -I/usr/local/include/wx-2.6/gtk2-ansi-release-2.6 -I/usr/local/include/wx-2.6 -DGTK_NO_CHECK_CASTS -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -DNO_GCC_PRAGMA"
LIBS="$LIBS -L/usr/local/lib  -pthread -L/usr/local/lib -liconv   -lwx_gtk2_xrc-2.6 -lwx_gtk2_qa-2.6 -lwx_gtk2_html-2.6 -lwx_gtk2_adv-2.6 -lwx_gtk2_core-2.6 -lwx_base_xml-2.6 -lwx_base_net-2.6 -lwx_base-2.6"

#
# mysql
AX_CHECK_MYSQL()
CXXFLAGS="$CXXFLAGS $MYSQL_INCLUDE"
LIBS="$LIBS $MYSQL_LIBS"

#
# valgrind
PKG_CHECK_MODULES(VALGRIND, [valgrind], valgrind_detected=yes, valgrind_detected=no)
if test x"$valgrind_detected" = xyes; then
   AC_DEFINE_UNQUOTED(HAVE_VALGRIND, 1, [True if valgrind headers have been found])
fi

#
# doxygen
GN_DOC="no"
HAVE_DOT="NO"
AC_ARG_ENABLE(doc, [  --enable-doc            enable dev documentation generation],[GN_DOC=$enable_doc])

if test "x$GN_DOC" = "xyes"
then
	#
	# check doxygen binary
	AC_CHECK_PROG([DOXYGEN],[doxygen],[yes],[no])
	if test "x$DOXYGEN" = "xno"
	then
		AC_MSG_ERROR(Unable to create doc : can't find doxygen)
	fi

	#
	# check dot
	AC_CHECK_PROG([DOT],[dot],[yes],[no])
	if test "$DOT" = "yes"
	then
		HAVE_DOT="YES"
	fi

	#
	# check pdflatex
	AC_CHECK_PROG([PDFLATEX],[pdflatex],[yes],[no])
	if test "$PDFLATEX" = "yes"
	then
		HAVE_PDFLATEX="yes"
	fi
fi

AC_SUBST(HAVE_DOT)
AM_CONDITIONAL(GENERATED_DOC, test "$GN_DOC" = "yes")
AM_CONDITIONAL(HAVE_PDFLATEX, test "x$HAVE_PDFLATEX" = "xyes")

#
#
AC_CONFIG_FILES([
  Makefile 
  src/Makefile 
  src/gui/Makefile
  src/lib/Makefile
  src/monitor/Makefile
])
AC_OUTPUT
