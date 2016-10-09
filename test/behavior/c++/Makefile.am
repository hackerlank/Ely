ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
	-I$(srcdir)/../../../behavior/source \
	-I$(srcdir)/../../../behavior/source/support \
	-I/usr/include/panda3d -I/usr/include/eigen3 \
	-DELY_DEBUG=

AM_CXXFLAGS = -O0 -g3 -Wall -Wno-reorder -fmessage-length=0 -std=c++11
AM_LDFLAGS = -L/usr/lib/x86_64-linux-gnu/panda3d

LDADD = -lp3framework -lpanda -lpandaexpress -lp3dtool -lp3dtoolconfig \
		-lp3pystub -lXxf86dga -lpython2.7 -lpandaphysics -lpthread

noinst_PROGRAMS = behavior #behavior_gprof behavior_gcov

BUILT_SOURCES = data.cpp

substDataDir = sed -e 's|@sampledatadir[@]|$(srcdir)/../../data|g'

data.cpp: $(srcdir)/data.cpp.in Makefile
	$(substDataDir) $(srcdir)/$@.in > $@
	
EXTRA_DIST = data.cpp.in

common_sources = \
	data.cpp \
	$(srcdir)/../../../common/source/commonTools.cxx \
	$(srcdir)/../../../behavior/source/gameBehaviorManager.cxx \
	$(srcdir)/../../../behavior/source/p3Activity.cxx \

#behavior
behavior_SOURCES = \
	behavior.cpp

nodist_behavior_SOURCES = $(common_sources)

###PROFILE BUILDS###
PROFILE_CPPFLAGS = $(AM_CPPFLAGS) -UOS_DEBUG
GPROF_CXXFLAGS = $(AM_CXXFLAGS) -pg -g -Wextra
GCOV_CXXFLAGS = $(AM_CXXFLAGS) -fprofile-arcs -ftest-coverage
GCOV_LDADD = $(LDADD) -lgcov

#behavior_gprof behavior_gcov
behavior_gprof_CPPFLAGS = $(PROFILE_CPPFLAGS)
behavior_gcov_CPPFLAGS = $(PROFILE_CPPFLAGS)
behavior_gprof_CXXFLAGS = $(GPROF_CXXFLAGS)
behavior_gcov_CXXFLAGS = $(GCOV_CXXFLAGS)
behavior_gcov_LDADD = $(GCOV_LDADD)

behavior_gprof_SOURCES = \
	behavior.cpp
behavior_gcov_SOURCES = \
	behavior.cpp

nodist_behavior_gprof_SOURCES = $(common_sources)
nodist_behavior_gcov_SOURCES = $(common_sources)

CLEANFILES =  data.cpp *.boo *.gcno
