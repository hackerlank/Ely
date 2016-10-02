ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
	-I$(srcdir)/../../../audio/source \
	-I$(srcdir)/../../../audio/source/support \
	-I$(srcdir)/../../../control/source \
	-I$(srcdir)/../../../control/source/support \
	-I/usr/include/panda3d -I/usr/include/eigen3 \
	-DELY_DEBUG=

AM_CXXFLAGS = -O0 -g3 -Wall -Wno-reorder -fmessage-length=0 -std=c++11
AM_LDFLAGS = -L/usr/lib/x86_64-linux-gnu/panda3d

LDADD = -lp3framework -lpanda -lpandaexpress -lp3dtool -lp3dtoolconfig \
		-lp3pystub -lXxf86dga -lpython2.7 -lpandaphysics -lpthread

noinst_PROGRAMS = audio #audio_gprof audio_gcov

BUILT_SOURCES = data.cpp

substDataDir = sed -e 's|@sampledatadir[@]|$(srcdir)/../../data|g'

data.cpp: $(srcdir)/data.cpp.in Makefile
	$(substDataDir) $(srcdir)/$@.in > $@
	
EXTRA_DIST = data.cpp.in

common_sources = \
	data.cpp \
	$(srcdir)/../../../audio/source/gameAudioManager.cxx \
	$(srcdir)/../../../audio/source/audioTools.cxx \
	$(srcdir)/../../../audio/source/p3Sound3d.cxx \
	$(srcdir)/../../../audio/source/p3Listener.cxx \
	$(srcdir)/../../../control/source/gameControlManager.cxx \
	$(srcdir)/../../../control/source/controlTools.cxx \
	$(srcdir)/../../../control/source/p3Driver.cxx \
	$(srcdir)/../../../control/source/p3Chaser.cxx	

#audio
audio_SOURCES = \
	audio.cpp

nodist_audio_SOURCES = $(common_sources)

###PROFILE BUILDS###
PROFILE_CPPFLAGS = $(AM_CPPFLAGS) -UOS_DEBUG
GPROF_CXXFLAGS = $(AM_CXXFLAGS) -pg -g -Wextra
GCOV_CXXFLAGS = $(AM_CXXFLAGS) -fprofile-arcs -ftest-coverage
GCOV_LDADD = $(LDADD) -lgcov

#audio_gprof audio_gcov
audio_gprof_CPPFLAGS = $(PROFILE_CPPFLAGS)
audio_gcov_CPPFLAGS = $(PROFILE_CPPFLAGS)
audio_gprof_CXXFLAGS = $(GPROF_CXXFLAGS)
audio_gcov_CXXFLAGS = $(GCOV_CXXFLAGS)
audio_gcov_LDADD = $(GCOV_LDADD)

audio_gprof_SOURCES = \
	audio.cpp
audio_gcov_SOURCES = \
	audio.cpp

nodist_audio_gprof_SOURCES = $(common_sources)
nodist_audio_gcov_SOURCES = $(common_sources)

CLEANFILES = data.cpp *.gcno
