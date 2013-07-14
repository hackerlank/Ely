# Ely training target
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)
LDADD = $(ELY_LIBS)

noinst_PROGRAMS = ely_training ely_recastnavigation ely_recastnavigation_character 

#ely_training
nodist_ely_training_SOURCES = \
		training.cpp \
		ai.cpp \
		anim.cpp \
		network.cpp \
		physics.cpp \
		threads.cpp \
		manual_example.cpp \
		draw_geometry.cpp \
		test_wo_local_scale.cpp
		
#recastnavigation
noinst_LTLIBRARIES = libeggToObj.la

pandatool_path = /REPOSITORY/KProjects/panda3d/pandatool/src

libeggToObj_la_SOURCES = \
		recastnavigation/eggToObj/eggToObj.cxx \
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
		recastnavigation/recastnavigation.cpp \
		recastnavigation/recastnavigation_data.cpp \
		recastnavigation/RN.cpp \
		recastnavigation/CrowdTool.cpp \
		recastnavigation/InputGeom.cpp \
		recastnavigation/SampleInterfaces.cpp \
		recastnavigation/PerfTimer.cpp \
		recastnavigation/MeshLoaderObj.cpp \
		recastnavigation/ChunkyTriMesh.cpp \
		recastnavigation/Sample.cpp \
		recastnavigation/Sample_SoloMesh.cpp \
		recastnavigation/Sample_TileMesh.cpp \
		recastnavigation/Sample_TempObstacles.cpp \
		recastnavigation/fastlz.c \
		recastnavigation/ValueHistory.cpp \
		recastnavigation/ConvexVolumeTool.cpp \
		recastnavigation/OffMeshConnectionTool.cpp \
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
