AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/elygame -iquote$(top_builddir)/elygame $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

AUTOMAKE_OPTIONS = subdir-objects

pkglib_LTLIBRARIES = initializations.la
initializations_la_SOURCES = \
	common_configs.cpp \
	defs/Actor_init.cpp \
	defs/Camera_init.cpp \
	defs/Game_init.cpp \
	defs/StaticObjects_init.cpp \
	defs/Character_init.cpp \
	defs/SoftObject_init.cpp \
	defs/OpenSteerPlugIn_init.cpp \
	defs/RecastNavMesh_init.cpp

noinst_HEADERS = \
	common_configs.h \
	defs/Game_init.h

initializations_la_LDFLAGS = -module -avoid-version \
	$(AM_LDFLAGS)

initializations_la_LIBADD = \
	$(ELY_LIBS)
		
