# Ely training target
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

noinst_PROGRAMS = ely_training ely_recastnavigation ely_recastnavigation_nocrowd

#ely_training
nodist_ely_training_SOURCES = \
		training.cpp \
		ai.cpp \
		anim.cpp \
		network.cpp \
		physics.cpp \
		threads.cpp \
		manual_example.cpp
		
ely_training_LDADD = \
		$(ELY_LIBS)

rn_common_sources = \
		recastnavigation/InputGeom.cpp \
		recastnavigation/SampleInterfaces.cpp \
		recastnavigation/PerfTimer.cpp \
		recastnavigation/MeshLoaderObj.cpp \
		recastnavigation/ChunkyTriMesh.cpp \
		recastnavigation/Sample.cpp \
		recastnavigation/Sample_SoloMesh.cpp \
		recastnavigation/ValueHistory.cpp

#ely_recastnavigation
nodist_ely_recastnavigation_SOURCES = \
		recastnavigation/recastnavigation.cpp \
		recastnavigation/CrowdTool.cpp \
		$(rn_common_sources)

#ely_recastnavigation_nocrowd
nodist_ely_recastnavigation_nocrowd_SOURCES = \
		recastnavigation/recastnavigation-nocrowd.cpp \
		$(rn_common_sources)

rn_path = /REPOSITORY/KProjects/WORKSPACE/recastnavigation

rn_common_cppflags = \
		-I$(rn_path)/DebugUtils/Include \
		-I$(rn_path)/Detour/Include \
		-I$(rn_path)/DetourCrowd/Include \
		-I$(rn_path)/DetourTileCache/Include \
		-I$(rn_path)/Recast/Include \
		$(AM_CPPFLAGS)
rn_common_ldadd = \
		$(ELY_LIBS) \
		$(rn_path)/DebugUtils/.libs/DebugUtils.a \
		$(rn_path)/Detour/.libs/Detour.a \
		$(rn_path)/DetourCrowd/.libs/DetourCrowd.a \
		$(rn_path)/DetourTileCache/.libs/DetourTileCache.a \
		$(rn_path)/Recast/.libs/Recast.a

ely_recastnavigation_CPPFLAGS = $(rn_common_cppflags) 
ely_recastnavigation_nocrowd_CPPFLAGS = $(rn_common_cppflags)

ely_recastnavigation_LDADD = $(rn_common_ldadd)
ely_recastnavigation_nocrowd_LDADD = $(rn_common_ldadd)
		
		