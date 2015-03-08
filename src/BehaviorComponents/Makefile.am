AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libBehaviorComponents.la

#library sources
libBehaviorComponents_la_SOURCES = \
	Activity.cpp \
	ActivityTemplate.cpp
