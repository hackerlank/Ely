AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

pkglib_LTLIBRARIES = initializations.la
initializations_la_SOURCES = \
		common_configs.cpp common_configs.h \
		defs/initialization_defs.cpp 

initializations_la_LDFLAGS = -module -avoid-version
