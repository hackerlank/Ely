#last one must be .
SUBDIRS = \
	BulletLocal \
	LibRocketLocal \
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
	$(builddir)/BulletLocal/libBulletLocal.la \
	$(builddir)/LibRocketLocal/libRocketLocal.la \
	$(builddir)/MemoryPool/libMemoryPool.la \
	$(builddir)/OpenSteerLocal/libOpenSteerLocal.la \
	$(builddir)/RecastNavigationLocal/libRecastNavigationLocal.la \
	$(builddir)/tinyxml2/libtinyxml2.la
	
