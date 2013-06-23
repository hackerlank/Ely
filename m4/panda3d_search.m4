# PANDA3D_SEARCH
# --------------
# Defines: PANDA3D_CPPFLAGS, PANDA3D_LDFLAGS, PANDA3D_LIBS, PANDA3D_RN_PHYSICS_LIBS
# Argument: expected PYTHON_CPPFLAGS
# Searches SDK first on cmd line flags then on std locations
#
AC_DEFUN([PANDA3D_SEARCH], 
[
CPPFLAGS_CMDLINE=${CPPFLAGS}
LDFLAGS_CMDLINE=${LDFLAGS}
LIBS_CMDLINE=${LIBS}
AC_MSG_NOTICE([Looking for Panda3d SDK environment...])
#
# PANDA3D_THIRDPARTY_SEARCH
#
###Bullet###
# check header first from cmd line specified include
AC_MSG_NOTICE([Looking for Bullet headers...])
BULLET_CPPFLAGS="-I/usr/include/bullet -I/usr/local/include/bullet"
CPPFLAGS="${BULLET_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADERS([Bullet-C-Api.h])
if test "x${ac_cv_header_Bullet_C_Api_h}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The Bullet header files are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi
# check libraries first from cmd line specified ones
BULLET_LDFLAGS=""
BULLET_LIBS=""
#
###Eigen###
# check header first from cmd line specified include
AC_MSG_NOTICE([Looking for Eigen headers...])
EIGEN_CPPFLAGS="-I/usr/include/eigen3 -I/usr/local/include/eigen3 -I/usr/include/eigen2 -I/usr/local/include/eigen2"
CPPFLAGS="${EIGEN_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADERS([Eigen/Dense])
if test "x${ac_cv_header_Eigen_Dense}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The Eigen (3 or 2) header files are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi
# check libraries first from cmd line specified ones
EIGEN_LDFLAGS=""
EIGEN_LIBS=""
#
###Recast Navigation###
# check header first from cmd line specified include
AC_MSG_NOTICE([Looking for Recast Navigation headers...])
RN_CPPFLAGS="-I/usr/include/librecastnavigation-dev -I/usr/local/include/librecastnavigation-dev"
CPPFLAGS="${RN_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADERS([Recast.h])
if test "x${ac_cv_header_Recast_h}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The Recast Navigation header files are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi
# check libraries first from cmd line specified ones
RN_LDFLAGS=""
RN_LIBS="-lrecastnavigation"
#RN_PHYSICS_LIBS is not included in PANDA3D_LIBS
RN_PHYSICS_LIBS="-lrecastnavigationphysics"
			
LDFLAGS="${RN_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${RN_LIBS} ${LIBS_CMDLINE}"
required_libraries=yes
rn_prologue="#include <Recast.h>"
rn_body="
	int argc=1;
	char** argv=0;
  	rcContext rc(true);
  	rc.startTimer(RC_TIMER_TOTAL);
  	rc.stopTimer(RC_TIMER_TOTAL)
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$rn_prologue],[$rn_body])],
  AC_MSG_NOTICE([Recast Navigation libraries... yes]) 
  AC_DEFINE([HAVE_RN], 1, [Recast Navigation enabled]),
  [required_libraries="Recast Navigation"]
)
if test "x${required_libraries}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The ${required_libraries} libraries are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi
#
#Define third party flags
PANDA3D_THIRDPARTY_CPPFLAGS="${BULLET_CPPFLAGS} ${EIGEN_CPPFLAGS} ${RN_CPPFLAGS}"
PANDA3D_THIRDPARTY_LDFLAGS="${BULLET_LDFLAGS} ${EIGEN_LDFLAGS} ${RN_LDFLAGS}"
PANDA3D_THIRDPARTY_LIBS="${BULLET_LIBS} ${EIGEN_LIBS} ${RN_LIBS}"
PANDA3D_THIRDPARTY_RN_PHYSICS_LIBS="${BULLET_LIBS} ${EIGEN_LIBS} ${RN_PHYSICS_LIBS}"
#
###Panda3d SDK###
# check headers first from cmd line specified ones
PANDA3D_CPPFLAGS="-I/usr/include/panda3d -I/usr/local/include/panda3d ${PANDA3D_THIRDPARTY_CPPFLAGS} $1"
CPPFLAGS="${PANDA3D_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
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
#remaining libraries: 
PANDA3D_LDFLAGS="-L/usr/lib/panda3d -L/usr/lib64/panda3d \
				-L/usr/local/lib/panda3d ${PANDA3D_THIRDPARTY_LDFLAGS}"
PANDA3D_LIBS="-lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
			-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct -lpandabullet \
			-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision \
			-lpandagl -lpandaode -lpandaphysics -lpandaskel -lp3ptloader ${PANDA3D_THIRDPARTY_LIBS}"
PANDA3D_RN_PHYSICS_LIBS="-lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
			-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct -lpandabullet \
			-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision \
			-lpandagl -lpandaode -lpandaphysics -lpandaskel -lp3ptloader ${PANDA3D_THIRDPARTY_RN_PHYSICS_LIBS}"
			
LDFLAGS="${PANDA3D_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${PANDA3D_LIBS} ${LIBS_CMDLINE}"
required_libraries=yes
panda3d_prologue="#include <pandaFramework.h>"
panda3d_body="
	int argc=1;
	char** argv=0;
  	PandaFramework framework;
  	framework.open_framework(argc, argv);
  	framework.close_framework()
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$panda3d_prologue],[$panda3d_body])],
  AC_MSG_NOTICE([Panda3d SDK libraries... yes]) 
  AC_DEFINE([HAVE_PANDA3D], 1, [Panda3d SDK enabled]),
  [required_libraries="Panda3d"]
)
if test "x${required_libraries}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The ${required_libraries} libraries are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi

CPPFLAGS=${CPPFLAGS_CMDLINE}
LDFLAGS=${LDFLAGS_CMDLINE}
LIBS=${LIBS_CMDLINE}
])# PANDA3D_SEARCH
