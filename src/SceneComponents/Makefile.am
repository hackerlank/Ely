AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libSceneComponents.a

#library sources
libSceneComponents_a_SOURCES = \
	InstanceOf.cpp \
	InstanceOfTemplate.cpp \
	Model.cpp \
	ModelTemplate.cpp \
	NodePathWrapper.cpp \
	NodePathWrapperTemplate.cpp \
	Terrain.cpp \
	TerrainTemplate.cpp
