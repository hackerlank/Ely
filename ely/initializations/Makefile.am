AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/tinyxml2 -iquote$(top_srcdir)/ely -iquote$(top_builddir)/ely $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

pkglib_LTLIBRARIES = initializations.la
initializations_la_SOURCES = \
		common_configs.cpp common_configs.h \
		defs/Actor1.cpp \
		defs/camera.cpp \
		defs/elyGame.cpp \
		defs/EnvironmentObjects.cpp \
		defs/player0.cpp \
		defs/SoftObjects.cpp \
		defs/Steerer1.cpp

initializations_la_LDFLAGS = -module -avoid-version
