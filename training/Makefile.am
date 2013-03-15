# Ely training target
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

noinst_PROGRAMS = ely_training ely_recastnavigation

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

#ely_recastnavigation
nodist_ely_recastnavigation_SOURCES = \
		recastnavigation.cpp

rn_path = /REPOSITORY/KProjects/WORKSPACE/recastnavigation

ely_recastnavigation_CPPFLAGS = \
		-I$(rn_path)/DebugUtils/Include \
		-I$(rn_path)/Detour/Include \
		-I$(rn_path)/DetourCrowd/Include \
		-I$(rn_path)/DetourTileCache/Include \
		-I$(rn_path)/Recast/Include \
		-I$(rn_path)/RecastDemo/Include \
		$(AM_CPPFLAGS)
ely_recastnavigation_LDADD = \
		$(ELY_LIBS) \
		$(rn_path)/DebugUtils/.libs/DebugUtils.a \
		$(rn_path)/Detour/.libs/Detour.a \
		$(rn_path)/DetourCrowd/.libs/DetourCrowd.a \
		$(rn_path)/DetourTileCache/.libs/DetourTileCache.a \
		$(rn_path)/Recast/.libs/Recast.a
		
		