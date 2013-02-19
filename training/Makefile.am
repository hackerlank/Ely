# Ely training target
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

ltlibraries = \
		$(top_srcdir)/src/libgame.la \
		$(top_srcdir)/src/libaicomponents.la \
		$(top_srcdir)/src/libaudiocomponents.la \
		$(top_srcdir)/src/libbehaviorcomponents.la \
		$(top_srcdir)/src/libcontrolcomponents.la \
		$(top_srcdir)/src/libphysicscomponents.la \
		$(top_srcdir)/src/libscenecomponents.la \
		$(top_srcdir)/src/libobjectmodel.la \
		$(top_srcdir)/src/libutilities.la

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
