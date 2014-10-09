AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/elygame $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

pkglib_LTLIBRARIES = callbacks.la
callbacks_la_SOURCES = \
		common_configs.cpp \
		defs/Actor_clbk.cpp \
		defs/Camera_clbk.cpp \
		defs/Car_clbk.cpp \
		defs/Character_clbk.cpp \
		defs/Game_clbk.cpp \
		defs/OpenSteerPlugIn_clbk.cpp \
		defs/RecastNavMesh_clbk.cpp

noinst_HEADERS = \
		common_configs.h
		
callbacks_la_LDFLAGS = -module -avoid-version
