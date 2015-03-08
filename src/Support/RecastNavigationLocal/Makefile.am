AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libRecastNavigationLocal.la

#library sources
libRecastNavigationLocal_la_SOURCES = \
	InputGeom.cpp \
	ChunkyTriMesh.cpp \
	MeshLoaderObj.cpp \
	DebugInterfaces.cpp \
	PerfTimer.cpp \
	NavMeshType.cpp \
	NavMeshType_Tile.cpp \
	NavMeshType_Solo.cpp \
	NavMeshType_Obstacle.cpp \
	fastlz.c \
	ConvexVolumeTool.cpp \
	CrowdTool.cpp \
	OffMeshConnectionTool.cpp
