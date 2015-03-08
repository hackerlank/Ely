AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libCommonComponents.la

#library sources
libCommonComponents_la_SOURCES = \
	Default.cpp \
	DefaultTemplate.cpp \
	GameConfig.cpp \
	GameConfigTemplate.cpp
