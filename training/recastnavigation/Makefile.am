# Ely recastnavigation targets
AM_CPPFLAGS = -DTESTING -I/usr/include/panda3d -I/usr/include/eigen3 \
		-I/usr/include/bullet -I/usr/include/recastnavigation
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

#AUTOMAKE_OPTIONS = subdir-objects

LDADD = -lp3direct -lp3dtoolconfig -lp3dtool -lp3ffmpeg -lp3framework \
		-lp3interrogatedb -lp3openal_audio -lp3ptloader -lp3rocket \
		-lp3tinydisplay -lp3vision -lpandaai -lpandabullet -lpandaegg \
		-lpandaexpress -lpandafx -lpandagl -lpandaode -lpandaphysics \
		-lpandaskel -lpanda -lp3pystub -lXxf86dga -lpandagl -lpython2.7 \
		-lDebugUtils -lDetour -lDetourCrowd -lDetourTileCache -lRecast \
		-lBulletDynamics -lBulletCollision -lLinearMath -lBulletSoftBody

#noinst_PROGRAMS = ely_recastnavigation ely_recastnavigation_character 
noinst_PROGRAMS = ely_recastnavigation
		
#recastnavigation
noinst_LIBRARIES = libeggToObj.a

pandatool_path = /REPOSITORY/KProjects/panda3d/pandatool/src

libeggToObj_a_SOURCES = \
		eggToObj/eggToObj.cxx \
		$(pandatool_path)/progbase/programBase.cxx \
		$(pandatool_path)/eggbase/eggSingleBase.cxx \
		$(pandatool_path)/eggbase/eggBase.cxx \
		$(pandatool_path)/progbase/withOutputFile.cxx \
		$(pandatool_path)/eggbase/eggToSomething.cxx \
		$(pandatool_path)/progbase/wordWrapStream.cxx \
		$(pandatool_path)/progbase/wordWrapStreamBuf.cxx \
		$(pandatool_path)/objegg/eggToObjConverter.cxx \
		$(pandatool_path)/eggbase/eggConverter.cxx \
		$(pandatool_path)/eggbase/eggFilter.cxx \
		$(pandatool_path)/eggbase/eggReader.cxx \
		$(pandatool_path)/eggbase/eggWriter.cxx \
		$(pandatool_path)/pandatoolbase/pathReplace.cxx \
		$(pandatool_path)/pandatoolbase/distanceUnit.cxx \
		$(pandatool_path)/pandatoolbase/pathStore.cxx \
		$(pandatool_path)/converter/eggToSomethingConverter.cxx
		
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
		Raycaster.cpp \
		Tools.cpp
		
#ely_recastnavigation
nodist_ely_recastnavigation_SOURCES = \
		$(rn_common_sources)

ely_recastnavigation_LDADD = \
		$(LDADD) \
		libeggToObj.a
		
#ely_recastnavigation_character
#nodist_ely_recastnavigation_character_SOURCES = \
#		$(rn_common_sources)

#ely_recastnavigation_character_CPPFLAGS = \
#		$(AM_CPPFLAGS) \
#		-DWITHCHARACTER
		
#ely_recastnavigation_character_LDADD = \
#		$(ELY_RN_PHYSICS_LIBS) \
#		libeggToObj.la
