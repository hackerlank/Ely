# recastnavigation tests
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
	-I$(srcdir)/../../../source \
	-I$(srcdir)/../../../source/support \
	-I/usr/include/panda3d -I/usr/include/eigen3 \
	-DELY_DEBUG=

AM_CXXFLAGS = -O0 -g3 -Wall -Wno-reorder -fmessage-length=0 -std=c++11
AM_LDFLAGS = -L/usr/lib/x86_64-linux-gnu/panda3d

LDADD = -lp3framework -lpanda -lpandaexpress -lp3dtool -lp3dtoolconfig \
		-lp3pystub -lXxf86dga -lpython2.7 -lpandaphysics -lpthread

noinst_PROGRAMS = basic #basic_gprof basic_gcov

BUILT_SOURCES = data.cpp

substDataDir = sed -e 's|@sampledatadir[@]|$(srcdir)/../data|g'

data.cpp: $(srcdir)/data.cpp.in Makefile
	$(substDataDir) $(srcdir)/$@.in > $@
	
EXTRA_DIST = data.cpp.in

common_sources = \
	data.cpp \
	$(srcdir)/../../../control/source/controlManager.cxx \
	$(srcdir)/../../../control/source/controlTools.cxx \
	$(srcdir)/../../../control/source/p3Driver.cxx

#basic
basic_SOURCES = \
	basic.cpp

nodist_basic_SOURCES = $(common_sources)

#callback_test
callback_test_SOURCES = \
	callback_test.cpp

nodist_callback_test_SOURCES = $(common_sources)

###PROFILE BUILDS###
PROFILE_CPPFLAGS = $(AM_CPPFLAGS) -UOS_DEBUG
GPROF_CXXFLAGS = $(AM_CXXFLAGS) -pg -g -Wextra
GCOV_CXXFLAGS = $(AM_CXXFLAGS) -fprofile-arcs -ftest-coverage
GCOV_LDADD = $(LDADD) -lgcov

#basic_gprof soccer_gcov
basic_gprof_CPPFLAGS = $(PROFILE_CPPFLAGS)
basic_gcov_CPPFLAGS = $(PROFILE_CPPFLAGS)
basic_gprof_CXXFLAGS = $(GPROF_CXXFLAGS)
basic_gcov_CXXFLAGS = $(GCOV_CXXFLAGS)
basic_gcov_LDADD = $(GCOV_LDADD)

basic_gprof_SOURCES = \
	basic.cpp
basicr_gcov_SOURCES = \
	basic.cpp

nodist_basic_gprof_SOURCES = $(common_sources)
nodist_basic_gcov_SOURCES = $(common_sources)

CLEANFILES = data.cpp
