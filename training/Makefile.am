# Ely training target
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

ltlibraries = \
		$(top_builddir)/src/libgame.la \
		$(top_builddir)/src/libaicomponents.la \
		$(top_builddir)/src/libaudiocomponents.la \
		$(top_builddir)/src/libbehaviorcomponents.la \
		$(top_builddir)/src/libcontrolcomponents.la \
		$(top_builddir)/src/libphysicscomponents.la \
		$(top_builddir)/src/libscenecomponents.la \
		$(top_builddir)/src/libobjectmodel.la \
		$(top_builddir)/src/libutilities.la

libraries = libsupport.a

noinst_PROGRAMS = training
nodist_training_SOURCES = \
		training.cpp \
		ai.cpp \
		anim.cpp \
		network.cpp \
		physics.cpp \
		threads.cpp
		
training_LDFLAGS = $(ELY_LDFLAGS)
training_LDADD = \
		$(LTLIBRARIES) \
		$(LIBRARIES) \
		$(ELY_LIBS)
