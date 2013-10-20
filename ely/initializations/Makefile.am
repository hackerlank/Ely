AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

pkglib_LTLIBRARIES = initializations.la
initializations_la_SOURCES = \
		common_configs.cpp common_configs.h \
		defs/Actor1.cpp \
		defs/camera.cpp \
		defs/EnvironmentObjects.cpp \
		defs/player0.cpp \
		defs/SoftObjects.cpp \
		defs/Steerer1.cpp

initializations_la_LDFLAGS = -module -avoid-version
