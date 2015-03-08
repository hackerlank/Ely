AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libPhysicsComponents.la

#library sources
libPhysicsComponents_la_SOURCES = \
	Ghost.cpp \
	GhostTemplate.cpp \
	RigidBody.cpp \
	RigidBodyTemplate.cpp \
	SoftBody.cpp \
	SoftBodyTemplate.cpp
