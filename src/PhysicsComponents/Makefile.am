AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libPhysicsComponents.la

#library sources
libPhysicsComponents_la_SOURCES = \
	Ghost.cpp \
	RigidBody.cpp \
	SoftBody.cpp
