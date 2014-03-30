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
AC_MSG_NOTICE([Looking for Bullet libraries...])
BULLET_LDFLAGS=""
BULLET_LIBS="-lBulletDynamics -lBulletCollision -lLinearMath -lBulletSoftBody"

LDFLAGS="${BULLET_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${BULLET_LIBS} ${LIBS_CMDLINE}"
required_libraries=yes
bullet_prologue="#include <btBulletCollisionCommon.h>"
bullet_body="
	int argc=1;
	char** argv=0;
  	btCollisionWorld(NULL, NULL, NULL);
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$bullet_prologue],[$bullet_body])],
  AC_MSG_NOTICE([Bullet libraries... yes]) 
  AC_DEFINE([HAVE_RN], 1, [Bullet enabled]),
  [required_libraries="Bullet"]
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
AC_MSG_NOTICE([Looking for Recast Navigation libraries...])
RN_LDFLAGS=""
RN_LIBS="-lrecastnavigation"
			
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
###OpenSteer###
# check header first from cmd line specified include
AC_MSG_NOTICE([Looking for OpenSteer headers...])
OS_CPPFLAGS="-I/usr/include/OpenSteer -I/usr/local/include/OpenSteer"
CPPFLAGS="${OS_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADERS([SteerLibrary.h])
if test "x${ac_cv_header_SteerLibrary_h}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The OpenSteer header files are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi
# check libraries first from cmd line specified ones
AC_MSG_NOTICE([Looking for OpenSteer libraries...])
OS_LDFLAGS=""
OS_LIBS="-lOpenSteer"
			
LDFLAGS="${OS_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${OS_LIBS} ${LIBS_CMDLINE}"
required_libraries=yes
os_prologue="#include <SteerLibrary.h>
	class _OSTest
	{
	public:
		OpenSteer::Vec3 velocity(){return OpenSteer::Vec3(0,0,0);}
		float maxSpeed(){return 0.0;}
		float speed(){return 0.0;}
		float radius(){return 0.0;}
		float maxForce(){return 0.0;}
		OpenSteer::Vec3 forward(){return OpenSteer::Vec3(0,0,0);}
		OpenSteer::Vec3 position(){return OpenSteer::Vec3(0,0,0);}
		OpenSteer::Vec3 side(){return OpenSteer::Vec3(0,0,0);}
		OpenSteer::Vec3 up(){return OpenSteer::Vec3(0,0,0);}
		OpenSteer::Vec3 predictFuturePosition(){return OpenSteer::Vec3(0,0,0);}
	};
	typedef OpenSteer::SteerLibraryMixin<_OSTest> OSTest;
	namespace OpenSteer{
	void drawLine(OpenSteer::Vec3 const&, OpenSteer::Vec3 const&, OpenSteer::Color const&){}
	bool enableAnnotation;
	void deferredDrawLine(OpenSteer::Vec3 const&, OpenSteer::Vec3 const&, OpenSteer::Color const&){}
	void drawCameraLookAt(OpenSteer::Vec3 const&, OpenSteer::Vec3 const&, OpenSteer::Vec3 const&){}
	bool drawPhaseActive;
	}
	"
os_body="
	int argc=1;
	char** argv=0;
	OSTest test;
	test.reset();
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$os_prologue],[$os_body])],
  AC_MSG_NOTICE([OpenSteer libraries... yes]) 
  AC_DEFINE([HAVE_OS], 1, [OpenSteer enabled]),
  [required_libraries="OpenSteer"]
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
###libRocket###
# check header first from cmd line specified include
AC_MSG_NOTICE([Looking for libRocket headers...])
ROCKET_CPPFLAGS="-I/usr/include/Rocket -I/usr/local/include/Rocket"
CPPFLAGS="${ROCKET_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADERS([Core.h])
if test "x${ac_cv_header_Core_h}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The libRocket header files are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi
# check libraries first from cmd line specified ones
AC_MSG_NOTICE([Looking for libRocket libraries...])
ROCKET_LDFLAGS=""
ROCKET_LIBS="-lRocketControls -lRocketCore -lRocketDebugger"

LDFLAGS="${ROCKET_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${ROCKET_LIBS} ${LIBS_CMDLINE}"
required_libraries=yes
rocket_prologue="#include <Core.h>"

rocket_body="
	int argc=1;
	char** argv=0;
	Rocket::Core::Initialise();
	Rocket::Core::Shutdown();
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$rocket_prologue],[$rocket_body])],
  AC_MSG_NOTICE([libRocket libraries... yes]) 
  AC_DEFINE([HAVE_ROCKET], 1, [libRocket enabled]),
  [required_libraries="libRocket"]
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
PANDA3D_THIRDPARTY_CPPFLAGS="${BULLET_CPPFLAGS} ${RN_CPPFLAGS} ${OS_CPPFLAGS} ${ROCKET_CPPFLAGS}"
PANDA3D_THIRDPARTY_LDFLAGS="${BULLET_LDFLAGS} ${RN_LDFLAGS} ${OS_LDFLAGS} ${ROCKET_LDFLAGS}"
PANDA3D_THIRDPARTY_LIBS="${BULLET_LIBS} ${RN_LIBS} ${OS_LIBS} ${ROCKET_LIBS}"
#
###Panda3d SDK###
#prerequisites: Eigen, libRocket
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
###Panda3d SDK###
# check headers first from cmd line specified ones
AC_MSG_NOTICE([Looking for Panda3d SDK headers...])
PANDA3DSDK_CPPFLAGS="-I/usr/include/panda3d -I/usr/local/include/panda3d -I$1 ${EIGEN_CPPFLAGS}"
CPPFLAGS="${PANDA3DSDK_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
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
AC_MSG_NOTICE([Looking for Panda3d SDK libraries...])
#remaining libraries: 
PANDA3DSDK_LDFLAGS="-L/usr/lib/panda3d -L/usr/lib64/panda3d \
				-L/usr/local/lib/panda3d ${EIGEN_LDFLAGS}"
PANDA3DSDK_LIBS="-lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
			-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct -lpandabullet \
			-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision -lp3rocket \
			-lpandagl -lpandaode -lpandaphysics -lpandaskel -lp3ptloader -l$2 ${EIGEN_LIBS}"
			
LDFLAGS="${PANDA3DSDK_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${PANDA3DSDK_LIBS} ${LIBS_CMDLINE}"
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
#PANDA3D flags
PANDA3D_CPPFLAGS="${PANDA3DSDK_CPPFLAGS} ${PANDA3D_THIRDPARTY_CPPFLAGS}"
PANDA3D_LDFLAGS="${PANDA3DSDK_LDFLAGS} ${PANDA3D_THIRDPARTY_LDFLAGS}"
PANDA3D_LIBS="${PANDA3DSDK_LIBS} ${PANDA3D_THIRDPARTY_LIBS}"
#
CPPFLAGS=${CPPFLAGS_CMDLINE}
LDFLAGS=${LDFLAGS_CMDLINE}
LIBS=${LIBS_CMDLINE}
])# PANDA3D_SEARCH
