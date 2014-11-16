AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/include/Support/tinyxlm2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

#ltlibraries
pkglib_LTLIBRARIES = \
		libphysicscomponents.la

#ltlibraries sources
libphysicscomponents_la_SOURCES = \
		Ghost.cpp \
		GhostTemplate.cpp \
		RigidBody.cpp \
		RigidBodyTemplate.cpp \
		SoftBody.cpp \
		SoftBodyTemplate.cpp
