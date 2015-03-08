AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libAIComponents.la

#library sources
libAIComponents_la_SOURCES = \
	CrowdAgent.cpp \
	CrowdAgentTemplate.cpp \
	NavMesh.cpp \
	NavMeshTemplate.cpp \
	SteerPlugIn.cpp \
	SteerPlugInTemplate.cpp \
	SteerVehicle.cpp \
	SteerVehicleTemplate.cpp
