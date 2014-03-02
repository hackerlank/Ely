AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/tinyxml2 -iquote$(top_srcdir)/ely $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

pkglib_LTLIBRARIES = transitions.la
transitions_la_SOURCES = \
		common_configs.cpp common_configs.h \
		defs/Actor.cpp \
		defs/Character.cpp

transitions_la_LDFLAGS = -module -avoid-version
