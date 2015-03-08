AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libPhysicsControlComponents.la

#library sources
libPhysicsControlComponents_la_SOURCES = \
	CharacterController.cpp \
	CharacterControllerTemplate.cpp \
	Vehicle.cpp \
	VehicleTemplate.cpp
