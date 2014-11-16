AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/elygame $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

AUTOMAKE_OPTIONS = subdir-objects

pkglib_LTLIBRARIES = transitions.la
transitions_la_SOURCES = \
		common_configs.cpp \
		defs/Actor_trns.cpp \
		defs/Character_trns.cpp

noinst_HEADERS = \
		common_configs.h

transitions_la_LDFLAGS = -module -avoid-version \
		$(AM_LDFLAGS)

transitions_la_LIBADD = \
		$(ELY_LIBS)
