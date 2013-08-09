#                                               -*- Autoconf -*-
# Process this file with auto(re)conf to produce a configure script.

###Standard `configure.ac' Layout###
# Autoconf requirements
# `AC_INIT(PACKAGE, VERSION, BUG-REPORT-ADDRESS)'
# information on the package
# checks for programs
# checks for libraries
# checks for header files
# checks for types
# checks for structures
# checks for compiler characteristics
# checks for library functions
# checks for system services
# `AC_CONFIG_FILES([FILE...])'
# `AC_OUTPUT'

AC_PREREQ([2.67])
AC_INIT([Ely], [1.0], [consultit@katamail.com])
AM_INIT_AUTOMAKE
LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/Ely.cpp])
AC_CONFIG_HEADERS([include/config.h])
AC_PREFIX_DEFAULT([/usr])

# checks for programs.
:${CXXFLAGS=""}
AC_PROG_CXX
AC_LANG([C++])
# default Ely CXXFLAGS (release mode)
ELY_CXXFLAGS="-O3 -w -fmessage-length=0"

# checks/sets options
AC_DEFINE([WITHIN_PANDA],[1],[within panda enabled])
# ELY_DEVEL
AC_ARG_ENABLE([ely-devel],
	AS_HELP_STRING([--enable-ely-devel],
		[=yes enable Ely development mode (disabled by default)]),
    [ely_devel=${enableval}], [ely_devel=no])
AM_CONDITIONAL([ELY_DEVEL], [test "x${ely_devel}" = xyes])
# ELY_DEBUG
AC_ARG_ENABLE([ely-debug],
	AS_HELP_STRING([--enable-ely-debug],
		[=yes enable Ely debug mode (disabled by default)]),
    [ely_debug=${enableval}], [ely_debug=no])
AS_IF([test "x${ely_debug}" = xyes],
	[AC_DEFINE([ELY_DEBUG],[1],[debug mode enabled])
	ELY_CXXFLAGS="-O0 -g3 -Wall -fmessage-length=0"],
	[AC_DEFINE([NDEBUG],[1],[release mode enabled])])
# enables silent rules
#AS_IF([test "x${ely_devel}" != xyes -a "x${ely_debug}" != xyes],
#	[AM_SILENT_RULES])
# ELY_THREAD
AC_ARG_ENABLE([ely-thread],
    AS_HELP_STRING([--enable-ely-thread],
    	[=yes enable Ely threading mode (disabled by default)]),
    [ely_thread=${enableval}], [ely_thread=no])
AS_IF([test "x${ely_thread}" = xyes],
	[AC_DEFINE([ELY_THREAD],[1],[threading mode enabled])])

# checks for libraries.
AC_SEARCH_LIBS([lt_dlopen], [ltdl])

# checks for standard header files.
AC_CHECK_HEADERS([stdint.h ltdl.h])
AX_CXX_HEADER_STDCXX_98
AX_BOOST_BASE([],[],[
	AC_MSG_ERROR([
	----------------------------------------
	The Boost header files are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
])

# checks Pyhton 2.6.
AX_PYTHON_DEVEL([>= '2.6'])
# checks Panda3d sdk environment
PANDA3D_SEARCH([${PYTHON_CPPFLAGS}])
# Ely flags for Makefiles
ELY_CPPFLAGS="${PANDA3D_CPPFLAGS}"
ELY_LDFLAGS="${PANDA3D_LDFLAGS}"
ELY_LIBS="${PANDA3D_LIBS}"
AC_SUBST(ELY_CXXFLAGS)
AC_SUBST(ELY_CPPFLAGS)
AC_SUBST(ELY_LDFLAGS)
AC_SUBST(ELY_LIBS)
#libs for using RN with Physics bullet character
ELY_RN_PHYSICS_LIBS="${PANDA3D_RN_PHYSICS_LIBS}"
AC_SUBST(ELY_RN_PHYSICS_LIBS)

# checks for Boost Unit Test Framework tests (mantainer only)
AC_ARG_ENABLE([ely-test],
    AS_HELP_STRING([--enable-ely-test],
    	[=yes enable Ely Boost Unit Test programs - mantainer only (disabled by default)]),
    [ely_test=${enableval}], [ely_test=no])
AM_CONDITIONAL([BOOST_TEST], [test "x${ely_test}" = xyes])
AM_COND_IF([BOOST_TEST],
	[AX_BOOST_BASE
	AX_BOOST_UNIT_TEST_FRAMEWORK
	AC_CONFIG_FILES([src/test/Makefile])])

# checks for training tests (mantainer only)
AC_ARG_ENABLE([ely-training],
    AS_HELP_STRING([--enable-ely-training],
    	[=yes enable Ely training test programs - mantainer only (disabled by default)]),
    [ely_training=${enableval}], [ely_training=no])
AM_CONDITIONAL([TRAINING], [test "x${ely_training}" = xyes])
AM_COND_IF([TRAINING],
	[AM_PROG_CC_C_O
	AC_CONFIG_FILES([training/Makefile])])

# checks for Ely ELF executable build (if no boost and no training tests)
AM_CONDITIONAL([ELY_ELF], [test "x${ely_test}" != xyes -a "x${ely_training}" != xyes])
AM_COND_IF([ELY_ELF],
	[AC_CONFIG_FILES([
		include/Makefile
		callbacks/Makefile
		transitions/Makefile
		initializations/Makefile
		tinyxml2/Makefile
		src/Makefile
		doc/Makefile
		data/Makefile
		data/models/Makefile
		data/shaders/Makefile
		data/sounds/Makefile
		data/textures/Makefile
		])])

# checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# checks for library functions.
AC_CHECK_FUNCS([pow sqrt strchr strtol])

AC_CONFIG_FILES([
		Makefile
		])

AC_OUTPUT
