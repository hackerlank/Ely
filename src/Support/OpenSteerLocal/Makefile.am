AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libOpenSteerLocal.la

#library sources
libOpenSteerLocal_la_SOURCES = \
	Draw.cpp \
	DrawMeshDrawer.cpp \
	SimpleVehicle.cpp
