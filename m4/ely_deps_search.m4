# ELY_DEPS_SEARCH
# --------------
# Defines: ELY_DEPS_CPPFLAGS, ELY_DEPS_LDFLAGS, ELY_DEPS_LIBS
# Argument: expected PYTHON_CPPFLAGS
# Searches SDK first on cmd line flags then on std locations
#
AC_DEFUN([ELY_DEPS_SEARCH], 
[
CPPFLAGS_CMDLINE=${CPPFLAGS}
LDFLAGS_CMDLINE=${LDFLAGS}
LIBS_CMDLINE=${LIBS}

# check LIBRARIES first from cmd line specified ones
AC_MSG_NOTICE([searching for Ely dependency library packages...])
CPPFLAGS=
required_libraries=
###Bullet###
AC_MSG_CHECKING([for Bullet libs])
BULLET_LDFLAGS=""
BULLET_LIBS="-lBulletDynamics -lBulletCollision -lLinearMath -lBulletSoftBody"
LDFLAGS="${BULLET_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${BULLET_LIBS} ${LIBS_CMDLINE}"
bullet_prologue="
	class btDispatcher;
	class btBroadphaseInterface;
	class btCollisionConfiguration;
	class btCollisionWorld
	{
	public:
		btCollisionWorld(btDispatcher*,
			btBroadphaseInterface*,btCollisionConfiguration*){}
	};
	"
bullet_body="
	int argc=1;
	char** argv=0;
  	btCollisionWorld(0, 0, 0);
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$bullet_prologue],[$bullet_body])],
  [AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_RN], 1, [Bullet enabled])],
  [AC_MSG_RESULT([no])
  required_libraries="${required_libraries}'Bullet'"]
)
###Recast Navigation###
AC_MSG_CHECKING([for Recast Navigation libs])
RN_LDFLAGS=""
RN_LIBS="-lrecastnavigation"			
LDFLAGS="${RN_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${RN_LIBS} ${LIBS_CMDLINE}"
rn_prologue="
	enum rcTimerLabel
	{
		RC_TIMER_TOTAL
	};
	class rcContext
	{
	public:
		inline rcContext(bool){}
		inline void startTimer(const rcTimerLabel){}
		inline void stopTimer(const rcTimerLabel){}
	};
	"
rn_body="
	int argc=1;
	char** argv=0;
  	rcContext rc(true);
  	rc.startTimer(RC_TIMER_TOTAL);
  	rc.stopTimer(RC_TIMER_TOTAL)
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$rn_prologue],[$rn_body])],
  [AC_MSG_RESULT([yes]) 
  AC_DEFINE([HAVE_RN], 1, [Recast Navigation enabled])],
  [AC_MSG_RESULT([no])
  required_libraries="${required_libraries} 'Recast Navigation'"]
)
###OpenSteer###
# check libraries first from cmd line specified ones
AC_MSG_CHECKING([for OpenSteer libs])
OS_LDFLAGS=""
OS_LIBS="-lOpenSteer"			
LDFLAGS="${OS_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${OS_LIBS} ${LIBS_CMDLINE}"
os_prologue="
	namespace OpenSteer {
	class Vec3{};
	class Color{};
    void drawLine(Vec3 const&,Vec3 const&,Color const&){}
	void deferredDrawLine(Vec3 const&,Vec3 const&,Color const&){}
	void drawCameraLookAt(Vec3 const&,Vec3 const&,Vec3 const&){}
	bool enableAnnotation;
	bool drawPhaseActive;
	class SimpleVehicle
    {
    public:
        SimpleVehicle(){}
        void reset(){}	
    };
    }
	"
os_body="
	int argc=1;
	char** argv=0;
	OpenSteer::SimpleVehicle vehicle;
	vehicle.reset();
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$os_prologue],[$os_body])],
  [AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_OS], 1, [OpenSteer enabled])],
  [AC_MSG_RESULT([no])
  required_libraries="${required_libraries} 'OpenSteer'"]
)
###libRocket###
AC_MSG_CHECKING([for libRocket libs])
ROCKET_LDFLAGS="-L/usr/lib/rocket -L/usr/local/lib/rocket"
ROCKET_LIBS="-lRocketControls -lRocketCore -lRocketDebugger"
LDFLAGS="${ROCKET_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${ROCKET_LIBS} ${LIBS_CMDLINE}"
rocket_prologue="
	namespace Rocket {
	namespace Core {
	bool Initialise();
	void Shutdown();
	}
	}
	"
