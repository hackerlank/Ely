AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libSceneComponents.la

#library sources
libSceneComponents_la_SOURCES = \
	InstanceOf.cpp \
	InstanceOfTemplate.cpp \
	Model.cpp \
	ModelTemplate.cpp \
	NodePathWrapper.cpp \
	NodePathWrapperTemplate.cpp \
	Terrain.cpp \
	TerrainTemplate.cpp
