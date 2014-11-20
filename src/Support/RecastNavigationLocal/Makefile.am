AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/include/Support/tinyxlm2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

#ltlibraries
pkglib_LTLIBRARIES = \
		librecastnavigation.la

#ltlibraries sources
librecastnavigation_la_SOURCES = \
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
