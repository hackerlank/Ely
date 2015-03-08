AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libObjectModel.la

#library sources
libObjectModel_la_SOURCES = \
	Component.cpp \
	ComponentTemplate.cpp \
	ComponentTemplateManager.cpp \
	Object.cpp \
	ObjectTemplate.cpp \
	ObjectTemplateManager.cpp
