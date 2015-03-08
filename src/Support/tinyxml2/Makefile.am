AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	-iquote$(top_srcdir)/include/Support/tinyxlm2 \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libtinyxml2.a

#library sources
libtinyxml2_a_SOURCES = \
	tinyxml2.cpp
