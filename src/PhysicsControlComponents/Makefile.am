AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libPhysicsControlComponents.a

#library sources
libPhysicsControlComponents_a_SOURCES = \
	CharacterController.cpp \
	CharacterControllerTemplate.cpp \
	Vehicle.cpp \
	VehicleTemplate.cpp
