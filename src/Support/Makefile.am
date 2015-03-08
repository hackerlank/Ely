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

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libSupport.a

#library sources
libSupport_a_SOURCES = \
	FSM.cpp \
	Picker.cpp \
	Raycaster.cpp

libSupport_a_LIBADD = \
	$(builddir)/BulletLocal/libBulletLocal.a \
	$(builddir)/LibRocketLocal/libRocketLocal.a \
	$(builddir)/MemoryPool/libMemoryPool.a \
	$(builddir)/OpenSteerLocal/libOpenSteerLocal.a \
	$(builddir)/RecastNavigationLocal/libRecastNavigationLocal.a \
	$(builddir)/tinyxml2/libtinyxml2.a
	
