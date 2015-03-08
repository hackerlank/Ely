AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/elygame $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

AUTOMAKE_OPTIONS = subdir-objects

pkglib_LTLIBRARIES = instanceupdates.la
instanceupdates_la_SOURCES = \
	common_configs.cpp \
	defs/Character_updt.cpp

noinst_HEADERS = \
	common_configs.h

instanceupdates_la_LDFLAGS = -module -avoid-version \
	$(AM_LDFLAGS)

instanceupdates_la_LIBADD = \
	$(ELY_LIBS)
