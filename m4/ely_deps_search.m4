# ELY_DEPS_SEARCH
# --------------
# Defines: ELY_DEPS_CPPFLAGS, ELY_DEPS_LDFLAGS, ELY_DEPS_LIBS, ELY_FLEXCPP
# Argument: expected PYTHON_CPPFLAGS
# Searches SDK first on cmd line flags then on std locations.
# This macro calls:
#	AC_SUBST(ELY_DEPS_CPPFLAGS)
#	AC_SUBST(ELY_DEPS_LDFLAGS)
#	AC_SUBST(ELY_DEPS_LIBS)
# and AC_SUBST for the equivalent CPPFLAGS,LDFLAGS,LIBS
# related to 
#	Bullet (BULLET_)
#	libRocket (ROCKET_)
#	OpenSteer (OPENSTEER_)
#	RecastNavigation (RECAST_)
#	Panda3d (PANDA3D_) 
#

#dnl $1=library $2=package
AC_DEFUN([GET_LDFLAGS], 
[
ld_cmd=$(which ld)
LDFLAGSFOUND=""
path_found=no
#dnl search without pkg prefix
${ld_cmd} --warn-unresolved-symbols -l$1 -o /dev/null 2>/dev/null && path_found=yes
#dnl search with pkg prefix ($2) if not already found
AS_IF([test "x${path_found}" != xyes],
	[
	libpathsgrep=$(${ld_cmd} --verbose | grep SEARCH_DIR)
	OLDIFS=$IFS
	IFS=';'
	for libpathstr in ${libpathsgrep}
	do
		libpath=$(echo ${libpathstr} | cut -f2 -d"\"" | cut -f2 -d"=")
		${ld_cmd} -nostdlib --warn-unresolved-symbols -L${libpath}/$2 -l$1 -o /dev/null 2>/dev/null \
			&& LDFLAGSFOUND="-L${libpath}/$2" \
			&& break
	done
	IFS=$OLDIFS
	])
])# GET_LDFLAGS

AC_DEFUN([ELY_DEPS_SEARCH], 
[
CPPFLAGS_CMDLINE=${CPPFLAGS}
LDFLAGS_CMDLINE=${LDFLAGS}
LIBS_CMDLINE=${LIBS}
# checks for PROGRAMS
AC_MSG_NOTICE([searching for Ely dependency programs...])
required_programs=
###flexc++###
AC_CHECK_PROGS([ELY_FLEXCPP], [flexc++],)
if test -z "${ELY_FLEXCPP}" ; then
	required_programs=${required_programs}" 'flexc++' "
fi
# check LIBRARIES first from cmd line specified ones
AC_MSG_NOTICE([searching for Ely dependency library packages...])
CPPFLAGS=
required_libraries=
###Bullet###
AC_MSG_CHECKING([for Bullet libs])
GET_LDFLAGS([BulletCollision], [bullet])
BULLET_LDFLAGS="${LDFLAGSFOUND}"
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
  AC_DEFINE([HAVE_BULLET], 1, [Bullet enabled])],
  [AC_MSG_RESULT([no])
  required_libraries="${required_libraries}'Bullet'"]
)
###RecastNavigation###
AC_MSG_CHECKING([for RecastNavigation libs])
GET_LDFLAGS([Recast], [recastnavigation])
RECAST_LDFLAGS="${LDFLAGSFOUND}"
RECAST_LIBS="-lDebugUtils -lDetour -lDetourCrowd -lDetourTileCache -lRecast"			
LDFLAGS="${RECAST_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${RECAST_LIBS} ${LIBS_CMDLINE}"
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
  AC_DEFINE([HAVE_RECASTNAVIGATION], 1, [RecastNavigation enabled])],
  [AC_MSG_RESULT([no])
  required_libraries="${required_libraries} 'RecastNavigation'"]
)
###OpenSteer###
# check libraries first from cmd line specified ones
AC_MSG_CHECKING([for OpenSteer libs])
GET_LDFLAGS([OpenSteer], [OpenSteer])
OPENSTEER_LDFLAGS="${LDFLAGSFOUND}"
OPENSTEER_LIBS="-lOpenSteer"			
LDFLAGS="${OPENSTEER_LDFLAGS} ${LDFLAGS_CMDLINE}"
LIBS="${OPENSTEER_LIBS} ${LIBS_CMDLINE}"
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
  AC_DEFINE([HAVE_OPENSTEER], 1, [OpenSteer enabled])],
  [AC_MSG_RESULT([no])
  required_libraries="${required_libraries} 'OpenSteer'"]
)
###libRocket###
AC_MSG_CHECKING([for libRocket libs])
GET_LDFLAGS([RocketCore], [rocket])
ROCKET_LDFLAGS="${LDFLAGSFOUND}"
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
GET_LDFLAGS([p3framework], [panda3d])
PANDA3D_LDFLAGS="${LDFLAGSFOUND}"
PANDA3D_LIBS="-lp3framework -lpanda -lpandaexpress -lp3dtool \
-lp3dtoolconfig -lp3pystub -lpandabullet -lp3rocket -lXxf86dga -l$2"	
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