rocket_body="
	int argc=1;
	char** argv=0;
	Rocket::Core::Initialise();
	Rocket::Core::Shutdown();
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$rocket_prologue],[$rocket_body])],
  [AC_MSG_RESULT([yes]) 
  AC_DEFINE([HAVE_ROCKET], 1, [libRocket enabled])],
  [AC_MSG_RESULT([no])
  required_libraries="${required_libraries} 'libRocket'"]
)
###Eigen###
EIGEN_LDFLAGS=""
EIGEN_LIBS=""
###Panda3d###
AC_MSG_CHECKING([for Panda3d libs])
PANDA3D_LDFLAGS="-L/usr/lib/panda3d -L/usr/lib64/panda3d -L/usr/local/lib/panda3d"
PANDA3D_LIBS="-lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress -lp3dtoolconfig \
-lp3pystub -lp3dtool -lp3direct -lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision \
-lpandagl -lpandaskel -lp3ptloader -l$2"	
LDFLAGS="${PANDA3D_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${PANDA3D_LIBS} ${LIBS_CMDLINE}"
panda3d_prologue="
	class PandaFramework
	{
	public:
		PandaFramework(){}
		void open_framework(int&,char**&){}
		void close_framework(){}
	};
	"
panda3d_body="
	int argc=1;
	char** argv=0;
  	PandaFramework framework;
  	framework.open_framework(argc, argv);
  	framework.close_framework()
  	"  	
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([$panda3d_prologue],[$panda3d_body])],
  [AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_PANDA3D], 1, [Panda3d SDK enabled])],
  [AC_MSG_RESULT([no])
  required_libraries="${required_libraries} 'Panda3d'"]
)
# LIBRARIES checks result
if test -n "${required_libraries}" ; then
	AC_MSG_ERROR([
	-----------------------------------------------
	The ${required_libraries} 
	library packages are required to build Ely. 
	Stopping...
	Check 'config.log' for more information.
	-----------------------------------------------])
fi

# check HEADERS first from cmd line specified include
AC_MSG_NOTICE([searching for Ely dependency development packages...])
required_headers=
###Bullet###
BULLET_CPPFLAGS="-I/usr/include/bullet -I/usr/local/include/bullet"
CPPFLAGS="${BULLET_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADER([Bullet-C-Api.h],[],[required_headers="${required_headers}'Bullet'"])
###Recast Navigation###
RN_CPPFLAGS="-I/usr/include/librecastnavigation-dev -I/usr/local/include/librecastnavigation-dev"
CPPFLAGS="${RN_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADER([Recast.h],[],[required_headers="${required_headers} 'Recast Navigation'"])
###OpenSteer###
OS_CPPFLAGS="-I/usr/include/OpenSteer -I/usr/local/include/OpenSteer"
CPPFLAGS="${OS_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADER([SteerLibrary.h],[],[required_headers="${required_headers} 'OpenSteer'"])
###libRocket###
ROCKET_CPPFLAGS="-I/usr/include/Rocket -I/usr/local/include/Rocket"
CPPFLAGS="${ROCKET_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADER([Core.h],[],[required_headers="${required_headers} 'libRocket'"])
###Eigen###
EIGEN_CPPFLAGS="-I/usr/include/eigen3 -I/usr/local/include/eigen3 -I/usr/include/eigen2 -I/usr/local/include/eigen2"
CPPFLAGS="${EIGEN_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADER([Eigen/Dense],[],[required_headers="${required_headers} 'Eigen'"])
###Panda3d###
PANDA3D_CPPFLAGS="-I/usr/include/panda3d -I/usr/local/include/panda3d -I$1 ${EIGEN_CPPFLAGS}"
CPPFLAGS="${PANDA3D_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADER([pandaFramework.h],[],[required_headers="${required_headers} 'Panda3d'"])
# HEADERS checks result
if test -n "${required_headers}" ; then
	AC_MSG_ERROR([
	-----------------------------------------------
	The ${required_headers} 
	development	packages are required to build Ely.
	Stopping...
	Check 'config.log' for more information.
	-----------------------------------------------])
fi

#ELY_DEPS flags
ELY_DEPS_CPPFLAGS="${PANDA3D_CPPFLAGS} ${BULLET_CPPFLAGS} ${RN_CPPFLAGS} ${OS_CPPFLAGS} ${ROCKET_CPPFLAGS}"
ELY_DEPS_LDFLAGS="${PANDA3D_LDFLAGS} ${BULLET_LDFLAGS} ${RN_LDFLAGS} ${OS_LDFLAGS} ${ROCKET_LDFLAGS}"
ELY_DEPS_LIBS="${PANDA3D_LIBS} ${BULLET_LIBS} ${RN_LIBS} ${OS_LIBS} ${ROCKET_LIBS}"
#
CPPFLAGS=${CPPFLAGS_CMDLINE}
LDFLAGS=${LDFLAGS_CMDLINE}
LIBS=${LIBS_CMDLINE}
])# ELY_DEPS_SEARCH
