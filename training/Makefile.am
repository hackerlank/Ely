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
		
ely_training_CPPFLAGS = \
		-I$(rn_path)/DebugUtils/Include \
		$(AM_CPPFLAGS)

ely_training_LDADD = \
		$(LDADD) \
		$(rn_path)/DebugUtils/.libs/DebugUtils.a
		
#recastnavigation
rn_path = /REPOSITORY/KProjects/WORKSPACE/recastnavigation

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
		$(top_srcdir)/src/Support/Raycaster.cpp
		
rn_common_CPPFLAGS = \
		-I$(rn_path)/DebugUtils/Include \
		-I$(rn_path)/Detour/Include \
		-I$(rn_path)/DetourCrowd/Include \
		-I$(rn_path)/DetourTileCache/Include \
		-I$(rn_path)/Recast/Include \
		$(AM_CPPFLAGS)

rn_common_LDADD = \
		$(LDADD) \
		$(rn_path)/DebugUtils/.libs/DebugUtils.a \
		$(rn_path)/Detour/.libs/Detour.a \
		$(rn_path)/DetourTileCache/.libs/DetourTileCache.a \
		$(rn_path)/Recast/.libs/Recast.a
		
#ely_recastnavigation
nodist_ely_recastnavigation_SOURCES = \
		$(rn_common_sources)

ely_recastnavigation_CPPFLAGS = \
		$(rn_common_CPPFLAGS)

ely_recastnavigation_LDADD = \
		$(rn_common_LDADD) \
		$(rn_path)/DetourCrowd/.libs/DetourCrowd.a

#ely_recastnavigation_character
nodist_ely_recastnavigation_character_SOURCES = \
		$(rn_common_sources)

ely_recastnavigation_character_CPPFLAGS = \
		$(rn_common_CPPFLAGS) \
		-DWITHCHARACTER

ely_recastnavigation_character_LDADD = \
		$(rn_common_LDADD) \
		$(rn_path)/DetourCrowd/.libs/DetourCrowdPhysics.a