# check HEADERS first from cmd line specified include
AC_MSG_NOTICE([searching for Ely dependency development packages...])
required_headers=
###Bullet###
BULLET_CPPFLAGS="-I/usr/include/bullet -I/usr/local/include/bullet"
CPPFLAGS="${BULLET_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADER([btBulletCollisionCommon.h],[],[required_headers="${required_headers}'Bullet'"])
###RecastNavigation###
RECAST_CPPFLAGS="-I/usr/include/recastnavigation -I/usr/local/include/recastnavigation"
CPPFLAGS="${RECAST_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
AC_CHECK_HEADER([Recast.h],[],[required_headers="${required_headers} 'RecastNavigation'"])
###OpenSteer###
OPENSTEER_CPPFLAGS="-I/usr/include/OpenSteer -I/usr/local/include/OpenSteer"
CPPFLAGS="${OPENSTEER_CPPFLAGS} ${CPPFLAGS_CMDLINE}"
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
#
STOP_ON_CHECKS_ERR([$required_programs],[$required_libraries],[$required_headers],['Ely'])
#
ELY_DEPS_CPPFLAGS="${PANDA3D_CPPFLAGS} ${BULLET_CPPFLAGS} ${RECAST_CPPFLAGS} ${OPENSTEER_CPPFLAGS} ${ROCKET_CPPFLAGS}"
ELY_DEPS_LDFLAGS="${PANDA3D_LDFLAGS} ${BULLET_LDFLAGS} ${RECAST_LDFLAGS} ${OPENSTEER_LDFLAGS} ${ROCKET_LDFLAGS}"
ELY_DEPS_LIBS="${BULLET_LIBS} ${RECAST_LIBS} ${OPENSTEER_LIBS} ${ROCKET_LIBS} ${PANDA3D_LIBS}"
#Dependencies flags
AC_SUBST(ELY_DEPS_CPPFLAGS)
AC_SUBST(ELY_DEPS_LDFLAGS)
AC_SUBST(ELY_DEPS_LIBS)
AC_SUBST(BULLET_CPPFLAGS)
AC_SUBST(BULLET_LDFLAGS)
AC_SUBST(BULLET_LIBS)
AC_SUBST(RECAST_CPPFLAGS)
AC_SUBST(RECAST_LDFLAGS)
AC_SUBST(RECAST_LIBS)
AC_SUBST(OPENSTEER_CPPFLAGS)
AC_SUBST(OPENSTEER_LDFLAGS)
AC_SUBST(OPENSTEER_LIBS)
AC_SUBST(ROCKET_CPPFLAGS)
AC_SUBST(ROCKET_LDFLAGS)
AC_SUBST(ROCKET_LIBS)
AC_SUBST(PANDA3D_CPPFLAGS)
AC_SUBST(PANDA3D_LDFLAGS)
AC_SUBST(PANDA3D_LIBS)
#
CPPFLAGS=${CPPFLAGS_CMDLINE}
LDFLAGS=${LDFLAGS_CMDLINE}
LIBS=${LIBS_CMDLINE}
])# ELY_DEPS_SEARCH
