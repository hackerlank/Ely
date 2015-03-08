AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libObjectModel.a

#library sources
libObjectModel_a_SOURCES = \
	Component.cpp \
	ComponentTemplate.cpp \
	ComponentTemplateManager.cpp \
	Object.cpp \
	ObjectTemplate.cpp \
	ObjectTemplateManager.cpp
