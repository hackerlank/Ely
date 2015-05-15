#last one must be .
SUBDIRS = \
	MemoryPool \
	OpenSteerLocal \
	RecastNavigationLocal \
	tinyxml2 \
	.

AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libSupport.la

#library sources
libSupport_la_SOURCES = \
	FSM.cpp \
	Picker.cpp \
	Raycaster.cpp

libSupport_la_LIBADD = \
	$(builddir)/MemoryPool/libMemoryPool.la \
	$(builddir)/OpenSteerLocal/libOpenSteerLocal.la \
	$(builddir)/RecastNavigationLocal/libRecastNavigationLocal.la \
	$(builddir)/tinyxml2/libtinyxml2.la
	
