AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libAIComponents.la

#library sources
libAIComponents_la_SOURCES = \
	CrowdAgent.cpp \
	NavMesh.cpp \
	SteerPlugIn.cpp \
	SteerVehicle.cpp
