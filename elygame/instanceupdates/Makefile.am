AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/elygame $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

pkglib_LTLIBRARIES = instanceupdates.la
instanceupdates_la_SOURCES = \
		common_configs.cpp \
		defs/Character_updt.cpp

noinst_HEADERS = \
		common_configs.h

instanceupdates_la_LDFLAGS = -module -avoid-version
