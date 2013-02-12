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

# Checks for programs.
:${CXXFLAGS=""}
AC_PROG_CXX
AC_LANG([C++])

# Default Ely CXXFLAGS
ELY_CXXFLAGS="-O3 -Wall -c -fmessage-length=0"
# Check options
AC_DEFINE([WITHIN_PANDA],[1],[within panda enabled])
# DEBUG
AC_ARG_ENABLE([ely-debug],
	AS_HELP_STRING([--enable-ely-debug],
		[enable Ely debug features (disabled by default)]),
    [ely_debug=${enableval}], [ely_debug=no])
if test "x${ely_debug}" = xyes; then
    AC_DEFINE([DEBUG],[1],[debug features enabled])
    ELY_CXXFLAGS="-O0 -g3 -Wall -c -fmessage-length=0"
fi
# ELY_THREAD
AC_ARG_ENABLE([ely-thread],
    AS_HELP_STRING([--enable-ely-thread],
    	[enable Ely threading mode (disabled by default)]),
    [ely_thread=${enableval}], [ely_thread=no])
if test "x${ely_thread}" = xyes; then
    AC_DEFINE([ELY_THREAD],[1],[ely thread enabled])
fi

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lboost_unit_test_framework':
#AC_CHECK_LIB([boost_unit_test_framework], [main])

# checks for standard header files.
AC_CHECK_HEADERS([stdint.h])
AX_CXX_HEADER_STDCXX_98
# checks for pyhton 2.6 headers files.
AX_PYTHON_DEVEL([>= '2.6'])
# bullet headers...
BULLET_SEARCH
# panda3d sdk
PANDA3D_SEARCH([${PYTHON_CPPFLAGS}])
ELY_CPPFLAGS="${BULLET_CPPFLAGS} ${PANDA3D_CPPFLAGS}"
ELY_LDFLAGS="${PANDA3D_LDFLAGS}"
# Ely flags for Makefiles
AC_SUBST(ELY_CPPFLAGS)
AC_SUBST(ELY_CXXFLAGS)
AC_SUBST(ELY_LDFLAGS)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt strchr strtol])

AC_CONFIG_FILES([
		Makefile
		callbacks/Makefile
		transitions/Makefile
		initializations/Makefile
		include/Makefile
		src/Makefile
		src/test/Makefile
		tinyxml2/Makefile
		training/Makefile
		])
		
AC_OUTPUT
