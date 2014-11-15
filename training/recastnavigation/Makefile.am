# Ely recastnavigation targets
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

AUTOMAKE_OPTIONS = subdir-objects

LDADD = -lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
	-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct -lpandabullet \
	-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision -lpandagl \
	-lpandaode -lpandaphysics -lpandaskel -lp3ptloader -lrecastnavigation

noinst_PROGRAMS = ely_recastnavigation ely_recastnavigation_character 
		
#recastnavigation
noinst_LTLIBRARIES = libeggToObj.la

pandatool_path = /REPOSITORY/KProjects/panda3d/pandatool/src

libeggToObj_la_SOURCES = \
		eggToObj/eggToObj.cxx \
		$(pandatool_path)/progbase/programBase.cxx \
		$(pandatool_path)/progbase/withOutputFile.cxx \
		$(pandatool_path)/progbase/wordWrapStream.cxx \
		$(pandatool_path)/progbase/wordWrapStreamBuf.cxx \
		$(pandatool_path)/eggbase/eggSingleBase.cxx \
		$(pandatool_path)/eggbase/eggToSomething.cxx \
		$(pandatool_path)/eggbase/eggReader.cxx \
		$(pandatool_path)/eggbase/eggWriter.cxx \
		$(pandatool_path)/eggbase/eggBase.cxx \
		$(pandatool_path)/eggbase/eggFilter.cxx \
		$(pandatool_path)/eggbase/eggConverter.cxx

rn_common_sources = \
		recastnavigation.cpp \
		recastnavigation_data.cpp \
		RN.cpp \
		CrowdTool.cpp \
		InputGeom.cpp \
		SampleInterfaces.cpp \
		PerfTimer.cpp \
		MeshLoaderObj.cpp \
		ChunkyTriMesh.cpp \
		Sample.cpp \
		Sample_SoloMesh.cpp \
		Sample_TileMesh.cpp \
		Sample_TempObstacles.cpp \
		fastlz.c \
		ValueHistory.cpp \
		ConvexVolumeTool.cpp \
		OffMeshConnectionTool.cpp \
		$(top_srcdir)/src/Support/Raycaster.cpp
		
#ely_recastnavigation
nodist_ely_recastnavigation_SOURCES = \
		$(rn_common_sources)

ely_recastnavigation_LDADD = \
		$(LDADD) \
		libeggToObj.la
		
#ely_recastnavigation_character
nodist_ely_recastnavigation_character_SOURCES = \
		$(rn_common_sources)

ely_recastnavigation_character_CPPFLAGS = \
		$(AM_CPPFLAGS) \
		-DWITHCHARACTER
		
ely_recastnavigation_character_LDADD = \
		$(ELY_RN_PHYSICS_LIBS) \
		libeggToObj.la
