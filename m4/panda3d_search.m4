# PANDA3D_SEARCH
# --------------
# Defines: PANDA3D_CPPFLAGS, PANDA3D_LDFLAGS, PANDA3D_LIBS
# Argument: expected PYTHON_CPPFLAGS
# Searches SDK first on cmd line flags then on std locations
#
AC_DEFUN([PANDA3D_SEARCH], 
[
CPPFLAGS_CMDLINE=${CPPFLAGS}
LDFLAGS_CMDLINE=${LDFLAGS}
LIBS_CMDLINE=${LIBS}
AC_MSG_NOTICE([Looking for Panda3d SDK environment...])
# check headers first from cmd line specified ones
PANDA3D_CPPFLAGS="-I/usr/include/panda3d -I/usr/local/include/panda3d $1"
CPPFLAGS="${PANDA3D_CPPFLAGS} ${CPPFLAGS}"
AC_CHECK_HEADERS([pandaFramework.h])
if test "x${ac_cv_header_pandaFramework_h}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The Panda3d header files are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi	
# check libraries first from cmd line specified ones
PANDA3D_LDFLAGS="-L/usr/lib/panda3d -L/usr/local/lib/panda3d"
PANDA3D_LIBS="-lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
			-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct -lpandabullet"
			
LDFLAGS="${PANDA3D_LDFLAGS} ${LDFLAGS}"
LIBS="${PANDA3D_LIBS} ${LIBS}"
required_libraries=yes
panda3d_prologue="#include <pandaFramework.h>"
panda3d_body="
	int argc=1;
	char** argv=NULL;
  	PandaFramework framework;
  	framework.open_framework(argc, argv);
  	framework.close_framework()
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$panda3d_prologue],[$panda3d_body])],
  AC_MSG_NOTICE([Panda3d SDK seems OK]) 
  AC_DEFINE([HAVE_PANDA3D], 1, [Panda3d SDK enabled]),
  [required_libraries=Panda3d]
)
if test "x${required_libraries}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The Panda3d libraries are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi

CPPFLAGS=${CPPFLAGS_CMDLINE}
LDFLAGS=${LDFLAGS_CMDLINE}
LIBS=${LIBS_CMDLINE}
])# PANDA3D_SEARCH
