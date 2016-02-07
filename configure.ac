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
:${CXXFLAGS=""}
:${CFLAGS=""}
AM_INIT_AUTOMAKE
LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_SRCDIR([src/ObjectModel/Object.cpp])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])
AM_SILENT_RULES

# Checks for programs.
required_programs=
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])

#
AC_SUBST(ELY_CXXFLAGS)
AC_SUBST(ELY_CPPFLAGS)
AC_SUBST(ELY_LDFLAGS)
AC_SUBST(ELY_LIBS)
#
ELY_CPPFLAGS=
# default build: Release
ELY_CXXFLAGS=
buildtype=
# ELY_DEVEL
AC_ARG_ENABLE([devel],
	AS_HELP_STRING([--enable-devel],
		[@<:@=yes@:>@ enable Development mode (disabled by default)]),
    [devel=${enableval}
    if test "x${devel}" != xno ; then
		devel=yes
		buildtype=${buildtype}"Devel-"
	fi
	], 
    [devel=no]
    )
AM_CONDITIONAL([ELY_DEVEL], [test "x${devel}" != xno])

#
AC_ARG_ENABLE([relwithdebinfo],
	AS_HELP_STRING([--enable-relwithdebinfo],
		[@<:@=yes@:>@ enable Release with Debug info mode (disabled by default)]),
    [relwithdebinfo=${enableval}],
    [relwithdebinfo=no]
    )
AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug],
		[@<:@=yes@:>@ enable Debug mode (disabled by default)]),
    [debug=${enableval}],
    [debug=no]
    )

AS_IF(
		#RelWithDebInfo
	[test "x${relwithdebinfo}" != xno],
	[buildtype=${buildtype}"RelWithDebInfo"
	AC_DEFINE([ELY_RELWITHDEBINFO],[1],[release with debug info mode enabled])
	ELY_CXXFLAGS="-O2 -g -fmessage-length=0"],
		#Debug
	[test "x${debug}" != xno],
	[buildtype=${buildtype}"Debug"
	AC_DEFINE([ELY_DEBUG],[1],[debug mode enabled])
	ELY_CXXFLAGS="-O0 -g3 -Wall -Wno-reorder -fmessage-length=0"],
		#Release
	[buildtype=${buildtype}"Release"
	AC_DEFINE([ELY_RELEASE],[1],[release mode enabled])
	ELY_CXXFLAGS="-O3 -w -fmessage-length=0"]
	)	
#
ELY_LDFLAGS=
ELY_LIBS=

#c++d std headers
AX_CXX_HEADER_STDCXX_98

# Checks for libraries.
required_libraries=
#pthread
PTHREAD_LIBS=
AC_SEARCH_LIBS([pthread_create], 
			   [pthread], 
			   [PTHREAD_LIBS=${ac_cv_search_pthread_create}
			   ELY_LIBS="${ELY_LIBS} ${PTHREAD_LIBS}"], 
			   [required_libraries=${required_libraries}" 'pthread' "])
AC_SUBST(PTHREAD_LIBS)
#ltdl
LTDL_LIBS=
AC_SEARCH_LIBS([lt_dlopen], 
			   [ltdl], 
			   [LTDL_LIBS=${ac_cv_search_lt_dlopen}
			   ELY_LIBS="${ELY_LIBS} ${LTDL_LIBS}"], 
			   [required_libraries=${required_libraries}" 'ltdl' "])
AC_SUBST(LTDL_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([float.h stddef.h stdlib.h string.h sys/time.h])

required_headers=
AC_CHECK_HEADER([pthread.h],[],[required_headers=${required_headers}" 'pthread' "])
AC_CHECK_HEADER([ltdl.h],[],[required_headers=${required_headers}" 'ltdl' "])
AX_BOOST_BASE([],
	[ELY_CPPFLAGS="${ELY_CPPFLAGS} ${BOOST_CPPFLAGS}"
	ELY_LDFLAGS="${ELY_LDFLAGS} ${BOOST_LDFLAGS}"],
	[required_headers=${required_headers}" 'Boost' "])
#
STOP_ON_CHECKS_ERR([$required_programs],[$required_libraries],[$required_headers],['Ely'])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memmove memset pow strchr strtol])

# Checks/sets options
AC_DEFINE([WITHIN_PANDA],[1],[within panda enabled])
# Checks for Python version.
ELY_AX_PYTHON([2],[8])
# Checks for Python >= 2.7.
AX_PYTHON_PATH
AX_PYTHON_VERSION_ENSURE([2.7])

