AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

pkglib_LTLIBRARIES = callbacks.la
callbacks_la_SOURCES = \
		common_configs.cpp common_configs.h \
		defs/activityActor.cpp \
		defs/activityCharacter.cpp \
		defs/controlCamera.cpp \
		defs/navMeshes.cpp \
		defs/steerPlugIns.cpp \
		defs/vehicleCar.cpp

callbacks_la_LDFLAGS = -module -avoid-version
