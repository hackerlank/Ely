ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
	-I$(srcdir)/../../../physics/source \
	-I$(srcdir)/../../../physics/source/support \
	-I/usr/include/panda3d -I/usr/include/eigen3 \
	-I/usr/include/bullet -I/usr/local/include/bullet -DELY_DEBUG=

AM_CXXFLAGS = -O0 -g3 -Wall -Wno-reorder -fmessage-length=0 -std=c++11
AM_LDFLAGS = -L/usr/lib/x86_64-linux-gnu/panda3d

LDADD = -lp3framework -lpanda -lpandaexpress -lp3dtool -lp3dtoolconfig \
	-lp3pystub -lXxf86dga -lpython2.7 -lpandaphysics -lpthread \
	-lpandabullet -lBulletDynamics -lLinearMath -lBulletCollision \
	-lBulletFileLoader -lBulletInverseDynamics -lBulletInverseDynamicsUtils \
	-lBulletSoftBody -lConvexDecomposition -lGIMPACTUtils -lHACD 

noinst_PROGRAMS = physics #physics_gprof physics_gcov

BUILT_SOURCES = data.cpp

substDataDir = sed -e 's|@sampledatadir[@]|$(srcdir)/../../data|g'

data.cpp: $(srcdir)/data.cpp.in Makefile
	$(substDataDir) $(srcdir)/$@.in > $@
	
EXTRA_DIST = data.cpp.in

common_sources = \
	data.cpp \
	$(srcdir)/../../../common/source/commonTools.cxx \
	$(srcdir)/../../../physics/source/gamePhysicsManager.cxx \
	$(srcdir)/../../../physics/source/btRigidBody.cxx
#	$(srcdir)/../../../physics/source/btSoftBody.cxx
#	$(srcdir)/../../../physics/source/btGhost.cxx	

#physics
physics_SOURCES = \
	physics.cpp

nodist_physics_SOURCES = $(common_sources)

###PROFILE BUILDS###
PROFILE_CPPFLAGS = $(AM_CPPFLAGS) -UOS_DEBUG
GPROF_CXXFLAGS = $(AM_CXXFLAGS) -pg -g -Wextra
GCOV_CXXFLAGS = $(AM_CXXFLAGS) -fprofile-arcs -ftest-coverage
GCOV_LDADD = $(LDADD) -lgcov

#physics_gprof physics_gcov
physics_gprof_CPPFLAGS = $(PROFILE_CPPFLAGS)
physics_gcov_CPPFLAGS = $(PROFILE_CPPFLAGS)
physics_gprof_CXXFLAGS = $(GPROF_CXXFLAGS)
physics_gcov_CXXFLAGS = $(GCOV_CXXFLAGS)
physics_gcov_LDADD = $(GCOV_LDADD)

physics_gprof_SOURCES = \
	physics.cpp
physics_gcov_SOURCES = \
	physics.cpp

nodist_physics_gprof_SOURCES = $(common_sources)
nodist_physics_gcov_SOURCES = $(common_sources)

CLEANFILES =  data.cpp *.boo *.gcno