# Checks for Ely dependency packages
ELY_DEPS_SEARCH([${PYTHON_INCLUDE_DIR}], [${PYTHON_LIB}])

# Ely flags for Makefiles
ELY_CPPFLAGS="${ELY_CPPFLAGS} ${ELY_DEPS_CPPFLAGS}"
ELY_LDFLAGS="${ELY_LDFLAGS} ${ELY_DEPS_LDFLAGS}"
ELY_LIBS="${ELY_LIBS} ${ELY_DEPS_LIBS}"

#----------------------------------------------------------------------------
# Package configuration switches.
#----------------------------------------------------------------------------
# Threading
AC_ARG_ENABLE([threading],
    AS_HELP_STRING([--enable-threading],
    	[@<:@=yes@:>@ enable Threading mode (disabled by default)]),
    [threading=${enableval}
    if test "x${threading}" != xno ; then
		threading=yes
		# Checks programs for Threading.
		required_programs=
		# Checks libraries for Threading.
		required_libraries=
		# Checks header files for Threading.
		required_headers=
		# Checks for typedefs, structures, and compiler characteristics.
		# Checks for library functions.
		#
		STOP_ON_CHECKS_ERR([$required_programs],[$required_libraries],[$required_headers],['Ely' in Threading mode])
		AC_DEFINE([ELY_THREAD],[1],[threading mode enabled])
		buildtype=${buildtype}"-Threading"
	fi
	], 
    [threading=no])
AM_CONDITIONAL([ELY_THREAD], [test "x${threading}" != xno])

# Unit Tests (maintainer only)
AC_ARG_ENABLE([unit-test],
    AS_HELP_STRING([--enable-unit-test],
    	[@<:@=yes@:>@ enable Boost Unit Test programs - maintainer only (disabled by default)]),
    [unit_test=${enableval}
    if test "x${unit_test}" != xno ; then
		unit_test=yes
		# Checks programs for Unit Tests.
		required_programs=
		# Checks libraries for Unit Tests.
		required_libraries=
		# Checks header files for Unit Tests.
		required_headers=
		#Check Boost Unit Test Framework
		AX_BOOST_UNIT_TEST_FRAMEWORK
		# Checks for typedefs, structures, and compiler characteristics.
		# Checks for library functions.
		#
		STOP_ON_CHECKS_ERR([$required_programs],[$required_libraries],[$required_headers],['Ely' Unit Tests])
    	buildtype="Boost_Unit_Test" 
	fi
	],
    [unit_test=no])
AM_CONDITIONAL([BOOST_TEST], [test "x${unit_test}" != xno])

# Training test programs (maintainer only)
AC_ARG_ENABLE([training],
    AS_HELP_STRING([--enable-training],
    	[@<:@=yes@:>@ enable training test programs - maintainer only (disabled by default)]),
    [training=${enableval}
    if test "x${training}" != xno ; then
		training=yes
		# Checks programs for Training.
		required_programs=
		# Checks libraries for Training.
		required_libraries=
		# Checks header files for Training.
		required_headers=
		AC_CHECK_HEADERS([unistd.h])
		# Checks for typedefs, structures, and compiler characteristics.
		AC_TYPE_INT64_T
		# Checks for library functions.
		AC_CHECK_FUNCS([sqrt])
		#
		STOP_ON_CHECKS_ERR([$required_programs],[$required_libraries],[$required_headers],['Ely' Training test programs])
		AM_PROG_CC_C_O
    	buildtype="Training"
	fi
	],
    [training=no])
AM_CONDITIONAL([TRAINING], [test "x${training}" != xno])

# Set Ely ELF executable build (if no boost and no training tests)
AM_CONDITIONAL([ELY_ELF], [test "x${unit_test}" = xno -a "x${training}" = xno])

#----------------------------------------------------------------------------
# Makefiles.
#----------------------------------------------------------------------------
AC_CONFIG_FILES([
		Makefile
		])
		
AM_COND_IF([BOOST_TEST],
	[AC_CONFIG_FILES([test/Makefile])])

AM_COND_IF([TRAINING],
	[AC_CONFIG_FILES([
		training/Makefile
		training/recastnavigation/Makefile
		training/interrogate/Makefile
		training/networking/Makefile
		training/opensteer/Makefile
		training/particles/Makefile
		])])

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

#----------------------------------------------------------------------------
# Emit generated files.
#----------------------------------------------------------------------------
AC_OUTPUT

echo
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
