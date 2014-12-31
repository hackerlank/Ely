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

AC_PREREQ([2.69])
AC_INIT([Ely], [1.0], [consultit@katamail.com])
AM_INIT_AUTOMAKE
LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_SRCDIR([src/ObjectModel/Object.cpp])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])
AM_SILENT_RULES

# checks for programs.
:${CXXFLAGS=""}
AC_PROG_CXX
AC_LANG([C++])
# default Ely CXXFLAGS (release mode)
ELY_CXXFLAGS="-O3 -w -fmessage-length=0"
buildtype=""
# ELY_DEVEL
AC_ARG_ENABLE([devel],
	AS_HELP_STRING([--enable-devel],
		[@<:@=yes@:>@ enable Development mode (disabled by default)]),
    [devel=${enableval}], [devel=no])
AM_CONDITIONAL([ELY_DEVEL], [test "x${devel}" != xno])
AS_IF([test "x${devel}" != xno], 
	[buildtype=${buildtype}"Devel-"])
# ELY_DEBUG
AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug],
		[@<:@=yes@:>@ enable Debug mode (disabled by default)]),
    [debug=${enableval}], [debug=no])
AS_IF([test "x${debug}" != xno],
	[AC_DEFINE([ELY_DEBUG],[1],[debug mode enabled])
	ELY_CXXFLAGS="-O0 -g3 -Wall -fmessage-length=0"
	buildtype=${buildtype}"Debug"],
	[AC_DEFINE([ELY_RELEASE],[1],[release mode enabled])
	buildtype=${buildtype}"Release"])
# ELY_THREAD
AC_ARG_ENABLE([thread],
    AS_HELP_STRING([--enable-thread],
    	[@<:@=yes@:>@ enable Threading mode (disabled by default)]),
    [thread=${enableval}], [thread=no])
AM_CONDITIONAL([ELY_THREAD], [test "x${thread}" != xno])
AS_IF([test "x${thread}" != xno],
	[AC_DEFINE([ELY_THREAD],[1],[threading mode enabled])
	buildtype=${buildtype}"-Thread"])

# checks for libraries.
AC_SEARCH_LIBS([lt_dlopen], [ltdl])
AX_PTHREAD
LIBS="$PTHREAD_LIBS $LIBS"
CXXFLAGS="$PTHREAD_CFLAGS $CXXFLAGS"

# checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# checks for library functions.
AC_CHECK_FUNCS([pow sqrt strchr strtol])

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

# checks/sets options
AC_DEFINE([WITHIN_PANDA],[1],[within panda enabled])
# checks for Python.
AX_PYTHON
# checks for Python >= 2.7.
AX_PYTHON_PATH
AX_PYTHON_VERSION_ENSURE([2.7])

# checks for Ely dependency packages
ELY_DEPS_SEARCH([${PYTHON_INCLUDE_DIR}], [${PYTHON_LIB}])
# Ely flags for Makefiles
ELY_CPPFLAGS="${ELY_DEPS_CPPFLAGS}"
ELY_LDFLAGS="${ELY_DEPS_LDFLAGS}"
ELY_LIBS="${ELY_DEPS_LIBS}"
AC_SUBST(ELY_CXXFLAGS)
AC_SUBST(ELY_CPPFLAGS)
AC_SUBST(ELY_LDFLAGS)
AC_SUBST(ELY_LIBS)

# checks for Boost Unit Test Framework tests (maintainer only)
AC_ARG_ENABLE([unit-test],
    AS_HELP_STRING([--enable-unit-test],
    	[@<:@=yes@:>@ enable Boost Unit Test programs - maintainer only (disabled by default)]),
    [unit_test=${enableval}], [unit_test=no])
AM_CONDITIONAL([BOOST_TEST], [test "x${unit_test}" != xno])
AM_COND_IF([BOOST_TEST],
	[AX_BOOST_BASE
	AX_BOOST_UNIT_TEST_FRAMEWORK
	AC_CONFIG_FILES([test/Makefile])])

# checks for training tests (maintainer only)
AC_ARG_ENABLE([training],
    AS_HELP_STRING([--enable-training],
    	[@<:@=yes@:>@ enable training test programs - maintainer only (disabled by default)]),
    [training=${enableval}
    AM_PROG_CC_C_O
    ELY_CXXFLAGS="${ELY_CXXFLAGS} -std=c++11"
    ELY_CPPFLAGS="${ELY_CPPFLAGS} -D__GXX_EXPERIMENTAL_CXX0X__"], 
    [training=no])
AM_CONDITIONAL([TRAINING], [test "x${training}" != xno])
AM_COND_IF([TRAINING],
	[AC_CONFIG_FILES([
		training/Makefile
		training/recastnavigation/Makefile
		training/interrogate/Makefile
		training/opensteer/Makefile
		])])

# checks for Ely ELF executable build (if no boost and no training tests)
AM_CONDITIONAL([ELY_ELF], [test "x${unit_test}" = xno -a "x${training}" = xno])
AM_COND_IF([ELY_ELF],
	[AC_CONFIG_FILES([
		include/Makefile
		src/Makefile
		src/AIComponents/Makefile
		src/AudioComponents/Makefile
		src/BehaviorComponents/Makefile
		src/CommonComponents/Makefile
		src/ControlComponents/Makefile
		src/Game/Makefile
		src/ObjectModel/Makefile
		src/PhysicsComponents/Makefile
		src/PhysicsControlComponents/Makefile
		src/SceneComponents/Makefile
		src/Support/Makefile
		src/Support/BulletLocal/Makefile
		src/Support/LibRocketLocal/Makefile
		src/Support/MemoryPool/Makefile
		src/Support/OpenSteerLocal/Makefile
		src/Support/RecastNavigationLocal/Makefile
		src/Support/tinyxml2/Makefile
		src/Utilities/Makefile
		elygame/Makefile
		elygame/callbacks/Makefile
		elygame/data/Makefile
		elygame/data/misc/Makefile
		elygame/data/models/Makefile
		elygame/data/shaders/Makefile
		elygame/data/sounds/Makefile
		elygame/data/textures/Makefile
		elygame/initializations/Makefile
		elygame/instanceupdates/Makefile
		elygame/transitions/Makefile
		doc/Makefile
		])])

AC_CONFIG_FILES([
		Makefile
		])

AC_OUTPUT

echo \
"-------------------------------------------------
${PACKAGE_NAME} Version ${PACKAGE_VERSION}

-Prefix: '${prefix}'.

-${CXX} compiler flags: 
    ' ${ELY_CXXFLAGS} ${ELY_CPPFLAGS}'

-${CXX} linker flags: 
    '${ELY_LDFLAGS} ${ELY_LIBS}'

-Build Type: ${buildtype}

Now type 'make @<:@<target>@:>@'
where the optional <target> is:
	all -->for building all binaries
	install -->for installing everything
--------------------------------------------------"
